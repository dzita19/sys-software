#include "directive.h"

#include "assembler.h"
#include "literals.h"
#include "logger_asm.h"
#include <string.h>

const char* dir_names[] = {
  [DIR_ERROR]   = ".error",
  [DIR_END]     = ".end",
  [DIR_SECTION] = ".section",
  [DIR_GLOBL]   = ".globl",
  [DIR_EXTRN]   = ".extrn",
  [DIR_DEF]     = ".def",
  [DIR_BYTE]    = ".byte",
  [DIR_WORD]    = ".word",
  [DIR_LONG]    = ".long",
  [DIR_ZERO]    = ".zero",
  [DIR_SKIP]    = ".skip",
  [DIR_ASCIZ]   = ".asciz",
  [DIR_NOTYP]   = ".notyp",
  [DIR_FUNCT]   = ".funct",
  [DIR_OBJCT]   = ".objct",
};



extern void SymbolText(void){
  StackPush(&symbol_stack, StringDuplicate(".text"));
}

extern void SymbolData(void){
  StackPush(&symbol_stack, StringDuplicate(".data"));
}

extern void SymbolRodata(void){
  StackPush(&symbol_stack, StringDuplicate(".rodata"));
}



void End(void){
  AsmDir* dir = AsmDirCreateEmpty();
  dir->directive = DIR_END;
  InsertDirLine(dir);
}

void SectionSymbol(void){
  char* symbol_name = StackPop(&symbol_stack);

  if(SymbolIsLocal(symbol_name)){
    ReportError("Symbol with local name (%s) declared section.", symbol_name);
    StringDrop(symbol_name);
    return;
  }
  AsmDir*  dir  = AsmDirCreateEmpty();
  AsmExpr* expr = SymbolExpression(symbol_name);
  LinkedListInsertFirstInfo(&dir->arguments, expr);

  dir->directive = DIR_SECTION;

  InsertDirLine(dir);

  // find if this section is already open
  Section* found = FindSection(symbol_name);

  if(found) {
    current_section = found;
  }
  else{
    SymtabEntry* symtab_entry = SymtabEntryCreateEmpty();
    symtab_entry->value = 0;
    symtab_entry->size  = 0;
    symtab_entry->align = 1;
    symtab_entry->type  = SYM_SECTION;
    symtab_entry->bind  = SYM_BIND_LOCAL;
    symtab_entry->name  = StringDuplicate(symbol_name);

    if(strcmp(symbol_name, ".data")   == 0) symtab_entry->align = 4;
    if(strcmp(symbol_name, ".rodata") == 0) symtab_entry->align = 4;

    InsertEntry(symtab_entry);

    Section* new_section = SectionCreateEmpty();
    new_section->index = symtab_entry->index;

    VectorPush(sections, new_section);

    current_section = new_section;
  }
}

void SectionText(void){
  SymbolText();
  SectionSymbol();
}

void SectionData(void){
  SymbolData();
  SectionSymbol();
}

void SectionRodata(void){
  SymbolRodata();
  SectionSymbol();
}

void Global(void){
  AsmDir* dir = AsmDirCreateEmpty();

  dir->directive = DIR_GLOBL;
  for(int i = 0; i < list_size; i++){
    char* symbol_name = StackPop(&symbol_stack);

    if(SymbolIsLocal(symbol_name)){
      ReportError("Symbol with local name (%s) declared global.", symbol_name);
      StringDrop(symbol_name);
      continue;
    }

    AsmExpr* expr = SymbolExpression(symbol_name);
    LinkedListInsertFirstInfo(&dir->arguments, expr);

    VectorPush(global_symbols, symbol_name);
  }
  list_size = 0;

  InsertDirLine(dir);
}

void Extern(void){
  AsmDir* dir = AsmDirCreateEmpty();

  dir->directive = DIR_EXTRN;
  for(int i = 0; i < list_size; i++){
    char* symbol_name = StackPop(&symbol_stack);

    if(SymbolIsLocal(symbol_name)){
      ReportError("Symbol with local name (%s) declared extern.", symbol_name);
      StringDrop(symbol_name);
      continue;
    }

    AsmExpr* expr = SymbolExpression(symbol_name);
    LinkedListInsertFirstInfo(&dir->arguments, expr);
  }

  for(Node* node = dir->arguments.first; node != NULL; node = node->next){
    AsmExpr* symbol_expr  = node->info;
    char* symbol_name     = VectorGet(symbol_expr->plus_symbols, 0);

    SymtabEntry* symtab_entry = SymtabEntryCreateEmpty();
    symtab_entry->value   = 0;
    symtab_entry->size    = 0;
    symtab_entry->align   = 0;
    symtab_entry->type    = SYM_NO_TYPE;
    symtab_entry->bind    = SYM_BIND_GLOBAL;
    symtab_entry->section = SECTION_UND;
    symtab_entry->name    = StringDuplicate(symbol_name);

    InsertEntry(symtab_entry);
  }
  list_size = 0;

  InsertDirLine(dir);
}

void Define(void){
  AsmDir* dir = AsmDirCreateEmpty();
  
  dir->directive = DIR_DEF;
  char* symbol_name = StackPop(&symbol_stack);
  AsmExpr* expr = SymbolExpression(symbol_name);
  LinkedListInsertFirstInfo(&dir->arguments, expr);
  LinkedListInsertLastInfo(&dir->arguments, StackPop(&expression_stack));

  SymtabEntry* symtab_entry = SymtabEntryCreateEmpty();
  symtab_entry->value   = 0;
  symtab_entry->size    = DEFAULT_SYM_SIZE;
  symtab_entry->align   = DEFAULT_SYM_ALIGN;
  symtab_entry->type    = DEFAULT_SYM_TYPE;
  symtab_entry->bind    = SYM_BIND_LOCAL;
  symtab_entry->section = SECTION_DEF;
  symtab_entry->name    = StringDuplicate(symbol_name);

  InsertEntry(symtab_entry);
  VectorPush(def_symbols, dir);

  InsertDirLine(dir);
}

void ByteData(void){
  AsmDir* dir = AsmDirCreateEmpty();

  dir->directive = DIR_BYTE;
  OpenDefaultSection();

  for(int i = 0; i < list_size; i++){
    LinkedListInsertFirstInfo(&dir->arguments, StackPop(&expression_stack));
    SectionIncCounter(current_section, 1);
  }
  list_size = 0;

  InsertDirLine(dir);
}

void WordData(void){
  AsmDir* dir = AsmDirCreateEmpty();

  dir->directive = DIR_WORD;
  OpenDefaultSection();

  for(int i = 0; i < list_size; i++){
    LinkedListInsertFirstInfo(&dir->arguments, StackPop(&expression_stack));
    if(current_section->counter % 2) ReportWarning("Word data address not aligned to 2.");
    SectionIncCounter(current_section, 2);
  }
  list_size = 0;

  InsertDirLine(dir);
}

void LongData(void){
  AsmDir* dir = AsmDirCreateEmpty();

  dir->directive = DIR_LONG;
  OpenDefaultSection();

  for(int i = 0; i < list_size; i++){
    LinkedListInsertFirstInfo(&dir->arguments, StackPop(&expression_stack));
    if(current_section->counter % 4) ReportWarning("Long data address not aligned to 4.");
    SectionIncCounter(current_section, 4);
  }
  list_size = 0;

  InsertDirLine(dir);
}

void ZeroData(void){
  AsmExpr* expr = StackPop(&expression_stack);
  if(VectorSize(expr->plus_symbols) + VectorSize(expr->minus_symbols) > 0){
    ReportError("Zero directive must allocate constant memory size.");
    AsmExprDrop(expr);
    return;
  }
  if(expr->offset < 0){
    ReportError("Zero directive must allocate non-negative memory size.");
    AsmExprDrop(expr);
    return;
  }
  
  AsmDir* dir = AsmDirCreateEmpty();

  dir->directive = DIR_ZERO;
  LinkedListInsertFirstInfo(&dir->arguments, expr);

  InsertDirLine(dir);
  OpenDefaultSection();
  SectionIncCounter(current_section, expr->offset);
}

void SkipData(void){
  AsmExpr* expr = StackPop(&expression_stack);
  if(VectorSize(expr->plus_symbols) + VectorSize(expr->minus_symbols) > 0){
    ReportError("Skip directive must allocate constant memory size.");
    AsmExprDrop(expr);
    return;
  }
  if(expr->offset < 0){
    ReportError("Skip directive must allocate non-negative memory size.");
    AsmExprDrop(expr);
    return;
  }

  AsmDir* dir = AsmDirCreateEmpty();

  dir->directive = DIR_SKIP;
  LinkedListInsertFirstInfo(&dir->arguments, expr);

  InsertDirLine(dir);
  OpenDefaultSection();
  SectionIncCounter(current_section, expr->offset);
}

void Asciiz(void){
  AsmDir* dir = AsmDirCreateEmpty();

  dir->directive = DIR_ASCIZ;
  dir->strlit    = StackPop(&strlit_stack);

  InsertDirLine(dir);
  OpenDefaultSection();
  SectionIncCounter(current_section, *(int*)&dir->strlit[0]);
}

void NoType(void){
  AsmDir*  dir  = AsmDirCreateEmpty();

  dir->directive = DIR_NOTYP;
  char* symbol_name = StackPop(&symbol_stack);
  AsmExpr* expr = SymbolExpression(symbol_name);
  LinkedListInsertFirstInfo(&dir->arguments, expr);

  InsertDirLine(dir);
}

void Function(void){
  AsmDir*  dir  = AsmDirCreateEmpty();

  dir->directive = DIR_FUNCT;
  char* symbol_name = StackPop(&symbol_stack);
  AsmExpr* expr = SymbolExpression(symbol_name);
  LinkedListInsertFirstInfo(&dir->arguments, expr);

  InsertDirLine(dir);
}

void Object(void){
  AsmDir*  dir  = AsmDirCreateEmpty();

  dir->directive = DIR_OBJCT;
  char* symbol_name = StackPop(&symbol_stack);
  AsmExpr* expr = SymbolExpression(symbol_name);
  LinkedListInsertFirstInfo(&dir->arguments, expr);

  InsertDirLine(dir);
}