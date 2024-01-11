#include "byte_vector.h"

#include <string.h>
#include <stdlib.h>

ByteVector* ByteVectorCreateEmpty(void){
  ByteVector* byte_vector = malloc(sizeof(ByteVector));
  byte_vector->content = 0;
  byte_vector->size = 0;
  byte_vector->reserved = 0;

  byte_vector_alloc++;

  return byte_vector;
}

void ByteVectorDrop(ByteVector* byte_vector){
  free(byte_vector->content);
  free(byte_vector);

  byte_vector_free++;
}

ByteVector* ByteVectorInit(void){
  ByteVector* byte_vector = ByteVectorCreateEmpty();
  ByteVectorReserve(byte_vector, DEFAULT_BYTE_VECTOR_SIZE);

  return byte_vector;
}

void ByteVectorReserve(ByteVector* byte_vector, int reserve){
  if(reserve < byte_vector->size) return;
  Byte* old = byte_vector->content;

  byte_vector->content = malloc(sizeof(Byte) * reserve);
  byte_vector->reserved = reserve;
  if(old) memcpy(byte_vector->content, old, sizeof(Byte) * byte_vector->size);

  if(old) free(old);
}

void ByteVectorPush(ByteVector* byte_vector, Byte data){
  if(byte_vector->size == byte_vector->reserved){
    ByteVectorReserve(byte_vector, byte_vector->size * 2);
  }

  byte_vector->content[byte_vector->size++] = data;
}

Byte ByteVectorPop(ByteVector* byte_vector){
  if(byte_vector->size == 0) return 0;
  return byte_vector->content[byte_vector->size--];
}

Byte ByteVectorGet(ByteVector* byte_vector, int index){
  if(index < byte_vector->size) return byte_vector->content[index];
  else return 0;
}

void ByteVectorSet(ByteVector* byte_vector, int index, Byte info){
  if(index < byte_vector->size) byte_vector->content[index] = info;
}

int ByteVectorSize(ByteVector* byte_vector){
  return byte_vector->size;
}