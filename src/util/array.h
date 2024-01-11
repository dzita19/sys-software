#ifndef _ARRAY_H_
#define _ARRAY_H_

typedef struct Array{
  void* data;
  int elem_size;
  int elem_num;
} Array;

void  ArrayAlloc(Array*, int elem_size, int elem_num);
void  ArrayFree(Array*);
void* ArrayGet(Array*, int index);

#endif