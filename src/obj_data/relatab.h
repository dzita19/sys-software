#ifndef _RELATAB_H_
#define _RELATAB_H_

#include "util/vector.h"

typedef enum RelaType{
  RELA_CODE_32,
  RELA_DATA_8,
  RELA_DATA_16,
  RELA_DATA_32,
} RelaType;

typedef struct RelatabEntry{
  int      offset;
  RelaType type;
  int      symbol;
  int      addend;
} RelatabEntry;

extern RelatabEntry* RelatabEntryCreateEmpty(void);
extern void RelatabEntryDrop(RelatabEntry*);
extern void RelatabEntryDump(RelatabEntry*);

typedef Vector Relatab;

extern Relatab* RelatabCreateEmpty(void);
extern void RelatabDrop(Relatab*);
extern void RelatabDump(Relatab*);

extern void RelatabInsertEntry(Relatab*, RelatabEntry*);
extern RelatabEntry* RelatabGetEntry(Relatab*, int index);

#endif