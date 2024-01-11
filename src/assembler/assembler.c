#include "assembler.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "logger_asm.h"
#include "util/memory_safety.h"

AsmDir* AsmDirCreateEmpty(void){
  AsmDir* dir    = malloc(sizeof(AsmDir));
  dir->directive = (AsmDirective)0;
  dir->arguments = (LinkedList){ NULL, NULL };

  asm_dir_alloc++;

  return dir;
}

void AsmDirDrop(AsmDir* dir){
  for(Node* node = dir->arguments.first; node != NULL; node = node->next){
    AsmExprDrop(node->info);
  }
  LinkedListDelete(&dir->arguments);
  if(dir->strlit) StringDrop(dir->strlit);
  free(dir);

  asm_dir_free++;
}

void AsmDirDump(AsmDir* dir){
  printf("%s ", dir_names[dir->directive]);
  for(Node* node = dir->arguments.first; node != NULL; node = node->next){
    AsmExprDump(node->info);
    if(node->next) printf(", ");
  }
}


AsmInstr* AsmInstrCreateEmpty(void){
  AsmInstr* instr    = malloc(sizeof(AsmInstr));
  instr->instruction = (AsmInstruction)0;
  instr->operands    = (LinkedList){ NULL, NULL };
  instr->size        = 0;

  asm_instr_alloc++;

  return instr;
}

void AsmInstrDrop(AsmInstr* instr){
  for(Node* node = instr->operands.first; node != NULL; node = node->next){
    AsmOperandDrop(node->info);
  }
  LinkedListDelete(&instr->operands);
  free(instr);

  asm_instr_free++;
}

void AsmInstrDump(AsmInstr* instr){
  printf("%s ", instr_names[instr->instruction]);
  for(Node* node = instr->operands.first; node != NULL; node = node->next){
    AsmOperandDump(node->info);
    if(node->next) printf(", ");
  }
  printf(" (%dB)", instr->size);
}

AsmLine* AsmLineCreateEmpty(void){
  AsmLine* line = malloc(sizeof(AsmLine));
  line->kind    = 0;
  line->row     = 0;
  line->content = NULL;

  asm_line_alloc++;

  return line;
}

void AsmLineDrop(AsmLine* line){
  if(line->kind == LINE_DIR){
    AsmDirDrop(line->content);
  }
  else if(line->kind == LINE_INSTR){
    AsmInstrDrop(line->content);
  }
  free(line);

  asm_line_free++;
}

void AsmLineDump(AsmLine* line){
  printf("Line %4d: ", line->row);
  if(line->kind == LINE_DIR){
    AsmDirDump(line->content);
  }
  else if(line->kind == LINE_INSTR){
    AsmInstrDump(line->content);
  }
}



Stack expression_stack = (Stack){ NULL };
Stack operand_stack    = (Stack){ NULL };

FILE* asout              = 0;
int semantic_errors      = 0;

Vector*  program         = 0;
Symtab*  symtab          = 0;
Symtab*  local_symtab    = 0;
Vector*  sections        = 0;
Vector*  relatabs        = 0;
Section* current_section = 0;
Relatab* current_relatab = 0;
Vector*  def_symbols     = 0;
Vector*  global_symbols  = 0;



static void UndefinedSymbol(void){
  SymtabEntry* entry = SymtabEntryCreateEmpty();
  entry->value   = 0;
  entry->size    = 0;
  entry->align   = 0;
  entry->type    = SYM_SECTION;
  entry->bind    = SYM_BIND_LOCAL;\
  entry->name    = StringDuplicate("*und*");

  InsertEntry(entry);
}

static void AbsoluteSymbol(void){
  SymtabEntry* entry = SymtabEntryCreateEmpty();
  entry->value   = 0;
  entry->size    = 0;
  entry->align   = 0;
  entry->type    = SYM_SECTION;
  entry->bind    = SYM_BIND_LOCAL;
  entry->name    = StringDuplicate("*abs*");

  InsertEntry(entry);
}

void AssemblerInit(void){
  program        = VectorInit();
  symtab         = SymtabCreateEmpty();
  local_symtab   = SymtabCreateEmpty();
  sections       = VectorInit();
  relatabs       = VectorInit();
  def_symbols    = VectorInit();
  global_symbols = VectorInit();
  
  UndefinedSymbol();
  AbsoluteSymbol();
}

void AssemblerFree(void){
  for(int i = 0; i < VectorSize(program); i++){
    AsmLineDrop(VectorGet(program, i));
  }
  VectorDrop(program);

  SymtabDrop(symtab);
  SymtabDrop(local_symtab);

  for(int i = 0; i < VectorSize(sections); i++){
    SectionDrop(VectorGet(sections, i));
  }
  VectorDrop(sections);

  for(int i = 0; i < VectorSize(relatabs); i++){
    RelatabDrop(VectorGet(relatabs, i));
  }
  VectorDrop(relatabs);

  // global symbols are not owned - dropped by expression
  VectorDrop(global_symbols);

  // def symbols are not owned - dropped by program
  VectorDrop(def_symbols);
}

void AssemblerPrint(void){
  printf("SYMTAB: [\n");
  SymtabDump(symtab);
  printf("]\n");

  printf("LOCAL SYMTAB: [\n");
  SymtabDump(local_symtab);
  printf("]\n");

  printf("DEF SYMBOLS: [\n");
  for(int i = 0; i < VectorSize(def_symbols); i++){
    printf("  ");
    AsmDirDump(VectorGet(def_symbols, i));
    printf("\n");
  }
  printf("]\n");

  printf("GLOBAL SYMBOLS: [\n");
  for(int i = 0; i < VectorSize(global_symbols); i++){
    printf("  %s\n", (const char*)VectorGet(global_symbols, i));
  }
  printf("]\n");

  printf("PROGRAM: [\n");
  for(int i = 0; i < VectorSize(program); i++){
    printf("  ");
    AsmLineDump(VectorGet(program, i));
    printf("\n");
  }
  printf("]\n");

  printf("SECTIONS: [\n");
  for(int i = 0; i < VectorSize(sections); i++){
    Section* section = VectorGet(sections, i);
    SymtabEntry* section_entry = SymtabGetEntry(symtab, section->index);
    printf("SECTION %s", section_entry->name);
    SectionDump(section);
    printf("\n");
  }
  printf("]\n");

  printf("RELATABS: [\n");
  for(int i = 0; i < VectorSize(relatabs); i++){
    Relatab* relatab = VectorGet(relatabs, i);
    Section* section = VectorGet(sections, i);
    SymtabEntry* section_entry = SymtabGetEntry(symtab, section->index);
    printf("RELATAB .rela%s\n", section_entry->name);
    RelatabDump(relatab);
  }
  printf("]\n");
}



void InsertDirLine(AsmDir* dir){
  extern int row;

  AsmLine* line = AsmLineCreateEmpty();
  line->kind    = LINE_DIR;
  line->row     = row;
  line->content = dir;

  VectorPush(program, line);
}

void InsertInstrLine(AsmInstr* instr){
  extern int row;

  AsmLine* line = AsmLineCreateEmpty();
  line->kind    = LINE_INSTR;
  line->row     = row;
  line->content = instr;

  VectorPush(program, line);
}


int SymbolIsLocal(const char* symbol_name){
  return symbol_name[0] == '.' && symbol_name[1] == 'L';
}

SymtabEntry* FindEntry(const char* symbol_name){
  return SymbolIsLocal(symbol_name)
    ? SymtabFindEntry(local_symtab, symbol_name)
    : SymtabFindEntry(symtab, symbol_name);
}

void InsertEntry(SymtabEntry* entry){
  SymtabEntry* insert = SymbolIsLocal(entry->name)
    ? SymtabInsertEntry(local_symtab, entry)
    : SymtabInsertEntry(symtab, entry);

  if(insert == NULL){
    ReportError("Symbol %s already defined.", entry->name);
  }
}

Section* FindSection(const char* section_name){
  for(int i = 0; i < VectorSize(sections); i++){
    Section* curr_section = VectorGet(sections, i);
    SymtabEntry* entry = SymtabGetEntry(symtab, curr_section->index);
    if(strcmp(entry->name, section_name) == 0){
      return curr_section;
    }
  }

  return NULL;
}

void OpenDefaultSection(void){
  if(current_section != NULL) return;
  
  SectionText();
  current_section = VectorGet(sections, 0);
}

void Line(void){
  extern int column, row;
  column = 1;
  row++;
}

void Label(void){
  OpenDefaultSection();
  char* symbol_name = StackPop(&symbol_stack);

  SymtabEntry* symtab_entry = SymtabEntryCreateEmpty();
  symtab_entry->value   = SectionGetCounter(current_section);
  symtab_entry->size    = DEFAULT_SYM_SIZE;
  symtab_entry->align   = DEFAULT_SYM_ALIGN;
  symtab_entry->type    = DEFAULT_SYM_TYPE;
  symtab_entry->bind    = SYM_BIND_LOCAL;
  symtab_entry->section = current_section->index;
  symtab_entry->name    = symbol_name;

  InsertEntry(symtab_entry);
}

void ResolveGlobalSymbols(void){
  for(int i = 0; i < VectorSize(global_symbols); i++){
    const char* symbol_name = VectorGet(global_symbols, i);
    SymtabEntry* entry = FindEntry(symbol_name);
    if(entry == NULL){
      ReportError("Global symbol %s is not defined.", symbol_name);
      continue;
    }

    if(entry->bind == SYM_BIND_GLOBAL){
      ReportError("Symbol %s is declared global multiple times.", symbol_name);
      continue;
    }

    if(entry->type == SYM_SECTION){
      ReportError("Section symbol %s is declared global.", symbol_name);
      continue;
    }

    entry->bind = SYM_BIND_GLOBAL;
  }
}

AsmExpr* ResolveExpression(AsmExpr* expr){
  LinkedList* plus_sections  = LinkedListCreateEmpty();
  LinkedList* minus_sections = LinkedListCreateEmpty();
  int offset = expr->offset;

  int resolved = 1;
  for(int i = 0; i < VectorSize(expr->plus_symbols); i++){
    char* symbol_name  = VectorGet(expr->plus_symbols, i);
    SymtabEntry* entry = FindEntry(symbol_name);
    if(entry == NULL) {
      ReportError("Expression depends on undefined symbol(s).");
      resolved = 0;
      break;
    }

    if(entry->section == SECTION_DEF){
      resolved = 0;
      break;
    }

    // if symbol has UND section, it is an extern symbol (it's treated as an independent section)
    int plus_section = entry->section == SECTION_UND ? entry->index : entry->section;
    if(plus_section != SECTION_ABS) LinkedListInsertLastInfo(plus_sections, (void*)(long)plus_section);
    offset += entry->value;
  }

  for(int i = 0; i < VectorSize(expr->minus_symbols); i++){
    char* symbol_name  = VectorGet(expr->minus_symbols, i);
    SymtabEntry* entry = FindEntry(symbol_name);
    if(entry == NULL) {
      ReportError("Expression depends on undefined symbol(s).");
      resolved = 0;
      break;
    }

    if(entry->section == SECTION_DEF){
      resolved = 0;
      break;
    }

    // if symbol has UND section, it is an extern symbol (it's treated as an independent section)
    int minus_section = entry->section == SECTION_UND ? entry->index : entry->section;
    if(minus_section != SECTION_ABS) LinkedListInsertLastInfo(minus_sections, (void*)(long)minus_section);
    offset -= entry->value;
  }

  AsmExpr* ret_expr = 0;

  if(resolved){
    Node* plus_node = plus_sections->first;
    while(plus_node != NULL){
      int found = 0;
      for(Node* minus_node = minus_sections->first; minus_node != NULL; minus_node = minus_node->next){
        if(plus_node->info == minus_node->info) {
          NodeDrop(LinkedListRemoveFrom(minus_sections, minus_node));
          found = 1;
          break;
        }
      }
      
      if(found){
        Node* old_node = plus_node;
        plus_node = plus_node->next;
        NodeDrop(LinkedListRemoveFrom(plus_sections, old_node));
      }
      else plus_node = plus_node->next;
    }

    ret_expr = AsmExprCreateEmpty();

    // add new dependencies to expression
    for(Node* plus_node = plus_sections->first; plus_node != NULL; plus_node = plus_node->next){
      SymtabEntry* entry = SymtabGetEntry(symtab, (int)(long)plus_node->info);
      VectorPush(ret_expr->plus_symbols, StringDuplicate(entry->name));
    }
    for(Node* minus_node = minus_sections->first; minus_node != NULL; minus_node = minus_node->next){
      SymtabEntry* entry = SymtabGetEntry(symtab, (int)(long)minus_node->info);
      VectorPush(ret_expr->minus_symbols, StringDuplicate(entry->name));
    }
    ret_expr->offset = offset;
  }

  LinkedListDrop(plus_sections);
  LinkedListDrop(minus_sections);

  return ret_expr;
}

void ResolveDefSymbols(void){
  int finished = 0;
  int progress = 1;
  while(progress && finished < VectorSize(def_symbols)){
    progress = 0;
    for(int i = 0; i < VectorSize(def_symbols); i++){
      AsmDir* def_dir = VectorGet(def_symbols, i);

      // fetch symbol that is being defined
      AsmExpr* def_symbol_expr = def_dir->arguments.first->info;
      char* symbol_name = VectorGet(def_symbol_expr->plus_symbols, 0);
      SymtabEntry* def_symbol = FindEntry(symbol_name);
      if(def_symbol->section != SECTION_DEF) continue;

      // resolve expression - second in the argument list
      AsmExpr* resolved = ResolveExpression(def_dir->arguments.first->next->info);

      if(resolved != NULL) {

        // add symbol to abs section
        if(VectorSize(resolved->plus_symbols) == 0 && VectorSize(resolved->minus_symbols) == 0){
          def_symbol->section = SECTION_ABS;
          def_symbol->value   = resolved->offset;

          finished++;
          progress = 1;
        }
        else if(VectorSize(resolved->plus_symbols) == 1 && VectorSize(resolved->minus_symbols) == 0){
          SymtabEntry* section_symbol = SymtabFindEntry(symtab, (char*)VectorGet(resolved->plus_symbols, 0));
          def_symbol->section = section_symbol->index;
          def_symbol->value   = resolved->offset;
        
          finished++;
          progress = 1;
        }
        else {
          ReportError("Def symbol depends on multiple sections.");
        }

        AsmExprDrop(resolved);
      }
    }
  }

  if(finished != VectorSize(def_symbols)) ReportError("Def symbols could not be resolved.");
}
