#ifndef _LINES_H_
#define _LINES_H_

#include "machine/architecture.h"
#include "directive.h"
#include "expression.h"
#include "instruction.h"
#include "literals.h"
#include "obj_data/symtab.h"
#include "obj_data/section.h"
#include "obj_data/relatab.h"
#include "util/linked_list.h"
#include "util/vector.h"
#include "util/stack.h"
#include <stdio.h>

typedef struct AsmDir{
  AsmDirective directive;
  LinkedList   arguments; // initially LinkedList(AsmExpression*)
  char*        strlit;
} AsmDir;

extern AsmDir* AsmDirCreateEmpty(void);
extern void AsmDirDrop(AsmDir*);
extern void AsmDirDump(AsmDir*);

typedef struct AsmInstr{
  AsmInstruction instruction;
  LinkedList     operands; // LinkedList(AsmOperand*)
  int size;
} AsmInstr;

extern AsmInstr* AsmInstrCreateEmpty(void);
extern void AsmInstrDrop(AsmInstr*);
extern void AsmInstrDump(AsmInstr*);

#define LINE_DIR   1
#define LINE_INSTR 2

typedef struct AsmLine{
  int   kind;
  int   row;
  void* content;
} AsmLine;

extern AsmLine* AsmLineCreateEmpty(void);
extern void AsmLineDrop(AsmLine*);
extern void AsmLineDump(AsmLine*);

extern Stack expression_stack;
extern Stack operand_stack;

extern FILE*    asout;
extern int      semantic_errors;

extern Vector*  program;  // Vector(AsmLine*)
extern Symtab*  symtab;
extern Symtab*  local_symtab;
extern Vector*  sections; // Vector(Section*)
extern Vector*  relatabs; // Vector(Relatab*)
extern Section* current_section;
extern Relatab* current_relatab;

extern Vector*  def_symbols; // Vector(AsmDir*)
extern Vector*  global_symbols; // LinkedList(char*)

extern void AssemblerInit(void);
extern void AssemblerFree(void);
extern void AssemblerPrint(void);
 
extern void InsertDirLine(AsmDir*);
extern void InsertInstrLine(AsmInstr*);

extern int  SymbolIsLocal(const char* symbol_name);
extern SymtabEntry* FindEntry(const char* symbol_name);
extern void InsertEntry(SymtabEntry* entry);
extern Section* FindSection(const char* section_name);
extern void OpenDefaultSection(void);

extern void Line(void);
extern void Label(void);

extern void ResolveGlobalSymbols(void);
extern AsmExpr* ResolveExpression(AsmExpr*);
extern void ResolveDefSymbols(void);

#endif