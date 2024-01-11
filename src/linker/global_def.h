#ifndef _GLOBAL_DEF_H_
#define _GLOBLA_DEF_H_

typedef struct GlobalDef{
  char*             name;
  int               file;
  int               entry;
  struct GlobalDef* link;
} GlobalDef;

extern GlobalDef* GlobalDefCreateEmpty(void);
extern void GlobalDefDrop(GlobalDef*);
extern void GlobalDefDump(GlobalDef*);

#endif