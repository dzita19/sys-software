#include "emulator.h"

#include <string.h>

FILE* emuin = 0;
unsigned int   registers[MACHINE_REG_COUNT];
unsigned int   program_status_word;
unsigned char  memory[MEMORY_SIZE];

unsigned int   interrupt_lines;

unsigned char  ireg[MAX_INSTR_LEN];
AsmInstruction instruction;
unsigned int   src_reg; // reg index for reg based addressing, payload for payload based addressing
unsigned int   dst_reg;
unsigned int   payload;
unsigned int   addr_mode;
unsigned int   src;
unsigned int*  dst;

Vector* exec_args = 0;

static int instr_counter = 0;

static void GenerateInterrupt(int interrupt){
  interrupt_lines |= 1 << interrupt;
}

#define SAFE_MEMORY_FETCH(address_expression, destination)\
  do{                                                     \
    if(address_expression >= MEMORY_SIZE){                \
      GenerateInterrupt(INTR_BUS_FAULT);                  \
      instruction = ASM_ERROR;                            \
      return;                                             \
    }                                                     \
    else{                                                 \
      destination = memory[address_expression];           \
    }                                                     \
    address_expression++;                                 \
  } while(0)

static void FetchInstruction(void){
  SAFE_MEMORY_FETCH(registers[INSTRUCTION_POINTER], ireg[0]);

  instruction = ASM_ERROR;
  for(int i = 0; i < ASM_INSTR_COUNT; i++){
    if(instr_machine_code[i] == ireg[0]){
      instruction = i;
      break;
    }
  }

  if(instruction == ASM_ERROR) {
    GenerateInterrupt(INTR_HARD_FAULT);
  }

  switch(instr_types[instruction]){
  case ASM_NOOP: {
  } break;
  case ASM_ONEOP: {
    SAFE_MEMORY_FETCH(registers[INSTRUCTION_POINTER], ireg[1]);
    
    src_reg = (ireg[1] >> 4) & 0x0f;
    dst_reg = (ireg[1] >> 0) & 0x0f;
  } break;
  
  case ASM_TWOOP: 
  case ASM_MOVEOP:
  case ASM_BRANCH: {
    SAFE_MEMORY_FETCH(registers[INSTRUCTION_POINTER], ireg[1]);
    src_reg = (ireg[1] >> 4) & 0x0f;
    dst_reg = (ireg[1] >> 0) & 0x0f;

    SAFE_MEMORY_FETCH(registers[INSTRUCTION_POINTER], ireg[2]);
    addr_mode = ireg[2];

    if((ireg[2] & 0x0f) == 0x00){
      payload = 0;
    }
    else if((ireg[2] & 0x0f) == 0x01){
      SAFE_MEMORY_FETCH(registers[INSTRUCTION_POINTER], ireg[3]);
      payload = (signed char)ireg[3];
    }
    else if((ireg[2] & 0x0f) == 0x02){
      SAFE_MEMORY_FETCH(registers[INSTRUCTION_POINTER], ireg[3]);
      SAFE_MEMORY_FETCH(registers[INSTRUCTION_POINTER], ireg[4]);
      payload = (signed short)(ireg[3] << 0 | ireg[4] << 8);
    }
    else if((ireg[2] & 0x0f) == 0x03){
      SAFE_MEMORY_FETCH(registers[INSTRUCTION_POINTER], ireg[3]);
      SAFE_MEMORY_FETCH(registers[INSTRUCTION_POINTER], ireg[4]);
      SAFE_MEMORY_FETCH(registers[INSTRUCTION_POINTER], ireg[5]);
      SAFE_MEMORY_FETCH(registers[INSTRUCTION_POINTER], ireg[6]);
      payload = (signed int)(
          ireg[3] << 0
        | ireg[4] << 8 
        | ireg[5] << 16 
        | ireg[6] << 24);
    }
    else if((ireg[2] & 0x0f) == 0x04){
      SAFE_MEMORY_FETCH(registers[INSTRUCTION_POINTER], ireg[3]);
      SAFE_MEMORY_FETCH(registers[INSTRUCTION_POINTER], ireg[4]);
      SAFE_MEMORY_FETCH(registers[INSTRUCTION_POINTER], ireg[5]);
      SAFE_MEMORY_FETCH(registers[INSTRUCTION_POINTER], ireg[6]);
      payload = registers[INSTRUCTION_POINTER] + (signed int)(
          ireg[3] << 0
        | ireg[4] << 8 
        | ireg[5] << 16 
        | ireg[6] << 24);
    }
  } break;
  }
}

static void AddressInstruction(void){
  switch(instr_types[instruction]){
  case ASM_NOOP: {

  } break;
  case ASM_ONEOP: {
    src =  registers[src_reg];
    dst = &registers[src_reg]; // there is no dst reg in machine code
  } break;
  
  case ASM_TWOOP: 
  case ASM_MOVEOP:
  case ASM_BRANCH: {
    // regdir
    if((addr_mode & 0xf0) == 0x00){
      src =  registers[src_reg] + payload;
      dst = &registers[dst_reg];
    }
    // regind
    else if((addr_mode & 0xf0) == 0x10){
      if(registers[src_reg] + payload >= MEMORY_SIZE){
        instruction = ASM_ERROR;
        GenerateInterrupt(INTR_BUS_FAULT);
      }
      else{
        src = *(unsigned int*)&memory[registers[src_reg] + payload];
        dst = &registers[dst_reg];
      }
    }
    // immed
    else if((addr_mode & 0xf0) == 0x20){
      src =  payload;
      dst = &registers[dst_reg];
    }
    // memdir
    else if((addr_mode & 0xf0) == 0x30){
      if(payload >= MEMORY_SIZE) {
        instruction = ASM_ERROR;
        GenerateInterrupt(INTR_BUS_FAULT);
      }
      else{
        src = *(unsigned int*)&memory[payload];
        dst = &registers[dst_reg];
      }
    }
    // regind store
    else if((addr_mode & 0xf0) == 0x40){
      if(registers[dst_reg] + payload >= MEMORY_SIZE){
        instruction = ASM_ERROR;
        GenerateInterrupt(INTR_BUS_FAULT);
      }
      else{
        src = registers[src_reg];
        dst = (unsigned int*)&memory[registers[dst_reg] + payload];
      }
    }
    // memdir store
    else if((addr_mode & 0xf0) == 0x50){
      if(payload >= MEMORY_SIZE) {
        instruction = ASM_ERROR;
        GenerateInterrupt(INTR_BUS_FAULT);
      }
      else{
        src = registers[src_reg];
        dst = (unsigned int*)&memory[payload];
      }
    }
  } break;
  }
}



static void PushByte(unsigned int src){
  registers[STACK_POINTER] -= 4;
  *(unsigned char*)&memory[registers[STACK_POINTER]] = src;
}

static void PopByte(unsigned int* dst){
  *dst = *(unsigned char*)&memory[registers[STACK_POINTER]];
  registers[STACK_POINTER] += 4;
}

static void PushWord(unsigned int src){
  registers[STACK_POINTER] -= 4;
  *(unsigned short*)&memory[registers[STACK_POINTER]] = src;
}

static void PopWord(unsigned int* dst){
  *dst = *(unsigned short*)&memory[registers[STACK_POINTER]];
  registers[STACK_POINTER] += 4;
}

static void PushLong(unsigned int src){
  registers[STACK_POINTER] -= 4;
  *(unsigned int*)&memory[registers[STACK_POINTER]] = src;
}

static void PopLong(unsigned int* dst){
  *dst = *(unsigned int*)&memory[registers[STACK_POINTER]];
  registers[STACK_POINTER] += 4;
}

static void Branch(unsigned int src){
  registers[INSTRUCTION_POINTER] = src;
}


static void ExecuteInstruction(void){
  switch(instruction){
  case ASM_ERROR: break;
  case ASM_HLT:   
    PSW_SET_HALT;
    break;
  case ASM_RET:
    PopLong(&registers[INSTRUCTION_POINTER]);
    break;
  case ASM_IRET:
    PopLong(&program_status_word);
    PopLong(&registers[INSTRUCTION_POINTER]);
    break;
  case ASM_PUSHF:
    PushLong(program_status_word);
    break;          
  case ASM_POPF:
    PopLong(&program_status_word);
    break;

  case ASM_SXBW:
    *dst = (short)(char)src; 
    break;
  case ASM_SXBL:
    *dst = (int)(char)src; 
    break;
  case ASM_SXWL:
    *dst = (int)(short)src; 
    break;
  case ASM_ZXBW:
    *dst = (unsigned short)(unsigned char)src; 
    break;
  case ASM_ZXBL:
    *dst = (unsigned int)(unsigned char)src; 
    break;
  case ASM_ZXWL:
    *dst = (unsigned int)(unsigned short)src; 
    break;

  case ASM_PUSHB:
    PushByte(src);
    break;
  case ASM_PUSHW:
    PushWord(src);
    break;
  case ASM_PUSHL:
    PushLong(src);
    break;
  case ASM_POPB:
    PopByte(dst);
    break;
  case ASM_POPW:
    PopWord(dst);
    break;
  case ASM_POPL:
    PopLong(dst);
    break;

  case ASM_NEG:
    *dst = -src;
    break;
  case ASM_INC:
    *dst = src + 1;
    break;
  case ASM_DEC:
    *dst = src - 1;
    break;
  case ASM_NOT:
    *dst = ~src;
    break;
  case ASM_TST:
    PSW_RESET_ZERO;
    PSW_RESET_SIGN;
    if(src == 0)         PSW_SET_ZERO;
    if(src & 0x80000000) PSW_SET_SIGN;
    break;
    
  case ASM_ADD:
    *dst += src;
    break;
  case ASM_SUB:
    *dst -= src;
    break;
  case ASM_MUL:
    *dst *= src;
    break;
  case ASM_IMUL:
    *(signed int*)dst *= (signed int)src;
    break;
  case ASM_DIV:
    if(src == 0){
      GenerateInterrupt(INTR_ARITHM_FAULT);
    }
    else{
      *dst /= src;
    }
    break;
  case ASM_IDIV:
    if(src == 0){
      GenerateInterrupt(INTR_ARITHM_FAULT);
    }
    else{
      *(signed int*)dst /= (signed int)src;
    }
    break;
  case ASM_MOD:
    if(src == 0){
      GenerateInterrupt(INTR_ARITHM_FAULT);
    }
    else{
      *dst %= src;
    }
  case ASM_IMOD:
    if(src == 0){
      GenerateInterrupt(INTR_ARITHM_FAULT);
    }
    else{
      *(signed int*)dst %= (signed int)src;
    }
    break;
  case ASM_CMP: {
    int temp = *dst - src;
    PSW_RESET_ZERO;
    PSW_RESET_SIGN;
    PSW_RESET_OVERF;
    PSW_RESET_CARRY;

    if(temp   == 0x00000000) PSW_SET_ZERO;
    if(temp    & 0x80000000) PSW_SET_SIGN;

    if( ~*dst  & 0x80000000
      &&  src  & 0x80000000
      && temp  & 0x80000000) PSW_SET_OVERF; // pos - neg = neg
    if(  *dst  & 0x80000000 
      && ~src  & 0x80000000
      && ~temp & 0x80000000) PSW_SET_OVERF; // neg - pos = pos

    if( ~*dst  & 0x80000000
      &&  src  & 0x80000000) PSW_SET_CARRY;
    if( ~*dst  & 0x80000000
      && ~src  & 0x80000000
      && temp  & 0x80000000) PSW_SET_CARRY; 
    if(  *dst  & 0x80000000
      &&  src  & 0x80000000
      && temp  & 0x80000000) PSW_SET_CARRY;    
  } break;

  case ASM_AND:
    *dst &= src;
    break;
  case ASM_OR:
    *dst |= src;
    break;
  case ASM_XOR:
    *dst ^= src;
    break;
  case ASM_ASL:
    *(signed int*)dst   <<= src;
    break;
  case ASM_ASR:
    *(signed int*)dst   >>= src;
    break;
  case ASM_LSL:
    *(unsigned int*)dst <<= src;
    break;
  case ASM_LSR:
    *(unsigned int*)dst >>= src;
    break;
  
  case ASM_MOVB:
    *(unsigned char*)dst  = src;
    break;
  case ASM_MOVW:
    *(unsigned short*)dst = src;
    break;
  case ASM_MOVL:
    *(unsigned int*)dst   = src;
    break;
  
  case ASM_JMP:
    registers[INSTRUCTION_POINTER] = src;
    break;
  case ASM_JEQ:
    if(PSW_GET_ZERO == 1) Branch(src);
    break;
  case ASM_JNQ:
    if(PSW_GET_ZERO == 0) Branch(src);
    break;
  case ASM_JGT:
    if(PSW_GET_SIGN == PSW_GET_OVERF && PSW_GET_ZERO == 0) Branch(src);
    break;
  case ASM_JLT:
    if(PSW_GET_SIGN != PSW_GET_OVERF) Branch(src);
    break;
  case ASM_JGE:
    if(PSW_GET_SIGN == PSW_GET_OVERF) Branch(src);
    break;
  case ASM_JLE:
    if(PSW_GET_SIGN != PSW_GET_OVERF || PSW_GET_ZERO == 1) Branch(src);
    break;
  case ASM_JA:
    if(PSW_GET_CARRY == 0 && PSW_GET_ZERO == 0) Branch(src);
    break;
  case ASM_JB:
    if(PSW_GET_CARRY == 1) Branch(src);
    break; 
  case ASM_JAE:
    if(PSW_GET_CARRY == 0) Branch(src);
    break;
  case ASM_JBE:
    if(PSW_GET_CARRY == 1 || PSW_GET_ZERO == 1) Branch(src);
    break;
  case ASM_CALL:
    PushLong(registers[INSTRUCTION_POINTER]);
    Branch(src);
    break;
  case ASM_INSTR_COUNT:
    break;
  }

  if(PSW_GET_HALT == 0 && registers[STACK_POINTER] < STACK_START) {
    GenerateInterrupt(INTR_STACK_OVERF);
  }
}

static void CheckInterrupts(void){
  static const char* interrupt_names[] = {
    [INTR_RESET]        = "RESET",
    [INTR_HARD_FAULT]   = "HARD FAULT",
    [INTR_BUS_FAULT]    = "BUS FAULT",
    [INTR_ARITHM_FAULT] = "ARITHMETIC FAULT",
    [INTR_STACK_OVERF]  = "STACK OVERFLOW",
    [INTR_STACK_OVERF + 1 ... INTR_COUNT - 1] = "CUSTOM INTERRUPT",
  };

  for(int i = 0; i < INTR_COUNT; i++){
    if(interrupt_lines & (1 << i)){
      // extern int debug_mode;

      // if(debug_mode == DEBUG_MODE_LONG || debug_mode == DEBUG_MODE_SHORT){
        printf("Interrupt generated: %s\n", interrupt_names[i]);
      // }

      PushLong(registers[INSTRUCTION_POINTER]);
      PushLong(program_status_word);
      registers[INSTRUCTION_POINTER] = *(unsigned int*)&memory[IVT_ADDRESS + (i << 2)];

      interrupt_lines &= ~(1 << i);
    }
  }
}

static void CheckTerminal(void){
  if(*(int*)&memory[TERM_OUT_CTRL] & 0x01){
    putchar(memory[TERM_OUT_VAL]);
    *(int*)&memory[TERM_OUT_CTRL] &= ~0x01;
  }
}

#define HEX_LINE_WIDTH 8

static void LoadProgram(void){
  int current_addr = 0;
  int current_byte = 0;
  while(fscanf(emuin, "%x : ", &current_addr) == 1){
    for(int i = 0; i < HEX_LINE_WIDTH; i++){
      fscanf(emuin, "%x", &current_byte);
      memory[current_addr + i] = current_byte;
    }
  }
}

static void InitializeProcessor(void){
  LoadProgram();

  program_status_word      = 0x00000000;
  for(int i = 0; i < MACHINE_REG_COUNT; i++){
    registers[i]           = 0x00000000;
  }
  registers[STACK_POINTER] = INITIAL_SP_VAL;
  registers[INSTRUCTION_POINTER] = *(unsigned int*)&memory[IVT_ADDRESS + (INTR_RESET << 2)];
}

static void PrintStackFrame(void);

static void InitializeMainStackFrame(void){
  if(exec_args == 0) return;

  Vector* exec_argv = VectorInit();
  for(int i = 0; i < VectorSize(exec_args); i++){
    VectorPush(exec_argv, 0);
  }

  for(int i = VectorSize(exec_args) - 1; i >= 0; i--){
    registers[STACK_POINTER] -= strlen((char*)VectorGet(exec_args, i)) + 1;
    VectorSet(exec_argv, i, (void*)(long)registers[STACK_POINTER]);
    strcpy((char*)&memory[registers[STACK_POINTER]], (char*)VectorGet(exec_args, i));
  }
  // odd  - padding to 8
  if(VectorSize(exec_args) % 2){
    registers[STACK_POINTER] = (registers[STACK_POINTER]) & ~0x07;
  }
  // even - padding to 4
  else{
    registers[STACK_POINTER] = ((registers[STACK_POINTER] - 4) & ~0x07) + 4;
  }

  // push mandatory null ptr (argv[argc])
  registers[STACK_POINTER] -= 4;
  *(unsigned*)&memory[registers[STACK_POINTER]] = 0;

  // push argv[0..argc-1]
  for(int i = VectorSize(exec_argv) - 1; i >= 0; i--){
    registers[STACK_POINTER] -= 4;
    *(unsigned*)&memory[registers[STACK_POINTER]] = (unsigned)(long)VectorGet(exec_argv, i);
  }

  // push argv
  registers[STACK_POINTER] -= 4;
  *(unsigned*)&memory[registers[STACK_POINTER]] = registers[STACK_POINTER] + 4;

  // push argc
  registers[STACK_POINTER] -= 4;
  *(unsigned*)&memory[registers[STACK_POINTER]] = (unsigned)VectorSize(exec_argv);

  PrintStackFrame();

  VectorDrop(exec_argv);
  VectorDrop(exec_args);
}

static void PrintInstr(void){
  AsmInstruction instr = ASM_ERROR;
  int addr_mode;
  int src_reg;
  int dst_reg;
  int payload;

  for(int i = 0; i < ASM_INSTR_COUNT; i++){
    if(instr_machine_code[i] == ireg[0]) {
      instr = i;
      break;
    }
  }

  printf("%s ", instr_names[instr]);

  switch(instr_types[instr]){
  case ASM_NOOP: {

  } break;
  case ASM_ONEOP: {
    src_reg = (ireg[1] >> 4) & 0x0f;
    dst_reg = (ireg[1] >> 0) & 0x0f;
    printf("%s", reg_names[src_reg]);
  } break;
  case ASM_TWOOP: 
  case ASM_MOVEOP:
  case ASM_BRANCH: {
    src_reg = (ireg[1] >> 4) & 0x0f;
    dst_reg = (ireg[1] >> 0) & 0x0f;

    addr_mode = ireg[2];

    if((ireg[2] & 0x0f) == 0x00){
      payload = 0;
    }
    else if((ireg[2] & 0x0f) == 0x01){
      payload = (signed char)ireg[3];
    }
    else if((ireg[2] & 0x0f) == 0x02){
      payload = (signed short)(ireg[3] << 0 | ireg[4] << 8);
    }
    else if((ireg[2] & 0x0f) == 0x03){
      payload = (signed int)(
          ireg[3] << 0
        | ireg[4] << 8 
        | ireg[5] << 16 
        | ireg[6] << 24);
    }
    else if((ireg[2] & 0x0f) == 0x04){
      payload = registers[INSTRUCTION_POINTER] + (signed int)(
          ireg[3] << 0
        | ireg[4] << 8 
        | ireg[5] << 16 
        | ireg[6] << 24);
    }

    if((addr_mode & 0xf0) == 0x00){
      if(instr_types[instr] != ASM_BRANCH){
        if(payload  > 0) printf("%s+%d, %s", reg_names[src_reg], +payload, reg_names[dst_reg]);
        if(payload == 0) printf("%s, %s",    reg_names[src_reg],           reg_names[dst_reg]);
        if(payload  < 0) printf("%s-%d, %s", reg_names[src_reg], -payload, reg_names[dst_reg]);
      }
      else {
        if(payload  > 0) printf("%s+%d", reg_names[src_reg], +payload);
        if(payload == 0) printf("%s",    reg_names[src_reg]          );
        if(payload  < 0) printf("%s-%d", reg_names[src_reg], -payload);
      }
    }
    // regind
    else if((addr_mode & 0xf0) == 0x10){
      if(instr_types[instr] != ASM_BRANCH){
        if(payload  > 0) printf("[%s+%d], %s", reg_names[src_reg], +payload, reg_names[dst_reg]);
        if(payload == 0) printf("[%s], %s",    reg_names[src_reg],           reg_names[dst_reg]);
        if(payload  < 0) printf("[%s-%d], %s", reg_names[src_reg], -payload, reg_names[dst_reg]);
      }
      else{
        if(payload  > 0) printf("[%s+%d]", reg_names[src_reg], +payload);
        if(payload == 0) printf("[%s]",    reg_names[src_reg]          );
        if(payload  < 0) printf("[%s-%d]", reg_names[src_reg], -payload);
      }
    }
    // immed
    else if((addr_mode & 0xf0) == 0x20){
      if(instr_types[instr] != ASM_BRANCH){
        printf("%d, %s", payload, reg_names[dst_reg]);
      }
      else{
        printf("%d", payload);
      }
    }
    // memdir
    else if((addr_mode & 0xf0) == 0x30){
      if(instr_types[instr] != ASM_BRANCH){
        printf("[%d], %s", payload, reg_names[dst_reg]);
      }
      else {
        printf("%d", payload);
      }
    }
    // regind store
    else if((addr_mode & 0xf0) == 0x40){
      if(payload  > 0) printf("%s, [%s+%d]", reg_names[src_reg], reg_names[dst_reg], +payload);
      if(payload == 0) printf("%s, [%s]",    reg_names[src_reg], reg_names[dst_reg]         );
      if(payload  < 0) printf("%s, [%s-%d]", reg_names[src_reg], reg_names[dst_reg], -payload);
    }
    // memdir store
    else if((addr_mode & 0xf0) == 0x50){
      printf("%s, [%d]", reg_names[src_reg], payload);
    }
  } break;
  }

  printf("\n");
}

static void PrintState(void){
  printf("--Instruction pointer : %08x\n", registers[INSTRUCTION_POINTER]);
  printf("--Stack pointer       : %08x\n", registers[STACK_POINTER]);
  printf("--Base pointer        : %08x\n", registers[BASE_POINTER]);
  printf("--Flags               : ");
  if(PSW_GET_ZERO)  printf("ZERO ");
  if(PSW_GET_SIGN)  printf("SIGN ");
  if(PSW_GET_OVERF) printf("OVERF ");
  if(PSW_GET_CARRY) printf("CARRY ");
  printf("\n");
  printf("--General purpose     : \n");
  printf("----");
  for(int i = 0; i < 4; i++){
    printf("%08x ", registers[i]);
  }
  printf("\n");
  printf("----");
  for(int i = 4; i < 8; i++){
    printf("%08x ", registers[i]);
  }
  printf("\n");
}

static void PrintStackFrame(void){
  int frame_start = registers[STACK_POINTER] / 4 * 4;
  int draw_box = 0;
  for(int i = INITIAL_SP_VAL - 4; i >= frame_start; i -= 4){
    if(draw_box == 0){
      printf("STACK FRAME: +3 +2 +1 +0 \n");
      draw_box = 1;
    }

    printf("+----------+-------------+\n");
    printf("| %08x | %02x %02x %02x %02x |", i, memory[i + 3], memory[i + 2], memory[i + 1], memory[i + 0]);
    if(registers[BASE_POINTER] != 0){
      if     (i == registers[BASE_POINTER]) printf(" <- BASE POINTER");
      else if(i > registers[BASE_POINTER]) printf(" +%d", +i - registers[BASE_POINTER]);
      else if(i < registers[BASE_POINTER]) printf(" -%d", -i + registers[BASE_POINTER]);
    }
    printf("\n");
  }
  if(draw_box) printf("+----------+-------------+\n");
}

static void Instruction(void){
  extern int debug_mode;

  instr_counter++;
  
  if(debug_mode == DEBUG_MODE_LONG || debug_mode == DEBUG_MODE_SHORT){
    printf("%08x : ", registers[INSTRUCTION_POINTER]);
  }

  FetchInstruction();

  if(debug_mode == DEBUG_MODE_LONG || debug_mode == DEBUG_MODE_SHORT) {
    PrintInstr();
  }
  
  AddressInstruction();
  ExecuteInstruction();

  if(debug_mode == DEBUG_MODE_LONG){
    PrintState();
    PrintStackFrame();
    printf("\n");
    fflush(stdout);

    if(instr_counter > 100) PSW_SET_HALT;
  }

  CheckInterrupts();
  CheckTerminal();
}

void EmulateProgram(void){
  InitializeProcessor();
  InitializeMainStackFrame();
  while(PSW_GET_HALT == 0){
    Instruction();
  }
  printf("Finished execution with exit code: %d\n", registers[0]);
  PrintState();
}