#include "obj_data.h"

#include <stdio.h>
#include <stdlib.h>

ObjData* ObjDataCreateEmpty(void){
  ObjData* obj_data  = malloc(sizeof(ObjData));
  obj_data->symtab   = SymtabCreateEmpty();
  obj_data->sections = VectorInit();
  obj_data->relatabs = VectorInit();

  obj_data_alloc++;

  return obj_data;
}

void ObjDataDrop(ObjData* obj_data){
  SymtabDrop(obj_data->symtab);

  for(int i = 0; i < VectorSize(obj_data->sections); i++){
    SectionDrop(VectorGet(obj_data->sections, i));
  }
  VectorDrop(obj_data->sections);

  for(int i = 0; i < VectorSize(obj_data->relatabs); i++){
    RelatabDrop(VectorGet(obj_data->relatabs, i));
  }
  VectorDrop(obj_data->relatabs);

  free(obj_data);

  obj_data_free++;
}

void ObjDataDump(ObjData* obj_data){
  printf("OBJ DATA: [\n");
  printf("Symtab:[\n");
  SymtabDump(obj_data->symtab);
  printf("]\n");

  printf("SECTIONS: [\n");
  for(int i = 0; i < VectorSize(obj_data->sections); i++){
    Section* section = VectorGet(obj_data->sections, i);
    SymtabEntry* section_entry = SymtabGetEntry(obj_data->symtab, section->index);
    printf("SECTION %s", section_entry->name);
    SectionDump(section);
    printf("\n");
  }
  printf("]\n");

  printf("RELATABS: [\n");
  for(int i = 0; i < VectorSize(obj_data->relatabs); i++){
    Relatab* relatab = VectorGet(obj_data->relatabs, i);
    Section* section = VectorGet(obj_data->sections, i);
    SymtabEntry* section_entry = SymtabGetEntry(obj_data->symtab, section->index);
    printf("RELATAB .rela%s\n", section_entry->name);
    RelatabDump(relatab);
  }
  printf("]\n");

  printf("]\n");
}