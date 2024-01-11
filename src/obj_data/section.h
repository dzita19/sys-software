#ifndef _SECTION_H_
#define _SECTION_H_

#include "util/byte_vector.h"

typedef struct Section{
  int index; // in symtab
  int counter; // location_counter
  ByteVector* content;
} Section;

extern Section* SectionCreateEmpty(void);
extern void SectionDrop(Section*);
extern void SectionDump(Section*);

extern void SectionPutDataByte(Section*, unsigned char  data);
extern void SectionPutDataWord(Section*, unsigned short data);
extern void SectionPutDataLong(Section*, unsigned int   data);

extern void SectionPutCodeByte(Section*, unsigned char  data);
extern void SectionPutCodeWord(Section*, unsigned short data);
extern void SectionPutCodeLong(Section*, unsigned int   data);

extern void SectionFixDataByte(Section*, int index, unsigned char  data);
extern void SectionFixDataWord(Section*, int index, unsigned short data);
extern void SectionFixDataLong(Section*, int index, unsigned int   data);

extern void SectionFixCodeByte(Section*, int index, unsigned char  data);
extern void SectionFixCodeWord(Section*, int index, unsigned short data);
extern void SectionFixCodeLong(Section*, int index, unsigned int   data);

extern int  SectionGetCounter(Section*);
extern void SectionIncCounter(Section*, int);

#endif