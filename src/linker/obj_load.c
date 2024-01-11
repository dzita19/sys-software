#include "obj_load.h"

#include <string.h>
  
static char* scan_string_safe(FILE* file){
  ByteVector* buffer = ByteVectorInit();

  char c = fgetc(file);
  while(c != ' ' && c != '\n' && c != '\0' ){
    ByteVectorPush(buffer, c);
    c = fgetc(file);
  }
  ByteVectorPush(buffer, '\0');

  char* dst = StringAllocate(strlen((char*)buffer->content));
  strcpy(dst, (char*)buffer->content);

  ByteVectorDrop(buffer);

  return dst;
}

ObjData* LoadObjData(FILE* file){
  ObjData* data = ObjDataCreateEmpty();

  // symtab
  int symtab_size = 0;
  fscanf(file, "%d", &symtab_size);

  for(int i = 0; i < symtab_size; i++){
    SymtabEntry* entry = SymtabEntryCreateEmpty();
    entry->index = i;
    fscanf(file, "%d %d %d %d %d %d ",
      &entry->value,
      &entry->size,
      (int*)&entry->align,
      (int*)&entry->type,
      (int*)&entry->bind,
      &entry->section
    );
    entry->name = scan_string_safe(file);

    SymtabInsertEntry(data->symtab, entry);
  }

  int num_of_sections = 0;
  fscanf(file, "%d", &num_of_sections);

  for(int i = 0; i < num_of_sections; i++){
    Section* section = SectionCreateEmpty();
    fscanf(file, "%d %d", &section->index, &section->counter);

    for(int j = 0; j < section->counter; j++){
      unsigned byte = 0;
      fscanf(file, "%02x", &byte);
      ByteVectorPush(section->content, (Byte)byte);
    }

    VectorPush(data->sections, section);
  }

  for(int i = 0; i < num_of_sections; i++){
    Relatab* relatab = RelatabCreateEmpty();
    int relatab_size = 0;
    fscanf(file, "%d", &relatab_size);

    for(int j = 0; j < relatab_size; j++){
      RelatabEntry* entry = RelatabEntryCreateEmpty();
      fscanf(file, "%d %d %d %d",
        &entry->offset,
        (int*)&entry->type,
        &entry->symbol,
        &entry->addend
      );
      RelatabInsertEntry(relatab, entry);
    }

    VectorPush(data->relatabs, relatab);
  }

  return data;
}