#ifndef _LITERALS_H_
#define _LITERALS_H_

#include "util/stack.h"
#include "util/memory_safety.h"

extern Stack symbol_stack;
extern Stack reglit_stack;
extern Stack numlit_stack;
extern Stack strlit_stack;

void reg(unsigned index);
void rbp(void);
void rsp(void);

void identifier(const char* str);

void hex_val(const char* str);
void oct_val(const char* str);
void dec_val(const char* str);

void simple_char_lit(const char* str);
void simple_escape_char_lit(const char* str);
void oct_char_lit(const char* str);
void hex_char_lit(const char* str);

void string_lit(const char*);

#endif