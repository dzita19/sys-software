#include "section.h"

#include <stdio.h>
#include <stdlib.h>

Section* SectionCreateEmpty(void){
  Section* section    = malloc(sizeof(Section));
  section->index      = 0;
  section->counter    = 0;
  section->content    = ByteVectorInit();

  section_alloc++;

  return section;
}

void SectionDrop(Section* section){
  ByteVectorDrop(section->content);

  section_free++;
}

void SectionDump(Section* section){
  for(int j = 0; j < ByteVectorSize(section->content); j++){
    if(j % 8 == 0) printf("\n  %04x : ", j);
    printf("%02x ", ByteVectorGet(section->content, j));
  }
}

void SectionPutDataByte(Section* section, unsigned char data){
  ByteVectorPush(section->content, data);
}

void SectionPutDataWord(Section* section, unsigned short data){
  ByteVectorPush(section->content, data >> 0);
  ByteVectorPush(section->content, data >> 8);
}

void SectionPutDataLong(Section* section, unsigned int data){
  ByteVectorPush(section->content, data >>  0);
  ByteVectorPush(section->content, data >>  8);
  ByteVectorPush(section->content, data >> 16);
  ByteVectorPush(section->content, data >> 24);
}



void SectionPutCodeByte(Section* section, unsigned char data){
  ByteVectorPush(section->content, data);
}

void SectionPutCodeWord(Section* section, unsigned short data){
  ByteVectorPush(section->content, data >> 0);
  ByteVectorPush(section->content, data >> 8);
}

void SectionPutCodeLong(Section* section, unsigned int data){
  ByteVectorPush(section->content, data >>  0);
  ByteVectorPush(section->content, data >>  8);
  ByteVectorPush(section->content, data >> 16);
  ByteVectorPush(section->content, data >> 24);
}



void SectionFixDataByte(Section* section, int index, unsigned char  data){
  ByteVectorSet(section->content, index, data);
}

void SectionFixDataWord(Section* section, int index, unsigned short data){
  ByteVectorSet(section->content, index + 0, data >> 0);
  ByteVectorSet(section->content, index + 1, data >> 8);
}

void SectionFixDataLong(Section* section, int index, unsigned int   data){
  ByteVectorSet(section->content, index + 0, data >>  0);
  ByteVectorSet(section->content, index + 1, data >>  8);
  ByteVectorSet(section->content, index + 2, data >> 16);
  ByteVectorSet(section->content, index + 3, data >> 24);
}



void SectionFixCodeByte(Section* section, int index, unsigned char  data){
  ByteVectorSet(section->content, index, data);
}

void SectionFixCodeWord(Section* section, int index, unsigned short data){
  ByteVectorSet(section->content, index + 0, data >> 0);
  ByteVectorSet(section->content, index + 1, data >> 8);
}

void SectionFixCodeLong(Section* section, int index, unsigned int   data){
  ByteVectorSet(section->content, index + 0, data >>  0);
  ByteVectorSet(section->content, index + 1, data >>  8);
  ByteVectorSet(section->content, index + 2, data >> 16);
  ByteVectorSet(section->content, index + 3, data >> 24);
}



int SectionGetCounter(Section* section){
  return section->counter;
}

void SectionIncCounter(Section* section, int size){
  section->counter += size;
}