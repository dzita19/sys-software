#include "global_def.h"

#include <stdio.h>
#include <stdlib.h>
#include "util/memory_safety.h"

GlobalDef* GlobalDefCreateEmpty(void){
  GlobalDef* global_def = malloc(sizeof(GlobalDef));
  global_def->name  = NULL;
  global_def->file  = 0;
  global_def->entry = 0;
  global_def->link  = 0;

  global_def_alloc++;

  return global_def;
}

void GlobalDefDrop(GlobalDef* global_def){
  if(global_def->name != NULL) StringDrop(global_def->name);
  free(global_def);

  global_def_free++;
}

void GlobalDefDump(GlobalDef* global_def){
  
}
