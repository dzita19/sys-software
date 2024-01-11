#include "instruction.h"

#include <string.h>
#include "assembler.h"
#include "literals.h"
#include "logger_asm.h"

static AsmInstruction current_instruction = ASM_ERROR;

void SymbolToInstr(void){
  current_instruction = ASM_ERROR;

  char* instr_name = StackPop(&symbol_stack);
  for(int i = 0; i < ASM_INSTR_COUNT; i++){
    if(strcmp(instr_name, instr_names[i]) == 0) {
      current_instruction = (AsmInstruction)i;
      break;
    }
  }

  if(current_instruction == ASM_ERROR){
    ReportError("Unknown operation code (%s).", instr_name);
  }
  StringDrop(instr_name);
}



static void InstrError(int num_of_operands){
  for(int i = 0; i < num_of_operands; i++){
    AsmOperandDrop(StackPop(&operand_stack));
  }

  AsmInstr* error_instr = AsmInstrCreateEmpty();
  InsertInstrLine(error_instr);
}

void NullaryInstr(void){
  if(current_instruction == ASM_ERROR) {
    InstrError(0);
    return;
  }

  if(instr_types[current_instruction] != ASM_NOOP) {
    ReportError("Illegal instruction operands.");
    InstrError(0);
    return;
  }

  AsmInstr* instr = AsmInstrCreateEmpty();
  instr->instruction = current_instruction;
  instr->size = 1;

  InsertInstrLine(instr);
  OpenDefaultSection();
  SectionIncCounter(current_section, instr->size);
}

void UnaryRegInstr(void){
  if(current_instruction == ASM_ERROR) {
    InstrError(1);
    return;
  }

  if(instr_types[current_instruction] != ASM_ONEOP
      && instr_types[current_instruction] != ASM_BRANCH) {
    ReportError("Illegal instruction operands.");
    InstrError(1);
    return;
  }

  AsmInstr* instr = AsmInstrCreateEmpty();
  AsmOperand* operand = StackPop(&operand_stack);

  instr->instruction = current_instruction;
  LinkedListInsertFirstInfo(&instr->operands, operand);

  if(instr_types[current_instruction] == ASM_ONEOP){
    instr->size = 2;
  }
  else{
    instr->size = 3;
  }

  InsertInstrLine(instr);
  OpenDefaultSection();
  SectionIncCounter(current_section, instr->size);
}

void UnaryOpInstr(void){
  if(current_instruction == ASM_ERROR) {
    InstrError(1);
    return;
  }

  if(instr_types[current_instruction] != ASM_BRANCH){
    ReportError("Illegal instruction operands.");
    InstrError(1);
    return;
  }

  AsmInstr* instr = AsmInstrCreateEmpty();
  AsmOperand* operand = StackPop(&operand_stack);

  instr->instruction = current_instruction;
  LinkedListInsertFirstInfo(&instr->operands, operand);

  instr->size = 3 + AddrPayloadSize(operand->address);

  InsertInstrLine(instr);
  OpenDefaultSection();
  SectionIncCounter(current_section, instr->size);
}

void BinaryRegToRegInstr(void){
  if(current_instruction == ASM_ERROR) {
    InstrError(2);
    return;
  }

  if(instr_types[current_instruction] != ASM_TWOOP
      && instr_types[current_instruction] != ASM_MOVEOP){
    ReportError("Illegal instruction operands.");
    InstrError(2);
    return;
  }

  AsmInstr* instr = AsmInstrCreateEmpty();
  AsmOperand* dst = StackPop(&operand_stack);
  AsmOperand* src = StackPop(&operand_stack);

  instr->instruction = current_instruction;
  LinkedListInsertFirstInfo(&instr->operands, dst);
  LinkedListInsertFirstInfo(&instr->operands, src);
  instr->size = 3;

  InsertInstrLine(instr);
  OpenDefaultSection();
  SectionIncCounter(current_section, instr->size);
}

void BinaryOpToRegInstr(void){
  if(current_instruction == ASM_ERROR) {
    InstrError(2);
    return;
  }

  if(instr_types[current_instruction] != ASM_TWOOP
      && instr_types[current_instruction] != ASM_MOVEOP){
    ReportError("Illegal instruction operands.");
    InstrError(2);
    return;
  }
  
  AsmInstr* instr = AsmInstrCreateEmpty();
  AsmOperand* dst = StackPop(&operand_stack);
  AsmOperand* src = StackPop(&operand_stack);

  instr->instruction = current_instruction;
  LinkedListInsertFirstInfo(&instr->operands, dst);
  LinkedListInsertFirstInfo(&instr->operands, src);

  instr->size = 3 + AddrPayloadSize(src->address);

  InsertInstrLine(instr);
  OpenDefaultSection();
  SectionIncCounter(current_section, instr->size);
}

void BinaryRegToOpInstr(void){
  if(current_instruction == ASM_ERROR) {
    InstrError(2);
    return;
  }

  if(instr_types[current_instruction] != ASM_MOVEOP){
    ReportError("Illegal instruction operands.");
    InstrError(2);
    return;
  }

  AsmOperand* destination = StackPeek(&operand_stack);

  if(!AddrIsRegind(destination->address) && !AddrIsMemdir(destination->address)){
    ReportError("Illegal instruction operands");
    InstrError(2);
    return;
  }

  destination->address = AddrToStoreAddr(destination->address);

  AsmInstr* instr = AsmInstrCreateEmpty();
  AsmOperand* dst = StackPop(&operand_stack);
  AsmOperand* src = StackPop(&operand_stack);

  instr->instruction = current_instruction;
  LinkedListInsertFirstInfo(&instr->operands, dst);
  LinkedListInsertFirstInfo(&instr->operands, src);
  
  instr->size = 3 + AddrPayloadSize(dst->address);

  InsertInstrLine(instr);
  OpenDefaultSection();
  SectionIncCounter(current_section, instr->size);
}



void OperandRegdir(void){
  AsmOperand* operand = AsmOperandCreateEmpty();
  operand->address = ADDR_REGDIR;
  operand->reg_ref = (int)(long)StackPop(&reglit_stack);

  StackPush(&operand_stack, operand);
}

void OperandRegdirConstAdd(void){
  AsmOperand* operand = AsmOperandCreateEmpty();
  operand->reg_ref =  (int)(long)StackPop(&reglit_stack);
  operand->offset  = +(int)(long)StackPop(&numlit_stack);
  if(operand->offset == 0) 
    operand->address = ADDR_REGDIR;
  else if(operand->offset >= (char)0x80 && operand->offset <= (char)0x7f) 
    operand->address = ADDR_REGDIR_8BIT;
  else if(operand->offset >= (short)0x8000 && operand->offset <= (short)0x7fff) 
    operand->address = ADDR_REGDIR_16BIT;
  else
    operand->address = ADDR_REGDIR_32BIT;
  operand->address = ADDR_REGDIR;

  StackPush(&operand_stack, operand);
}

void OperandRegdirConstSub(void){
  AsmOperand* operand = AsmOperandCreateEmpty();
  operand->reg_ref =  (int)(long)StackPop(&reglit_stack);
  operand->offset  = -(int)(long)StackPop(&numlit_stack);
  if(operand->offset == 0) 
    operand->address = ADDR_REGDIR;
  else if(operand->offset >= (char)0x80 && operand->offset <= (char)0x7f) 
    operand->address = ADDR_REGDIR_8BIT;
  else if(operand->offset >= (short)0x8000 && operand->offset <= (short)0x7fff) 
    operand->address = ADDR_REGDIR_16BIT;
  else
    operand->address = ADDR_REGDIR_32BIT;

  StackPush(&operand_stack, operand);
}



void OperandImmedConstPlus(void){
  AsmOperand* operand = AsmOperandCreateEmpty();
  operand->offset  = +(int)(long)StackPop(&numlit_stack);
  if(operand->offset == 0) 
    operand->address = ADDR_IMMED;
  else if(operand->offset >= (char)0x80 && operand->offset <= (char)0x7f) 
    operand->address = ADDR_IMMED_8BIT;
  else if(operand->offset >= (short)0x8000 && operand->offset <= (short)0x7fff) 
    operand->address = ADDR_IMMED_16BIT;
  else
    operand->address = ADDR_IMMED_32BIT;

  StackPush(&operand_stack, operand);
}

void OperandImmedConstMinus(void){
  AsmOperand* operand = AsmOperandCreateEmpty();
  operand->address = ADDR_IMMED;
  operand->offset  = -(int)(long)StackPop(&numlit_stack);
  if(operand->offset == 0) 
    operand->address = ADDR_IMMED;
  else if(operand->offset >= (char)0x80 && operand->offset <= (char)0x7f) 
    operand->address = ADDR_IMMED_8BIT;
  else if(operand->offset >= (short)0x8000 && operand->offset <= (short)0x7fff) 
    operand->address = ADDR_IMMED_16BIT;
  else
    operand->address = ADDR_IMMED_32BIT;

  StackPush(&operand_stack, operand);
}

void OperandImmedSymbol(void){
  AsmOperand* operand = AsmOperandCreateEmpty();
  operand->address = ADDR_IMMED_PCREL;
  operand->symbol  = StackPop(&symbol_stack);

  StackPush(&operand_stack, operand);
}

void OperandImmedSymbolConstAdd(void){
  AsmOperand* operand = AsmOperandCreateEmpty();
  operand->address = ADDR_IMMED_PCREL;
  operand->symbol  = StackPop(&symbol_stack);
  operand->offset  = +(int)(long)StackPop(&numlit_stack);

  StackPush(&operand_stack, operand);
}

void OperandImmedSymbolConstSub(void){
  AsmOperand* operand = AsmOperandCreateEmpty();
  operand->address = ADDR_IMMED_PCREL;
  operand->symbol  = StackPop(&symbol_stack);
  operand->offset  = -(int)(long)StackPop(&numlit_stack);

  StackPush(&operand_stack, operand);
}



void OperandRegind(void){
  AsmOperand* operand = AsmOperandCreateEmpty();
  operand->address = ADDR_REGIND;
  operand->reg_ref = (int)(long)StackPop(&reglit_stack);

  StackPush(&operand_stack, operand);
}

void OperandRegindConstAdd(void){
  AsmOperand* operand = AsmOperandCreateEmpty();
  operand->reg_ref =  (int)(long)StackPop(&reglit_stack);
  operand->offset  = +(int)(long)StackPop(&numlit_stack);
  if(operand->offset == 0) 
    operand->address = ADDR_REGIND;
  else if(operand->offset >= (char)0x80 && operand->offset <= (char)0x7f) 
    operand->address = ADDR_REGIND_8BIT;
  else if(operand->offset >= (short)0x8000 && operand->offset <= (short)0x7fff) 
    operand->address = ADDR_REGIND_16BIT;
  else
    operand->address = ADDR_REGIND_32BIT;

  StackPush(&operand_stack, operand);
}

void OperandRegindConstSub(void){
  AsmOperand* operand = AsmOperandCreateEmpty();
  operand->reg_ref =  (int)(long)StackPop(&reglit_stack);
  operand->offset  = -(int)(long)StackPop(&numlit_stack);
  if(operand->offset == 0) 
    operand->address = ADDR_REGIND;
  else if(operand->offset >= (char)0x80 && operand->offset <= (char)0x7f) 
    operand->address = ADDR_REGIND_8BIT;
  else if(operand->offset >= (short)0x8000 && operand->offset <= (short)0x7fff) 
    operand->address = ADDR_REGIND_16BIT;
  else
    operand->address = ADDR_REGIND_32BIT;

  StackPush(&operand_stack, operand);
}



void OperandMemdirConstPlus(void){
  AsmOperand* operand = AsmOperandCreateEmpty();
  operand->offset  = +(int)(long)StackPop(&numlit_stack);
  if(operand->offset == 0) 
    operand->address = ADDR_MEMDIR;
  else if(operand->offset >= (char)0x80 && operand->offset <= (char)0x7f) 
    operand->address = ADDR_MEMDIR_8BIT;
  else if(operand->offset >= (short)0x8000 && operand->offset <= (short)0x7fff) 
    operand->address = ADDR_MEMDIR_16BIT;
  else
    operand->address = ADDR_MEMDIR_32BIT;

  StackPush(&operand_stack, operand);
}

void OperandMemdirConstMinus(void){
  AsmOperand* operand = AsmOperandCreateEmpty();
  operand->offset  = -(int)(long)StackPop(&numlit_stack);
  if(operand->offset == 0) 
    operand->address = ADDR_MEMDIR;
  else if(operand->offset >= (char)0x80 && operand->offset <= (char)0x7f) 
    operand->address = ADDR_MEMDIR_8BIT;
  else if(operand->offset >= (short)0x8000 && operand->offset <= (short)0x7fff) 
    operand->address = ADDR_MEMDIR_16BIT;
  else
    operand->address = ADDR_MEMDIR_32BIT;

  StackPush(&operand_stack, operand);
}

void OperandMemdirSymbol(void){
  AsmOperand* operand = AsmOperandCreateEmpty();
  operand->address = ADDR_MEMDIR_PCREL;
  operand->symbol  = StackPop(&symbol_stack);

  StackPush(&operand_stack, operand);
}

void OperandMemdirSymbolConstAdd(void){
  AsmOperand* operand = AsmOperandCreateEmpty();
  operand->address = ADDR_MEMDIR_PCREL;
  operand->symbol  = StackPop(&symbol_stack);
  operand->offset  = +(int)(long)StackPop(&numlit_stack);

  StackPush(&operand_stack, operand);
}

void OperandMemdirSymbolConstSub(void){
  AsmOperand* operand = AsmOperandCreateEmpty();
  operand->address = ADDR_MEMDIR_PCREL;
  operand->symbol  = StackPop(&symbol_stack);
  operand->offset  = -(int)(long)StackPop(&numlit_stack);

  StackPush(&operand_stack, operand);
}