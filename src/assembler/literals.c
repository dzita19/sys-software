#include "literals.h"

#include <stdlib.h>
#include <string.h>

#include "machine/architecture.h"
#include "yy/y.tab.h"

Stack symbol_stack = (Stack){ 0 };
Stack reglit_stack = (Stack){ 0 };
Stack numlit_stack = (Stack){ 0 };
Stack strlit_stack = (Stack){ 0 }; // string is not represented in a regular way
  // it's represented as: 4byte for length; content; null terminator
  // length represents length of both content and null terminator


void identifier(const char* str){
  StackPush(&symbol_stack, StringDuplicate(str));
}

void reg(unsigned index){
  if(index < REG_GP_COUNT){
    StackPush(&reglit_stack, (void*)(long)index);
  }
  else StackPush(&reglit_stack, (void*)(long)-1);
}

void rbp(void){
  StackPush(&reglit_stack, (void*)(long)REG_BP);
}

void rsp(void){
  StackPush(&reglit_stack, (void*)(long)REG_SP);
}

void hex_val(const char* str){
  int val = 0;
  str += 2;
  while(*str){
    if     (*str >= '0' && *str <= '9') val = 16 * val + (*str - '0');
    else if(*str >= 'a' && *str <= 'f') val = 16 * val + (*str - 'a') + 10;
    else if(*str >= 'A' && *str <= 'F') val = 16 * val + (*str - 'A') + 10;
    else break;
    str++;
  }
  char flags = 0; // u - 0, l - 1
  while(*str){
    if     (*str == 'u' || *str == 'U') flags |= 1 << 0;
    else if(*str == 'l' || *str == 'L') flags |= 1 << 1;
    str++;
  }

  StackPush(&numlit_stack, (void*)(long)val);
}

void oct_val(const char* str){
  int val = 0;
  str += 1;
  while(*str){
    if (*str >= '0' && *str <= '7') val = 8 * val + (*str - '0');
    else break;
    str++;
  }
  char flags = 0; // u - 0, l - 1
  while(*str){
    if     (*str == 'u' || *str == 'U') flags |= 1 << 0;
    else if(*str == 'l' || *str == 'L') flags |= 1 << 1;
    str++;
  }

  StackPush(&numlit_stack, (void*)(long)val);
}

void dec_val(const char* str){
  int val = 0;
  while(*str){
    if (*str >= '0' && *str <= '9') val = 10 * val + (*str - '0');
    else break;
    str++;
  }
  char flags = 0; // u - 0, l - 1
  while(*str){
    if     (*str == 'u' || *str == 'U') flags |= 1 << 0;
    else if(*str == 'l' || *str == 'L') flags |= 1 << 1;
    str++;
  }

  StackPush(&numlit_stack, (void*)(long)val);
}

void simple_char_lit(const char* str){
  StackPush(&numlit_stack, (void*)(long)str[0]);
}

// \\['"?\\abfnrtv]

void simple_escape_char_lit(const char* str){
  char val;

  switch(str[1]){
  case '\'': val =  '\''; break;
  case '\"': val =  '\"'; break;
  case '\?': val =  '\?'; break;
  case '\\': val =  '\\'; break;
  case 'a' : val =  '\a'; break;
  case 'b' : val =  '\b'; break;
  case 'f' : val =  '\f'; break;
  case 'n' : val =  '\n'; break;
  case 'r' : val =  '\r'; break;
  case 't' : val =  '\t'; break;
  case 'v' : val =  '\v'; break;
  default  : val =  0;
  }

  StackPush(&numlit_stack, (void*)(long)val);
}

void oct_char_lit(const char* str){
  char c = 0;
  int cntr = 0;
  str += 1;
  while(*str){
    if(*str >= '0' && *str <= '7' && cntr++ <= 3) c = c * 8 + (*str - '0');
    else break;

    str++;
  }

  StackPush(&numlit_stack, (void*)(long)c);
}

void hex_char_lit(const char* str){
  char c = 0;
  str += 2;
  while(*str){
    if     (*str >= '0' && *str <= '9') c = 16 * c + (*str - '0');
    else if(*str >= 'a' && *str <= 'f') c = 16 * c + (*str - 'a') + 10;
    else if(*str >= 'A' && *str <= 'F') c = 16 * c + (*str - 'A') + 10;
    else break;

    str++;
  }

  StackPush(&numlit_stack, (void*)(long)c);
}

/*void string_lit(const char* str){
  char* lit = StringAllocate(strlen(str) - 1);
  for(int i = 0; i < strlen(str) - 2; i++){
    lit[i] = str[i + 1];
  }
  lit[strlen(str) - 2] = 0;
  
  StackPush(&strlit_stack, lit);
}*/

//    \"([^\\'"\n]|(\\['"?\\abfnrtv])|(\\{O}{1,3})|(\\x{H}+))*\" 

#define STRING_PARSE_REGULAR   0
#define STRING_PARSE_BACKSLASH 1
#define STRING_PARSE_OCT1      2
#define STRING_PARSE_OCT2      3
#define STRING_PARSE_OCT3      4
#define STRING_PARSE_HEX       5

void string_lit(const char* str){
  char* lit = StringAllocate(strlen(str) - 1 + 4); // quotes(") are not added to the string, first 4byte is for the length
  int lit_cntr = 4; // skip first 4byte (reserved for length)
  int hex_container = 0;
  int oct_container = 0;

  int parse_state = STRING_PARSE_REGULAR;
  for(int i = 1; i < strlen(str) - 1;){
    switch(parse_state){
    case STRING_PARSE_REGULAR:{
      if(str[i] == '\\') {
        parse_state = STRING_PARSE_BACKSLASH;
        i++;
      }
      else {
        lit[lit_cntr++] = str[i];
        i++;
      }
    } break;

    case STRING_PARSE_BACKSLASH:{
      if(str[i] == '\\') {
        lit[lit_cntr++] = '\\';
        parse_state = STRING_PARSE_REGULAR;
        i++;
      }
      else if(str[i] == 'a'){
        lit[lit_cntr++] = '\a';
        parse_state = STRING_PARSE_REGULAR;
        i++;
      }
      else if(str[i] == 'b'){
        lit[lit_cntr++] = '\b';
        parse_state = STRING_PARSE_REGULAR;
        i++;
      }
      else if(str[i] == 'f'){
        lit[lit_cntr++] = '\f';
        parse_state = STRING_PARSE_REGULAR;
        i++;
      }
      else if(str[i] == 'n'){
        lit[lit_cntr++] = '\n';
        parse_state = STRING_PARSE_REGULAR;
        i++;
      }
      else if(str[i] == 'r'){
        lit[lit_cntr++] = '\r';
        parse_state = STRING_PARSE_REGULAR;
        i++;
      }
      else if(str[i] == 't'){
        lit[lit_cntr++] = '\t';
        parse_state = STRING_PARSE_REGULAR;
        i++;
      }
      else if(str[i] == 'v'){
        lit[lit_cntr++] = '\v';
        parse_state = STRING_PARSE_REGULAR;
        i++;
      }
      else if(str[i] >= '0' && str[i] <= '7'){
        parse_state = STRING_PARSE_OCT1;
      }
      else if(str[i] >= 'x'){
        parse_state = STRING_PARSE_HEX;
        i++;
      }
    } break;

    case STRING_PARSE_OCT1:{
      if(str[i] >= '0' && str[i] <= '7'){
        oct_container <<= 3;
        oct_container += str[i] - '0';
        parse_state = STRING_PARSE_OCT2;
        i++;
      }
      else {
        lit[lit_cntr++] = oct_container;
        oct_container = 0;
        parse_state = STRING_PARSE_REGULAR;
      }
    } break;

    case STRING_PARSE_OCT2:{
      if(str[i] >= '0' && str[i] <= '7'){
        oct_container <<= 3;
        oct_container += str[i] - '0';
        parse_state = STRING_PARSE_OCT3;
        i++;
      }
      else {
        lit[lit_cntr++] = oct_container;
        oct_container = 0;
        parse_state = STRING_PARSE_REGULAR;
      }
    } break;

    case STRING_PARSE_OCT3:{
      if(str[i] >= '0' && str[i] <= '7'){
        oct_container <<= 3;
        oct_container += str[i] - '0';
        i++;
        
        lit[lit_cntr++] = oct_container;
        oct_container = 0;
        parse_state = STRING_PARSE_REGULAR;
      }
      else {
        lit[lit_cntr++] = oct_container;
        oct_container = 0;
        parse_state = STRING_PARSE_REGULAR;
      }
    } break;

    case STRING_PARSE_HEX:{
      if(str[i] >= '0' && str[i] <= '9'){
        hex_container <<= 4;
        hex_container += str[i] - '0';
        i++;
      }
      else if(str[i] >= 'a' && str[i] <= 'f'){
        hex_container <<= 4;
        hex_container += str[i] - 'a' + 10;
        i++;
      }
      else if(str[i] >= 'A' && str[i] <= 'F'){
        hex_container <<= 4;
        hex_container += str[i] - 'A' + 10;
        i++;
      }
      else{
        lit[lit_cntr++] = hex_container;
        hex_container = 0;
        parse_state = STRING_PARSE_REGULAR;
      }
    } break;

    }
  }

  // perform cleanup for oct and hex states (they won't be added to string if they are last in the literal)
  // for example "\012" won't be added in the for loop
  if(parse_state == STRING_PARSE_OCT1 || parse_state == STRING_PARSE_OCT2 || parse_state == STRING_PARSE_OCT3){
    lit[lit_cntr++] = oct_container;
    oct_container = 0;
    parse_state = STRING_PARSE_REGULAR;
  }
  else if(parse_state == STRING_PARSE_HEX){
    lit[lit_cntr++] = hex_container;
    hex_container = 0;
    parse_state = STRING_PARSE_REGULAR;
  }

  lit[lit_cntr++] = 0;
  *(int*)&lit[0] = lit_cntr - 4; // save string length

  StackPush(&strlit_stack, lit);
}