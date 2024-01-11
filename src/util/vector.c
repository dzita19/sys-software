#include "vector.h"

#include <string.h>
#include <stdlib.h>

Vector* VectorCreateEmpty(void){
  Vector* vector = malloc(sizeof(Vector));
  vector->content = 0;
  vector->size = 0;
  vector->reserved = 0;

  vector_alloc++;

  return vector;
}

void VectorDrop(Vector* vector){
  free(vector->content);
  free(vector);

  vector_free++;
}

Vector* VectorInit(void){
  Vector* vector = VectorCreateEmpty();
  VectorReserve(vector, DEFAULT_VECTOR_SIZE);

  return vector;
}

void VectorReserve(Vector* vector, int reserve){
  if(reserve < vector->size) return;
  void** old = vector->content;

  vector->content = malloc(sizeof(void*) * reserve);
  vector->reserved = reserve;
  if(old) memcpy(vector->content, old, sizeof(void*) * vector->size);

  if(old) free(old);
}

void VectorPush(Vector* vector, void* data){
  if(vector->size == vector->reserved){ // vector->size + 1 == vector->reserved
    VectorReserve(vector, vector->size * 2);
  }

  vector->content[vector->size++] = data;
}

void* VectorPop(Vector* vector){
  if(vector->size == 0) return 0;
  return vector->content[vector->size--];
}

void* VectorGet(Vector* vector, int index){
  if(index < vector->size) return vector->content[index];
  else return 0;
}

void VectorSet(Vector* vector, int index, void* info){
  if(index < vector->size) vector->content[index] = info;
}

int VectorSize(Vector* vector){
  return vector->size;
}

void VectorClear(Vector* vector){
  vector->size = 0;
}