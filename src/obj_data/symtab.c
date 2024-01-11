#include "symtab.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "util/memory_safety.h"

SymtabEntry* SymtabEntryCreateEmpty(){
  SymtabEntry* entry = malloc(sizeof(SymtabEntry));
  entry->index   = 0;
  entry->value   = 0;
  entry->size    = 0;
  entry->align   = 0;
  entry->type    = 0;
  entry->bind    = 0;
  entry->section = 0;
  entry->name    = NULL;

  symtab_entry_alloc++;

  return entry;
}

void SymtabEntryDrop(SymtabEntry* entry){
  if(entry->name) StringDrop(entry->name);
  free(entry);

  symtab_entry_free++;
}

void SymtabEntryDump(SymtabEntry* entry){
  static const char* type_print[] = {
    [SYM_NO_TYPE]  = "NOTYP",
    [SYM_SECTION]  = "SECTN",
    [SYM_FUNCTION] = "FUNCT",
    [SYM_OBJECT]   = "OBJCT",
  };

  static const char* bind_print[] = {
    [SYM_BIND_LOCAL]  = "LOCAL",
    [SYM_BIND_GLOBAL] = "GLOBL",
  };

  printf("Index: %4d, Value: %4d, Size: %4d, Align: %4d, Type: %5s, Bind: %5s, Section: %4d, Name: %s",
    entry->index,
    entry->value,
    entry->size,
    entry->align,
    type_print[entry->type],
    bind_print[entry->bind],
    entry->section,
    entry->name);
}

Symtab* SymtabCreateEmpty(){
  Symtab* symtab = VectorInit();

  symtab_alloc++;

  return symtab;
}

void SymtabDrop(Symtab* symtab){
  for(int i = 0; i < VectorSize(symtab); i++){
    SymtabEntryDrop(VectorGet(symtab, i));
  }
  VectorDrop(symtab);

  symtab_free++;
}

void SymtabDump(Symtab* symtab){
  for(int i = 0; i < VectorSize(symtab); i++){
    SymtabEntryDump(VectorGet(symtab, i));
    printf("\n");
  }
}

SymtabEntry* SymtabInsertEntry(Symtab* symtab, SymtabEntry* entry){
  for(int i = 0; i < VectorSize(symtab); i++){
    SymtabEntry* current_entry = VectorGet(symtab, i);
    if(strcmp(current_entry->name, entry->name) == 0) {
      SymtabEntryDrop(entry);
      return NULL;
    }
  }

  entry->index = VectorSize(symtab);
  if(entry->type == SYM_SECTION) entry->section = VectorSize(symtab);
  VectorPush(symtab, entry);

  return entry;
}

SymtabEntry* SymtabGetEntry(Symtab* symtab, int index){
  return VectorGet(symtab, index);
}

SymtabEntry* SymtabFindEntry(Symtab* symtab, const char* symbol_name){
  for(int i = 0; i < VectorSize(symtab); i++){
    SymtabEntry* entry = VectorGet(symtab, i);
    if(strcmp(entry->name, symbol_name) == 0) return entry;
  }

  return NULL;
}