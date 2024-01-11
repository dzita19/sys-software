#ifndef _SYMTAB_H_
#define _SYMTAB_H_

#include "util/vector.h"

typedef enum SymType{
  SYM_NO_TYPE,
  SYM_SECTION,
  SYM_FUNCTION,
  SYM_OBJECT,
} SymType;

typedef enum SymBind{
  SYM_BIND_LOCAL,
  SYM_BIND_GLOBAL,
} SymBind;

#define DEFAULT_SYM_SIZE  0
#define DEFAULT_SYM_ALIGN 0
#define DEFAULT_SYM_TYPE  SYM_NO_TYPE

#define SECTION_UND   0
#define SECTION_ABS   1
#define SECTION_DEF (-1)

typedef struct SymtabEntry{
  int     index;
  int     value;
  int     size;
  int     align;
  SymType type;
  SymBind bind;
  int     section;
  char*   name;
} SymtabEntry;

extern SymtabEntry* SymtabEntryCreateEmpty();
extern void SymtabEntryDrop(SymtabEntry*);
extern void SymtabEntryDump(SymtabEntry*);

typedef Vector Symtab;

extern Symtab* SymtabCreateEmpty();
extern void SymtabDrop(Symtab*);
extern void SymtabDump(Symtab*);

extern SymtabEntry* SymtabInsertEntry(Symtab*, SymtabEntry*);
extern SymtabEntry* SymtabGetEntry(Symtab*, int index);
extern SymtabEntry* SymtabFindEntry(Symtab*, const char* symbol_name);

#endif