#include "relatab.h"

#include <stdio.h>
#include <stdlib.h>
#include "util/memory_safety.h"

RelatabEntry* RelatabEntryCreateEmpty(void){
  RelatabEntry* entry = malloc(sizeof(RelatabEntry));
  entry->offset = 0;
  entry->type   = 0;
  entry->symbol = 0;
  entry->addend = 0;

  relatab_entry_alloc++;

  return entry;
}

void RelatabEntryDrop(RelatabEntry* entry){
  free(entry);

  relatab_entry_free++;
}

void RelatabEntryDump(RelatabEntry* entry){
  static const char* rela_type_print[] = {
    [RELA_CODE_32] = "CODE32",
    [RELA_DATA_8 ] = "DATA8",
    [RELA_DATA_16] = "DATA16",
    [RELA_DATA_32] = "DATA32",
  };

  printf("Offset: %4d, Type: %6s, Symbol: %4d, Addend: %4d",
    entry->offset,
    rela_type_print[entry->type],
    entry->symbol,
    entry->addend);
}


typedef Vector Relatab;

Relatab* RelatabCreateEmpty(void){
  Relatab* relatab = VectorInit();
  
  relatab_alloc++;

  return relatab;
}

void RelatabDrop(Relatab* relatab){
  for(int i = 0; i < VectorSize(relatab); i++){
    RelatabEntryDrop(VectorGet(relatab, i));
  }
  VectorDrop(relatab);

  relatab_free++;
}

void RelatabDump(Relatab* relatab){
  for(int i = 0; i < VectorSize(relatab); i++){
    printf("  ");
    RelatabEntryDump(VectorGet(relatab, i));
    printf("\n");
  }
}


void RelatabInsertEntry(Relatab* relatab, RelatabEntry* entry){
  VectorPush(relatab, entry);
}

RelatabEntry* RelatabGetEntry(Relatab* relatab, int index){
  return VectorGet(relatab, index);
}