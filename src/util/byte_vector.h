#ifndef _BYTE_VECTOR_H_
#define _BYTE_VECTOR_H_

#include "memory_safety.h"

#define DEFAULT_BYTE_VECTOR_SIZE 16

typedef unsigned char Byte;

typedef struct ByteVector{
  Byte* content;
  int size;
  int reserved;
} ByteVector;

ByteVector* ByteVectorCreateEmpty(void);
void        ByteVectorDrop(ByteVector*);

ByteVector* ByteVectorInit(void);
void        ByteVectorReserve(ByteVector*, int);
void        ByteVectorPush(ByteVector*, Byte);
Byte        ByteVectorPop(ByteVector*);
Byte        ByteVectorGet(ByteVector*, int);
void        ByteVectorSet(ByteVector*, int, Byte);
int         ByteVectorSize(ByteVector*);

#endif