#include "linker.h"

#include "global_def.h"
#include "util/vector.h"
#include "util/array.h"
#include "obj_data/obj_data.h"
#include "obj_load.h"
#include "logger_ld.h"
#include <string.h>

int link_errors = 0;

FILE* ldout = 0;

static Vector* loaded_data     = 0; // Vector(ObjData*)
static Vector* import_symbols  = 0; // Vector(GlobalDef*)
static Vector* export_symbols  = 0; // Vector(GlobalDef*)
static Vector* sorted_sections = 0; // Vector(GlobalDef*)

static void CollectAllSymbols(void){
  for(int i = 0; i < VectorSize(loaded_data); i++){
    ObjData* current_data = VectorGet(loaded_data, i);
    Symtab* current_symtab = current_data->symtab;
    for(int j = 0; j < VectorSize(current_symtab); j++){
      SymtabEntry* current_symbol = SymtabGetEntry(current_symtab, j);
      if(current_symbol->bind == SYM_BIND_GLOBAL){
        GlobalDef* global_def = GlobalDefCreateEmpty();
        global_def->file  = i;
        global_def->entry = j;
        global_def->name  = StringDuplicate(current_symbol->name);
        global_def->link  = NULL;

        if(current_symbol->section == SECTION_UND) VectorPush(import_symbols, global_def);
        else VectorPush(export_symbols, global_def);
      }
    }
  }
}

static void CheckExports(void){
  for(int i = 0; i < VectorSize(export_symbols); i++){
    GlobalDef* def1 = VectorGet(export_symbols, i);
    for(int j = i + 1; j < VectorSize(export_symbols); j++){
      GlobalDef* def2 = VectorGet(export_symbols, j);
      if(strcmp(def1->name, def2->name) == 0){
        ReportError("Symbol %s defined multiple times.", def1->name);
      }
    }
  }
}

static void LinkImports(void){
  for(int i = 0; i < VectorSize(import_symbols); i++){
    GlobalDef* import_def = VectorGet(import_symbols, i);
    for(int j = 0; j < VectorSize(export_symbols); j++){
      GlobalDef* export_def = VectorGet(export_symbols, j);
      if(strcmp(import_def->name, export_def->name) == 0){
        import_def->link = export_def;
        break;
      }
    }
    if(import_def->link == NULL){
      ReportError("Symbol %s is not defined.", import_def->name);
    }
  }
}

static void SortSections(void){
  // init added with 0 (no section is added - allocated yet)
  // no need for two indirections
  Vector* added = VectorInit();
  for(int i = 0; i < VectorSize(loaded_data); i++){
    ObjData* obj_data = VectorGet(loaded_data, i);
    for(int j = 0; j < VectorSize(obj_data->sections); j++){
      VectorPush(added, (void*)(long)0);
    }
  }

  int counter = 0; // used to reference Vector added
  for(int i = 0; i < VectorSize(loaded_data); i++){
    ObjData* obj_data = VectorGet(loaded_data, i);
    for(int j = 0; j < VectorSize(obj_data->sections); j++, counter++){
      int is_added = (int)(long)VectorGet(added, counter);

      if(is_added != 0) continue;

      VectorSet(added, counter, (void*)(long)1);

      // section that is currently being put
      Section* section   = VectorGet(obj_data->sections, j);
      SymtabEntry* entry = SymtabGetEntry(obj_data->symtab, section->index);

      GlobalDef* global_def = GlobalDefCreateEmpty();
      global_def->file  = i;
      global_def->entry = j;
      VectorPush(sorted_sections, global_def);

      int search_counter = counter + 1;

      for(int ii = i; ii < VectorSize(loaded_data); ii++){
        ObjData* search_obj_data = VectorGet(loaded_data, ii);
        for(int jj = ii == i ? j + 1 : 0; jj < VectorSize(search_obj_data->sections); jj++, search_counter++){
          int search_is_added = (int)(long)VectorGet(added, search_counter);

          if(search_is_added != 0) continue;

          Section* search_section   = VectorGet(search_obj_data->sections, jj);
          SymtabEntry* search_entry = SymtabGetEntry(search_obj_data->symtab, search_section->index);

          if(strcmp(entry->name, search_entry->name) == 0){
            VectorSet(added, search_counter, (void*)(long)1);
            GlobalDef* global_def = GlobalDefCreateEmpty();
            global_def->file  = ii;
            global_def->entry = jj;
            VectorPush(sorted_sections, global_def);
          }
        }
      }
    }    
  }

  VectorDrop(added);
}

static void CalculateSectionsOffsets(void){
  int curr_addr = 0;
  for(int i = 0; i < VectorSize(sorted_sections); i++){
    GlobalDef* section_def = VectorGet(sorted_sections, i);
    ObjData* obj_data = VectorGet(loaded_data, section_def->file);
    Section* section  = VectorGet(obj_data->sections, section_def->entry);
    SymtabEntry* section_symbol = VectorGet(obj_data->symtab, section->index);

    section_symbol->value = (curr_addr + section_symbol->align - 1) / section_symbol->align * section_symbol->align;
    curr_addr = section_symbol->value + section_symbol->size;
  }
}

static void CalculateAbsoluteValues(void){
  for(int i = 0; i < VectorSize(loaded_data); i++){
    ObjData* obj_data = VectorGet(loaded_data, i);
    for(int j = 0; j < VectorSize(obj_data->symtab); j++){
      SymtabEntry* entry = VectorGet(obj_data->symtab, j);

      if(entry->index == entry->section) continue;
      if(entry->index == SECTION_UND)    continue;

      SymtabEntry* section_entry = VectorGet(obj_data->symtab, entry->section);

      entry->value += section_entry->value;
    }
  }

  for(int i = 0; i < VectorSize(import_symbols); i++){
    GlobalDef* import_def = VectorGet(import_symbols, i);
    GlobalDef* export_def = import_def->link;
    ObjData* import_data  = VectorGet(loaded_data, import_def->file);
    ObjData* export_data  = VectorGet(loaded_data, export_def->file);
    SymtabEntry* import_symbol = VectorGet(import_data->symtab, import_def->entry);
    SymtabEntry* export_symbol = VectorGet(export_data->symtab, export_def->entry);

    import_symbol->value = export_symbol->value;
  }
}

static void CalculateRelocation(ObjData* obj_data, Section* section, RelatabEntry* entry){
  switch(entry->type){
  case RELA_CODE_32:{
    SymtabEntry* symbol_ref = VectorGet(obj_data->symtab, entry->symbol);
    SymtabEntry* section_symbol = VectorGet(obj_data->symtab, section->index);
    int value = symbol_ref->value - (entry->offset + section_symbol->value) + entry->addend;
    SectionFixCodeLong(section, entry->offset, value);
  } break;
  case RELA_DATA_8:
  case RELA_DATA_16:
  case RELA_DATA_32:{
    SymtabEntry* symbol_ref = VectorGet(obj_data->symtab, entry->symbol);
    int value = symbol_ref->value + entry->addend;
    if(entry->type == RELA_DATA_8)  SectionFixDataByte(section, entry->offset, value);
    if(entry->type == RELA_DATA_16) SectionFixDataWord(section, entry->offset, value);
    if(entry->type == RELA_DATA_32) SectionFixDataLong(section, entry->offset, value);
  } break;
  }
}

static void CalculateAllRelocations(void){
  for(int i = 0; i < VectorSize(loaded_data); i++){
    ObjData* obj_data = VectorGet(loaded_data, i);
    for(int j = 0; j < VectorSize(obj_data->relatabs); j++){
      Relatab* relatab = VectorGet(obj_data->relatabs, j);
      Section* section = VectorGet(obj_data->sections, j);
      for(int k = 0; k < VectorSize(relatab); k++){
        RelatabEntry* entry = VectorGet(relatab, k);
        CalculateRelocation(obj_data, section, entry);
      }
    }
  }
}



void LinkerInit(void){
  loaded_data     = VectorInit();
  import_symbols  = VectorInit();
  export_symbols  = VectorInit();
  sorted_sections = VectorInit();
}

void LinkerFree(void){

  for(int i = 0; i < VectorSize(loaded_data); i++){
    ObjDataDrop(VectorGet(loaded_data, i));
  }
  VectorDrop(loaded_data);

  for(int i = 0; i < VectorSize(import_symbols); i++){
    GlobalDefDrop(VectorGet(import_symbols, i));
  }
  VectorDrop(import_symbols);

  for(int i = 0; i < VectorSize(export_symbols); i++){
    GlobalDefDrop(VectorGet(export_symbols, i));
  }
  VectorDrop(export_symbols);

  for(int i = 0; i < VectorSize(sorted_sections); i++){
    GlobalDefDrop(VectorGet(sorted_sections, i));
  }
  VectorDrop(sorted_sections);
}

void LinkerAssignFile(FILE* input_file){
  ObjData* obj_data = LoadObjData(input_file);
  VectorPush(loaded_data, obj_data);
}

void LinkerLinkData(void){
  CollectAllSymbols();
  CheckExports();
  LinkImports();

  if(link_errors != 0) return;

  SortSections();
  CalculateSectionsOffsets();
  CalculateAbsoluteValues();
  CalculateAllRelocations();
}

#define HEX_LINE_WIDTH 8

void LinkerGenerateOutput(void){
  int curr_addr = 0;

  for(int i = 0; i < VectorSize(sorted_sections); i++){
    GlobalDef* global_def = VectorGet(sorted_sections, i);
    ObjData* obj_data = VectorGet(loaded_data, global_def->file);
    Section* section  = VectorGet(obj_data->sections, global_def->entry);
    SymtabEntry* section_symbol = VectorGet(obj_data->symtab, section->index);

    int padding = (section_symbol->value - curr_addr) % HEX_LINE_WIDTH;
    // pad with empty bytes (if needed for alignment)
    for(int j = 0; j < padding; j++){
      fprintf(ldout, "%02x ", 0x00);
      curr_addr++;
      if(curr_addr % HEX_LINE_WIDTH == 0) fprintf(ldout, "\n");
    }
    curr_addr = section_symbol->value;

    for(int j = 0; j < ByteVectorSize(section->content); j++){
      if(curr_addr % HEX_LINE_WIDTH == 0) fprintf(ldout, "%08x : ", curr_addr);
      fprintf(ldout, "%02x ", ByteVectorGet(section->content, j));
      curr_addr++;
      if(curr_addr % HEX_LINE_WIDTH == 0) fprintf(ldout, "\n");
    }
  }

  int padding = (curr_addr + HEX_LINE_WIDTH - 1) / HEX_LINE_WIDTH * HEX_LINE_WIDTH - curr_addr;

  // finalize with padding
  for(int i = 0; i < padding % HEX_LINE_WIDTH; i++){
    fprintf(ldout, "%02x ", 0x00);
    curr_addr++;
  }

  if(curr_addr >= HEAP_START){
    ReportWarning("Static zone overlaps with heap zone (static zone size: %d, heap zone start: %d)", 
      curr_addr, HEAP_START);
  }
  else if(curr_addr >= STACK_START){
    ReportWarning("Static zone overlaps with stack zone (static zone size: %d, stack zone start: %d)", 
      curr_addr, STACK_START);
  }
  else if(curr_addr >= IO_START){
    ReportWarning("Static zone overlaps with I/O zone (static zone size: %d, I/O zone start: %d)", 
      curr_addr, IO_START);
  }
}

void LinkerPrint(void){
  for(int i = 0; i < VectorSize(loaded_data); i++){
    ObjData* obj_data = VectorGet(loaded_data, i);
    ObjDataDump(obj_data);
  }
}