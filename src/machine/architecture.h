#ifndef _ARCHITECTURE_H_
#define _ARCHITECTURE_H_

typedef enum AsmInstruction{
  ASM_ERROR,

  ASM_HLT,
  ASM_RET,
  ASM_IRET,
  ASM_PUSHF,
  ASM_POPF,

  ASM_SXBW,
  ASM_SXBL,
  ASM_SXWL,
  ASM_ZXBW,
  ASM_ZXBL,
  ASM_ZXWL,

  ASM_PUSHB,
  ASM_PUSHW,
  ASM_PUSHL,
  ASM_POPB,
  ASM_POPW,
  ASM_POPL,

  ASM_NEG,
  ASM_INC,
  ASM_DEC,
  ASM_NOT,
  ASM_TST,
  
  ASM_ADD,
  ASM_SUB,
  ASM_MUL,
  ASM_IMUL,
  ASM_DIV,
  ASM_IDIV,
  ASM_MOD,
  ASM_IMOD,
  ASM_CMP,

  ASM_AND,
  ASM_OR,
  ASM_XOR,
  ASM_ASL,
  ASM_ASR,
  ASM_LSL,
  ASM_LSR,

  ASM_MOVB,
  ASM_MOVW,
  ASM_MOVL,

  ASM_JMP,
  ASM_JEQ,
  ASM_JNQ,
  ASM_JGT,
  ASM_JLT,
  ASM_JGE,
  ASM_JLE,
  ASM_JA,
  ASM_JB,
  ASM_JAE,
  ASM_JBE,
  ASM_CALL,

  ASM_INSTR_COUNT,
} AsmInstruction;

typedef enum AsmInstrType{
  ASM_NOOP,   // nullary
  ASM_ONEOP,  // unary  - reg only
  ASM_TWOOP,  // binary - operand and reg
  ASM_MOVEOP, // binary - operand and reg or reg and operand
  ASM_BRANCH, // branch - reg or simple operand
} AsmInstrType;

typedef enum AsmAddress{
  ADDR_REGDIR,
  ADDR_REGDIR_8BIT,
  ADDR_REGDIR_16BIT,
  ADDR_REGDIR_32BIT,
  ADDR_REGIND,
  ADDR_REGIND_8BIT,
  ADDR_REGIND_16BIT,
  ADDR_REGIND_32BIT,
  ADDR_IMMED,
  ADDR_IMMED_8BIT,
  ADDR_IMMED_16BIT,
  ADDR_IMMED_32BIT,
  ADDR_IMMED_PCREL,
  ADDR_MEMDIR,
  ADDR_MEMDIR_8BIT,
  ADDR_MEMDIR_16BIT,
  ADDR_MEMDIR_32BIT,
  ADDR_MEMDIR_PCREL,
  ADDR_STORE_REGIND,
  ADDR_STORE_REGIND_8BIT,
  ADDR_STORE_REGIND_16BIT,
  ADDR_STORE_REGIND_32BIT,
  ADDR_STORE_MEMDIR,
  ADDR_STORE_MEMDIR_8BIT,
  ADDR_STORE_MEMDIR_16BIT,
  ADDR_STORE_MEMDIR_32BIT,
  ADDR_STORE_MEMDIR_PCREL
} AsmAddress;

typedef enum AsmReg{
  REG_GP_COUNT = 8,
  REG_IP = REG_GP_COUNT,
  REG_SP,
  REG_BP,
} AsmReg;

extern const char*  instr_names[];
extern AsmInstrType instr_types[];
extern const char*  reg_names[];

extern const unsigned char instr_machine_code[];
extern const unsigned char addr_machine_code[];

typedef struct AsmOperand{
  AsmAddress address;
  AsmReg reg_ref;
  char* symbol;
  int offset;
} AsmOperand;

AsmOperand* AsmOperandCreateEmpty(void);
void AsmOperandDrop(AsmOperand*);
void AsmOperandDump(AsmOperand*);

int AddrIsRegdir(AsmAddress);
int AddrIsRegind(AsmAddress);
int AddrIsImmed (AsmAddress);
int AddrIsMemdir(AsmAddress);

AsmAddress AddrToStoreAddr(AsmAddress);
int AddrPayloadSize(AsmAddress);

#endif