#ifndef _OBJ_DATA_H_
#define _OBJ_DATA_H_

#include "symtab.h"
#include "section.h"
#include "relatab.h"

typedef struct ObjData{
  Symtab* symtab;
  Vector* sections;
  Vector* relatabs;
} ObjData;

extern ObjData* ObjDataCreateEmpty(void);
extern void ObjDataDrop(ObjData*);
extern void ObjDataDump(ObjData*);

#endif