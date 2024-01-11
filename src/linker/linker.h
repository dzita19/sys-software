#ifndef _LINKER_H_
#define _LINKER_H_

#include <stdio.h>

extern int link_errors;

#define STATIC_START 0x00000000
#define STATIC_END   HEAP_START

#define HEAP_START   0x00040000
#define HEAP_END     STACK_START

#define STACK_START  0x00080000
#define STACK_END    IO_START

#define IO_START 0x000F0000
#define IO_END   0x00100000

void LinkerInit(void);
void LinkerFree(void);

void LinkerAssignFile(FILE*);
void LinkerLinkData(void);
void LinkerGenerateOutput(void);
void LinkerPrint(void);

#endif