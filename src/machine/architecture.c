#include "architecture.h"

#include <stdio.h>
#include <stdlib.h>
#include "util/memory_safety.h"

const char*  instr_names[] = {
  [ASM_ERROR] = ".error",
  [ASM_HLT]   = "hlt",
  [ASM_RET]   = "ret",
  [ASM_IRET]  = "iret",
  [ASM_PUSHF] = "pushf",
  [ASM_POPF]  = "popf",

  [ASM_SXBW]  = "sxbw",
  [ASM_SXBL]  = "sxbl",
  [ASM_SXWL]  = "sxwl",
  [ASM_ZXBW]  = "zxbw",
  [ASM_ZXBL]  = "zxbl",
  [ASM_ZXWL]  = "zxwl",

  [ASM_PUSHB] = "pushb",
  [ASM_PUSHW] = "pushw",
  [ASM_PUSHL] = "pushl",
  [ASM_POPB]  = "popb",
  [ASM_POPW]  = "popw",
  [ASM_POPL]  = "popl",

  [ASM_NEG]   = "neg",
  [ASM_INC]   = "inc",
  [ASM_DEC]   = "dec",
  [ASM_NOT]   = "not",
  [ASM_TST]   = "tst",
  
  [ASM_ADD]   = "add",
  [ASM_SUB]   = "sub",
  [ASM_MUL]   = "mul",
  [ASM_IMUL]  = "imul",
  [ASM_DIV]   = "div",
  [ASM_IDIV]  = "idiv",
  [ASM_MOD]   = "mod",
  [ASM_IMOD]  = "imod",
  [ASM_CMP]   = "cmp",
  [ASM_AND]   = "and",
  [ASM_OR]    = "or",
  [ASM_XOR]   = "xor",
  [ASM_ASL]   = "asl",
  [ASM_ASR]   = "asr",
  [ASM_LSL]   = "lsl",
  [ASM_LSR]   = "lsr",

  [ASM_MOVB]  = "movb",
  [ASM_MOVW]  = "movw",
  [ASM_MOVL]  = "movl",

  [ASM_JMP]   = "jmp",
  [ASM_JEQ]   = "jeq",
  [ASM_JNQ]   = "jnq",
  [ASM_JGT]   = "jgt",
  [ASM_JLT]   = "jlt",
  [ASM_JGE]   = "jge",
  [ASM_JLE]   = "jle",
  [ASM_JA]    = "ja",
  [ASM_JB]    = "jb",
  [ASM_JAE]   = "jae",
  [ASM_JBE]   = "jbe",
  [ASM_CALL]  = "call",
};

AsmInstrType instr_types[] = {
  [ASM_ERROR]              = -1,
  [ASM_HLT  ... ASM_POPF]  = ASM_NOOP,
  [ASM_SXBW ... ASM_TST]   = ASM_ONEOP,
  [ASM_ADD  ... ASM_LSR]   = ASM_TWOOP,
  [ASM_MOVB ... ASM_MOVL]  = ASM_MOVEOP,
  [ASM_JMP  ... ASM_CALL]  = ASM_BRANCH,
};

const char*  reg_names[] = {
  [0]       = "%reg0",
  [1]       = "%reg1",
  [2]       = "%reg2",
  [3]       = "%reg3",
  [4]       = "%reg4",
  [5]       = "%reg5",
  [6]       = "%reg6",
  [7]       = "%reg7",
  [REG_IP]  = "%rip",
  [REG_BP]  = "%rbp",
  [REG_SP]  = "%rsp",
};

const unsigned char instr_machine_code[] = {
  [ASM_ERROR] = 0xff,
  [ASM_HLT]   = 0x00,
  [ASM_RET]   = 0x01,
  [ASM_IRET]  = 0x02,
  [ASM_PUSHF] = 0x03,
  [ASM_POPF]  = 0x04,

  [ASM_SXBW]  = 0x10,
  [ASM_SXBL]  = 0x11,
  [ASM_SXWL]  = 0x12,
  [ASM_ZXBW]  = 0x13,
  [ASM_ZXBL]  = 0x14,
  [ASM_ZXWL]  = 0x15,

  [ASM_PUSHB] = 0x20,
  [ASM_PUSHW] = 0x21,
  [ASM_PUSHL] = 0x22,
  [ASM_POPB]  = 0x23,
  [ASM_POPW]  = 0x24,
  [ASM_POPL]  = 0x25,

  [ASM_NEG]   = 0x30,
  [ASM_INC]   = 0x31,
  [ASM_DEC]   = 0x32,
  [ASM_NOT]   = 0x33,
  [ASM_TST]   = 0x34,
  
  [ASM_ADD]   = 0x40,
  [ASM_SUB]   = 0x41,
  [ASM_MUL]   = 0x42,
  [ASM_IMUL]  = 0x43,
  [ASM_DIV]   = 0x44,
  [ASM_IDIV]  = 0x45,
  [ASM_MOD]   = 0x46,
  [ASM_IMOD]  = 0x47,
  [ASM_CMP]   = 0x48,

  [ASM_AND]   = 0x50,
  [ASM_OR]    = 0x51,
  [ASM_XOR]   = 0x52,
  [ASM_ASL]   = 0x53,
  [ASM_ASR]   = 0x54,
  [ASM_LSL]   = 0x55,
  [ASM_LSR]   = 0x56,

  [ASM_MOVB]  = 0x60,
  [ASM_MOVW]  = 0x61,
  [ASM_MOVL]  = 0x62,

  [ASM_JMP]   = 0x70,
  [ASM_JEQ]   = 0x71,
  [ASM_JNQ]   = 0x72,
  [ASM_JGT]   = 0x73,
  [ASM_JLT]   = 0x74,
  [ASM_JGE]   = 0x75,
  [ASM_JLE]   = 0x76,
  [ASM_JA]    = 0x77,
  [ASM_JB]    = 0x78,
  [ASM_JAE]   = 0x79,
  [ASM_JBE]   = 0x7A,
  [ASM_CALL]  = 0x7B,
};

const unsigned char addr_machine_code[] = {
  [ADDR_REGDIR]             = 0x00,
  [ADDR_REGDIR_8BIT]        = 0x01,
  [ADDR_REGDIR_16BIT]       = 0x02,
  [ADDR_REGDIR_32BIT]       = 0x03,
  [ADDR_REGIND]             = 0x10,
  [ADDR_REGIND_8BIT]        = 0x11,
  [ADDR_REGIND_16BIT]       = 0x12,
  [ADDR_REGIND_32BIT]       = 0x13,
  [ADDR_IMMED]              = 0x20,
  [ADDR_IMMED_8BIT]         = 0x21,
  [ADDR_IMMED_16BIT]        = 0x22,
  [ADDR_IMMED_32BIT]        = 0x23,
  [ADDR_IMMED_PCREL]        = 0x24,
  [ADDR_MEMDIR]             = 0x30,
  [ADDR_MEMDIR_8BIT]        = 0x31,
  [ADDR_MEMDIR_16BIT]       = 0x32,
  [ADDR_MEMDIR_32BIT]       = 0x33,
  [ADDR_MEMDIR_PCREL]       = 0x34,
  [ADDR_STORE_REGIND]       = 0x40,
  [ADDR_STORE_REGIND_8BIT]  = 0x41,
  [ADDR_STORE_REGIND_16BIT] = 0x42,
  [ADDR_STORE_REGIND_32BIT] = 0x43,
  [ADDR_STORE_MEMDIR]       = 0x50,
  [ADDR_STORE_MEMDIR_8BIT]  = 0x51,
  [ADDR_STORE_MEMDIR_16BIT] = 0x52,
  [ADDR_STORE_MEMDIR_32BIT] = 0x53,
  [ADDR_STORE_MEMDIR_PCREL] = 0x54,
};


AsmOperand* AsmOperandCreateEmpty(void){
  AsmOperand* operand = malloc(sizeof(AsmOperand));
  operand->address    = 0;
  operand->reg_ref    = 0;
  operand->symbol     = NULL;
  operand->offset     = 0;

  asm_operand_alloc++;

  return operand;
}

void AsmOperandDrop(AsmOperand* operand){
  if(operand->symbol != NULL) StringDrop(operand->symbol);
  free(operand);

  asm_operand_free++;
}

void AsmOperandDump(AsmOperand* operand){

  switch(operand->address){
  case ADDR_REGDIR:
  case ADDR_REGDIR_8BIT:
  case ADDR_REGDIR_16BIT:
  case ADDR_REGDIR_32BIT:
    printf("%s", reg_names[operand->reg_ref]);
    if(operand->offset > 0) printf("+%d", +operand->offset);
    if(operand->offset < 0) printf("-%d", -operand->offset);
    break;
  case ADDR_REGIND:
  case ADDR_REGIND_8BIT:
  case ADDR_REGIND_16BIT:
  case ADDR_REGIND_32BIT:
  case ADDR_STORE_REGIND:
  case ADDR_STORE_REGIND_8BIT:
  case ADDR_STORE_REGIND_16BIT:
  case ADDR_STORE_REGIND_32BIT:
    printf("(");
    printf("%s", reg_names[operand->reg_ref]);
    if(operand->offset > 0) printf("+%d", +operand->offset);
    if(operand->offset < 0) printf("-%d", -operand->offset);
    printf(")");
    break;
  case ADDR_IMMED:
  case ADDR_IMMED_8BIT:
  case ADDR_IMMED_16BIT:
  case ADDR_IMMED_32BIT:
  case ADDR_IMMED_PCREL:
    printf("$");
    if(operand->symbol != NULL) printf("%s", operand->symbol);
    if(operand->offset >= 0) printf("+%d", +operand->offset);
    if(operand->offset <  0) printf("-%d", -operand->offset);
    break;
  case ADDR_MEMDIR:
  case ADDR_MEMDIR_8BIT:
  case ADDR_MEMDIR_16BIT:
  case ADDR_MEMDIR_32BIT:
  case ADDR_MEMDIR_PCREL:
  case ADDR_STORE_MEMDIR:
  case ADDR_STORE_MEMDIR_8BIT:
  case ADDR_STORE_MEMDIR_16BIT:
  case ADDR_STORE_MEMDIR_32BIT:
  case ADDR_STORE_MEMDIR_PCREL:
    if(operand->symbol != NULL) printf("%s", operand->symbol);
    if(operand->offset >= 0) printf("+%d", +operand->offset);
    if(operand->offset <  0) printf("-%d", -operand->offset);
    break;
  }
  
}

int AddrIsRegdir(AsmAddress addr){
  return addr >= ADDR_REGDIR && addr <= ADDR_REGDIR_32BIT;
}

int AddrIsRegind(AsmAddress addr){
  return addr >= ADDR_REGIND && addr <= ADDR_REGIND_32BIT;
}

int AddrIsImmed (AsmAddress addr){
  return addr >= ADDR_IMMED && addr <= ADDR_IMMED_PCREL;
}

int AddrIsMemdir(AsmAddress addr){
  return addr >= ADDR_MEMDIR && addr <= ADDR_MEMDIR_PCREL;
}

AsmAddress AddrToStoreAddr(AsmAddress addr){
  if(AddrIsRegind(addr)) return addr + ADDR_STORE_REGIND - ADDR_REGIND;
  if(AddrIsMemdir(addr)) return addr + ADDR_STORE_MEMDIR - ADDR_MEMDIR;
  return 0;
}

int AddrPayloadSize(AsmAddress addr){
  switch(addr){
  case ADDR_REGDIR:
  case ADDR_REGIND:
  case ADDR_IMMED:
  case ADDR_MEMDIR:
  case ADDR_STORE_REGIND:
  case ADDR_STORE_MEMDIR:
    return 0;

  case ADDR_REGDIR_8BIT:
  case ADDR_REGIND_8BIT:
  case ADDR_IMMED_8BIT:
  case ADDR_MEMDIR_8BIT:
  case ADDR_STORE_REGIND_8BIT:
  case ADDR_STORE_MEMDIR_8BIT:
    return 1;

  case ADDR_REGDIR_16BIT:
  case ADDR_REGIND_16BIT:
  case ADDR_IMMED_16BIT:
  case ADDR_MEMDIR_16BIT:
  case ADDR_STORE_REGIND_16BIT:
  case ADDR_STORE_MEMDIR_16BIT:
    return 2;

  case ADDR_REGDIR_32BIT:
  case ADDR_REGIND_32BIT:
  case ADDR_IMMED_32BIT:
  case ADDR_MEMDIR_32BIT:
  case ADDR_STORE_REGIND_32BIT:
  case ADDR_STORE_MEMDIR_32BIT:
    return 4;

  case ADDR_IMMED_PCREL:
  case ADDR_MEMDIR_PCREL:
  case ADDR_STORE_MEMDIR_PCREL:
    return 4;
    
  default:
    return 0;
  }
}