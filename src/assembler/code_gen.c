#include "code_gen.h"

#include <string.h>
#include "logger_asm.h"

#define PCREL_ADDEND -4

static void GenerateInstructionOperands(AsmInstruction instruction, AsmOperand* src, AsmOperand* dst){
  if(dst != NULL) SectionPutCodeByte(current_section, src->reg_ref << 4 | dst->reg_ref);
  else SectionPutCodeByte(current_section, src->reg_ref << 4);

  if(dst == NULL || dst->address < ADDR_STORE_REGIND){
    if(instr_types[instruction] != ASM_ONEOP) {
      SectionPutCodeByte(current_section, addr_machine_code[src->address]);
    }

    switch(src->address){
    case ADDR_REGDIR:
    case ADDR_REGIND:
    case ADDR_IMMED: 
    case ADDR_MEMDIR:
      break;
      
    case ADDR_REGDIR_8BIT:
    case ADDR_REGIND_8BIT:
    case ADDR_IMMED_8BIT: 
    case ADDR_MEMDIR_8BIT: 
      SectionPutCodeByte(current_section, src->offset);
      break;
      
    case ADDR_REGDIR_16BIT:
    case ADDR_REGIND_16BIT:
    case ADDR_IMMED_16BIT: 
    case ADDR_MEMDIR_16BIT: 
      SectionPutCodeWord(current_section, src->offset);
      break;
      
    case ADDR_REGDIR_32BIT:
    case ADDR_REGIND_32BIT:
    case ADDR_IMMED_32BIT: 
    case ADDR_MEMDIR_32BIT:   
      SectionPutCodeLong(current_section, src->offset);
      break;
    
    case ADDR_IMMED_PCREL:
    case ADDR_MEMDIR_PCREL: {
      SymtabEntry* symbol = FindEntry(src->symbol);
      if(symbol == NULL){
        ReportError("Referenced symbol %s is not found.", src->symbol);
        SectionPutCodeLong(current_section, 0x00);
        break;
      }

      if(symbol->section == SECTION_ABS){
        SectionPutCodeLong(current_section, symbol->value + src->offset);
      }
      else if(symbol->section == current_section->index){
        int current_location_counter = ByteVectorSize(current_section->content);
        SectionPutCodeLong(current_section, symbol->value + src->offset - current_location_counter + PCREL_ADDEND);
      }
      else{
        int current_location_counter = ByteVectorSize(current_section->content);

        RelatabEntry* entry = RelatabEntryCreateEmpty();
        entry->offset = current_location_counter;
        entry->type   = RELA_CODE_32;
        entry->symbol = symbol->bind == SYM_BIND_GLOBAL ? symbol->index : symbol->section;
        entry->addend = (symbol->bind == SYM_BIND_GLOBAL ? src->offset : symbol->value + src->offset) + PCREL_ADDEND;
        RelatabInsertEntry(current_relatab, entry);

        SectionPutCodeLong(current_section, entry->addend);
      }
    } break;

    default: break;
    }
  }
  else{
    if(instr_types[instruction] != ASM_ONEOP) {
      SectionPutCodeByte(current_section, addr_machine_code[dst->address]);
    }

    switch(dst->address){
    case ADDR_STORE_REGIND:
    case ADDR_STORE_MEMDIR:      
      break;
        
    case ADDR_STORE_REGIND_8BIT:
    case ADDR_STORE_MEMDIR_8BIT:
      SectionPutCodeByte(current_section, dst->offset);
      break;
        
    case ADDR_STORE_REGIND_16BIT:
    case ADDR_STORE_MEMDIR_16BIT:
      SectionPutCodeWord(current_section, dst->offset);
      break;
        
    case ADDR_STORE_REGIND_32BIT:
    case ADDR_STORE_MEMDIR_32BIT:
      SectionPutCodeLong(current_section, dst->offset);
      break;
        
    case ADDR_STORE_MEMDIR_PCREL: {
      SymtabEntry* symbol = FindEntry(dst->symbol);
      if(symbol == NULL){
        ReportError("Referenced symbol %s is not found.", dst->symbol);
        SectionPutCodeLong(current_section, 0x00);
        break;
      }

      if(symbol->section == SECTION_ABS){
        SectionPutCodeLong(current_section, symbol->value + dst->offset);
      }
      else if(symbol->section == current_section->index){
        int current_location_counter = ByteVectorSize(current_section->content);
        SectionPutCodeLong(current_section, symbol->value + dst->offset - current_location_counter + PCREL_ADDEND);
      }
      else{
        int current_location_counter = ByteVectorSize(current_section->content);

        RelatabEntry* entry = RelatabEntryCreateEmpty();
        entry->offset = current_location_counter;
        entry->type   = RELA_CODE_32;
        entry->symbol = symbol->bind == SYM_BIND_GLOBAL ? symbol->index : symbol->section;
        entry->addend = (symbol->bind == SYM_BIND_GLOBAL ? dst->offset : symbol->value + dst->offset) + PCREL_ADDEND;
        RelatabInsertEntry(current_relatab, entry);

        SectionPutCodeLong(current_section, entry->addend);
      }
    } break;

    default: break;
    }
  }
}

static void GenerateInstruction(AsmInstr* instr){
  SectionPutCodeByte(current_section, instr_machine_code[instr->instruction]);

  switch(instr_types[instr->instruction]){
  case ASM_NOOP: break;
  case ASM_ONEOP:{
    AsmOperand* operand = instr->operands.first->info;
    GenerateInstructionOperands(instr->instruction, operand, 0);
  } break;
  case ASM_TWOOP:{
    AsmOperand* src = instr->operands.first->info; // regdir or operand 
    AsmOperand* dst = instr->operands.last->info;  // regdir
    GenerateInstructionOperands(instr->instruction, src, dst);
  } break;
  case ASM_MOVEOP:{
    AsmOperand* src = instr->operands.first->info; // regdir or operand 
    AsmOperand* dst = instr->operands.last->info;  // regdir or operand
    GenerateInstructionOperands(instr->instruction, src, dst);
  } break;
  case ASM_BRANCH:{
    AsmOperand* operand = instr->operands.first->info; // regdir or operand
    GenerateInstructionOperands(instr->instruction, operand, 0);
  } break;
  }
}

static void GenerateDirective(AsmDir* dir){
  switch(dir->directive){
  case DIR_SECTION:{
    AsmExpr* arg = dir->arguments.first->info;
    char* section_name = VectorGet(arg->plus_symbols, 0);

    for(int i = 0; i < VectorSize(sections); i++){
      Section* section = VectorGet(sections, i);
      SymtabEntry* section_symbol = SymtabGetEntry(symtab, section->index);
      if(strcmp(section_name, section_symbol->name) == 0){
        current_section = VectorGet(sections, i);
        current_relatab = VectorGet(relatabs, i);
        break;
      }
    }

  } break;

  case DIR_BYTE:
  case DIR_WORD:
  case DIR_LONG:{
    for(Node* node = dir->arguments.first; node != NULL; node = node->next){
      AsmExpr* resolved = ResolveExpression(node->info);

      if(resolved != NULL) {

        // add symbol to abs section
        if(VectorSize(resolved->plus_symbols) == 0 && VectorSize(resolved->minus_symbols) == 0){
          if(dir->directive == DIR_BYTE) SectionPutDataByte(current_section, resolved->offset);
          if(dir->directive == DIR_WORD) SectionPutDataWord(current_section, resolved->offset);
          if(dir->directive == DIR_LONG) SectionPutDataLong(current_section, resolved->offset);
        }
        else if(VectorSize(resolved->plus_symbols) == 1 && VectorSize(resolved->minus_symbols) == 0){
          SymtabEntry* section_symbol = SymtabFindEntry(symtab, (char*)VectorGet(resolved->plus_symbols, 0));
          
          RelatabEntry* relatab_entry = RelatabEntryCreateEmpty();
          relatab_entry->offset = ByteVectorSize(current_section->content);
          relatab_entry->symbol = section_symbol->index;
          relatab_entry->addend = resolved->offset;
          if(dir->directive == DIR_BYTE) relatab_entry->type = RELA_DATA_8;
          if(dir->directive == DIR_WORD) relatab_entry->type = RELA_DATA_16;
          if(dir->directive == DIR_LONG) relatab_entry->type = RELA_DATA_32;
          RelatabInsertEntry(current_relatab, relatab_entry);

          if(dir->directive == DIR_BYTE) SectionPutDataByte(current_section, resolved->offset);
          if(dir->directive == DIR_WORD) SectionPutDataWord(current_section, resolved->offset);
          if(dir->directive == DIR_LONG) SectionPutDataLong(current_section, resolved->offset);
        }
        else {
          ReportError("Data generation expression depends on multiple sections.");
          if(dir->directive == DIR_BYTE) SectionPutDataByte(current_section, 0x00);
          if(dir->directive == DIR_WORD) SectionPutDataWord(current_section, 0x00);
          if(dir->directive == DIR_LONG) SectionPutDataLong(current_section, 0x00);
        }

        AsmExprDrop(resolved);
      }
      else{
        if(dir->directive == DIR_BYTE) SectionPutDataByte(current_section, 0x00);
        if(dir->directive == DIR_WORD) SectionPutDataWord(current_section, 0x00);
        if(dir->directive == DIR_LONG) SectionPutDataLong(current_section, 0x00);
      }
    }
  } break;

  case DIR_SKIP:
  case DIR_ZERO:{
    AsmExpr* expr = dir->arguments.first->info;
    for(int i = 0; i < expr->offset; i++){
      SectionPutDataByte(current_section, 0x00);
    }
  } break;

  case DIR_ASCIZ:{
    for(int i = 0; i < *(int*)&dir->strlit[0]; i++){
      SectionPutDataByte(current_section, dir->strlit[i + 4]);
    }
  } break;

  default: break;
  }
}

static void GenerateLine(AsmLine* line){
  line->kind == LINE_DIR 
    ? GenerateDirective(line->content) 
    : GenerateInstruction(line->content);
}

static void OutputToFile(void){
  // print symtab
  fprintf(asout, "%d\n", VectorSize(symtab));
  for(int i = 0; i < VectorSize(symtab); i++){
    SymtabEntry* entry = SymtabGetEntry(symtab, i);
    fprintf(asout, "%d %d %d %d %d %d %s\n",
      entry->value,
      entry->size,
      entry->align,
      entry->type,
      entry->bind,
      entry->section,
      entry->name
    );
  }
  fprintf(asout, "\n");

  // print number of sections and relatab
  fprintf(asout, "%d\n\n", VectorSize(sections));
  
  // print sections
  for(int i = 0; i < VectorSize(sections); i++){
    Section* section = VectorGet(sections, i);
    fprintf(asout, "%d %d", section->index, ByteVectorSize(section->content));
    for(int j = 0; j < ByteVectorSize(section->content); j++){
      if(j % 8 == 0) fprintf(asout, "\n");
      fprintf(asout, "%02x ", ByteVectorGet(section->content, j));
    }
    fprintf(asout, "\n");
  }
  fprintf(asout, "\n");

  for(int i = 0; i < VectorSize(relatabs); i++){
    Relatab* relatab = VectorGet(relatabs, i);
    fprintf(asout, "%d\n", VectorSize(relatab));
    for(int j = 0; j < VectorSize(relatab); j++){
      RelatabEntry* entry = VectorGet(relatab, j);
      fprintf(asout, "%d %d %d %d\n",
        entry->offset,
        entry->type,
        entry->symbol,
        entry->addend
      );
    }
  }
}

void GenerateMachineCode(void){
  for(int i = 0; i < VectorSize(sections); i++){
    Section* section = VectorGet(sections, i);
    SymtabEntry* section_entry = SymtabGetEntry(symtab, section->index);
    section_entry->size = section->counter;

    ByteVectorReserve(section->content, section->counter);
    VectorPush(relatabs, RelatabCreateEmpty());
  }

  for(int i = 0; i < VectorSize(program); i++){
    GenerateLine(VectorGet(program, i));
  }

  OutputToFile();
}