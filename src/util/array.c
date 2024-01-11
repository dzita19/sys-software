#include "array.h"

#include <stdlib.h>
#include "memory_safety.h"

void ArrayAlloc(Array* array, int elem_size, int elem_num){
  array->data      = malloc(elem_size * elem_num);
  array->elem_size = elem_size;
  array->elem_num  = elem_num;

  array_alloc++;
}

void ArrayFree(Array* array){
  free(array->data);
  array->data      = 0;
  array->elem_size = 0;
  array->elem_num  = 0;

  array_free++;
}

void* ArrayGet(Array* array, int index){
  return array->data + array->elem_size * index;
}