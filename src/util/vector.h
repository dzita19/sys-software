#ifndef _VECTOR_H_
#define _VECTOR_H_

#include "memory_safety.h"

#define DEFAULT_VECTOR_SIZE 4

typedef struct Vector{
  void** content;
  int size;
  int reserved;
} Vector;

Vector* VectorCreateEmpty(void);
void    VectorDrop(Vector*);

Vector* VectorInit(void);
void    VectorReserve(Vector*, int);
void    VectorPush(Vector*, void*);
void*   VectorPop(Vector*);
void*   VectorGet(Vector*, int);
void    VectorSet(Vector*, int, void*);
int     VectorSize(Vector*);
void    VectorClear(Vector*);

#endif