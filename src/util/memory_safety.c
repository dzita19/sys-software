#include "memory_safety.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define TAB_LENGTH 12
#define PRINT_REPORT(symbol_name, symbol_alloc, symbol_free)\
  printf("--" #symbol_name); \
  for(int i = 0; i < 16 - strlen(#symbol_name); i++) printf(" "); \
  printf("allocated = %4d, freed = %4d;", symbol_alloc, symbol_free);\
  if(symbol_alloc > symbol_free) { \
    printf(" <-- WARNING\n");\
  } \
  else printf("\n")

// symtab
int symtab_entry_alloc  = 0;
int symtab_entry_free   = 0;
int symtab_alloc        = 0;
int symtab_free         = 0;
int section_alloc       = 0;
int section_free        = 0;
int relatab_entry_alloc = 0;
int relatab_entry_free  = 0;
int relatab_alloc       = 0;
int relatab_free        = 0;
int obj_data_alloc      = 0;
int obj_data_free       = 0;

// assembler
int asm_expr_alloc      = 0;
int asm_expr_free       = 0;
int asm_operand_alloc   = 0;
int asm_operand_free    = 0;
int asm_dir_alloc       = 0;
int asm_dir_free        = 0;
int asm_instr_alloc     = 0;
int asm_instr_free      = 0;
int asm_line_alloc      = 0;
int asm_line_free       = 0;

// linker
int global_def_alloc    = 0;
int global_def_free     = 0;

// util
int linked_list_alloc   = 0;
int linked_list_free    = 0;
int node_alloc          = 0;
int node_free           = 0;
int string_alloc        = 0;
int string_free         = 0;
int vector_alloc        = 0;
int vector_free         = 0;
int byte_vector_alloc   = 0;
int byte_vector_free    = 0;
int array_alloc         = 0;
int array_free          = 0;

char* StringDuplicate(const char* str){
  char* string = strdup(str); 
  string_alloc++;

  return string;
}

char* StringAllocate(int size){
  char* string = malloc(sizeof(char) * size);
  string_alloc++;

  return string;
}

void StringDrop(const char* string){
  free((void*)string);
  string_free++;
}

void memory_safety_report(){
  printf("Memory safety report: [\n");
  printf("-Symtab: [\n");
  PRINT_REPORT(SymtabEntry,  symtab_entry_alloc,  symtab_entry_free);
  PRINT_REPORT(Symtab,       symtab_alloc,        symtab_free);
  PRINT_REPORT(Section,      section_alloc,       section_free);
  PRINT_REPORT(RelatabEntry, relatab_entry_alloc, relatab_entry_free);
  PRINT_REPORT(Relatab,      relatab_alloc,       relatab_free);
  PRINT_REPORT(ObjData,      obj_data_alloc,      obj_data_free);
  printf("-]\n");
  printf("-Assembler: [\n");
  PRINT_REPORT(AsmExpr,      asm_expr_alloc,      asm_expr_free);
  PRINT_REPORT(AsmOperand,   asm_operand_alloc,   asm_operand_free);
  PRINT_REPORT(AsmDir,       asm_dir_alloc,       asm_dir_free);
  PRINT_REPORT(AsmInstr,     asm_instr_alloc,     asm_instr_free);
  PRINT_REPORT(AsmLine,      asm_line_alloc,      asm_line_free);
  printf("-]\n");
  printf("-Linker: [\n");
  PRINT_REPORT(GlobalDef,    global_def_alloc,    global_def_free);
  printf("-]\n");
  printf("-Utility: [\n");
  PRINT_REPORT(LinkedList,   linked_list_alloc,   linked_list_free);
  PRINT_REPORT(Node,         node_alloc,          node_free);
  PRINT_REPORT(String,       string_alloc,        string_free);
  PRINT_REPORT(Vector,       vector_alloc,        vector_free);
  PRINT_REPORT(ByteVector,   byte_vector_alloc,   byte_vector_free);
  PRINT_REPORT(Array,        array_alloc,         array_free);
  printf("-]\n");
  printf("]\n");
}