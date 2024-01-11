
D			[0-9]
O			[0-7]
L			[a-zA-Z_]
H			[a-fA-F0-9]
E			[Ee][+-]?{D}+
FS			(f|F|l|L)
IS			(u|U|l|L)*

%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "y.tab.h"
#include "assembler/literals.h"

#include "util/memory_safety.h"
#include "obj_data/symtab.h"

void count();
void comment();
void one_liner();

#define YY_NO_UNPUT

int yywrap();
%}

%%
"#"|";" 		  { comment(); return('\n');   }
"\n"					{ count();   return('\n');   }

".end"        { count(); return(_END);     }
".section"    { count(); return(_SECTION); }
".text"       { count(); return(_TEXT);    }
".data"       { count(); return(_DATA);    }
".rodata"     { count(); return(_RODATA);  }
".globl"      { count(); return(_GLOBL);   }
".global"     { count(); return(_GLOBL);   }
".extrn"      { count(); return(_EXTRN);   }
".extern"     { count(); return(_EXTRN);   }
".def"        { count(); return(_DEF);     }
".byte"       { count(); return(_BYTE);    }
".word"       { count(); return(_WORD);    }
".long"       { count(); return(_LONG);    }
".zero"       { count(); return(_ZERO);    }
".skip"       { count(); return(_SKIP);    }
".asciz"      { count(); return(_ASCIZ);   }
".asciiz"     { count(); return(_ASCIZ);   }
"@notyp"      { count(); return(_NOTYP);   }
"@funct"      { count(); return(_FUNCT);   }
"@objct"      { count(); return(_OBJCT);   }

"%reg"[0-7]   { count(); reg(yytext[4] - '0');  return(REGISTER); }
"%rbp"        { count(); rbp();           return(REGISTER); }
"%rsp"        { count(); rsp();           return(REGISTER); }

".L"({L}|{D})+          { count(); identifier(yytext);	  return(SYMBOL); }

{L}({L}|{D})*						{ count(); identifier(yytext); 		return(SYMBOL);   }

0[xX]{H}+{IS}?					{ count(); hex_val(yytext);       return(CONSTANT); }

0{O}+{IS}?							{ count(); oct_val(yytext);       return(CONSTANT); }

{D}+{IS}?								{ count(); dec_val(yytext);       return(CONSTANT); }

'[^\\']'								{ count(); simple_char_lit(yytext + 1);         return(CONSTANT); }

'\\['"?\\abfnrtv]'			{ count(); simple_escape_char_lit(yytext + 1);  return(CONSTANT); }

'\\{O}{1,3}'						{ count(); oct_char_lit(yytext + 1);            return(CONSTANT); }

'\\x{H}+'								{ count(); hex_char_lit(yytext + 1);            return(CONSTANT); }

\"([^\\"\n]|(\\['"?\\abfnrtv])|(\\{O}{1,3})|(\\x{H}+))*\" { count(); string_lit(yytext); return(STRING_LITERAL); }

","						{ count(); return(','); }
":"           { count(); return(':'); }
"="						{ count(); return('='); }
"("						{ count(); return('('); }
")"						{ count(); return(')'); }
"["						{ count(); return('['); }
"]"						{ count(); return(']'); }
"-"						{ count(); return('-'); }
"+"						{ count(); return('+'); }
"$"						{ count(); return('$'); }
"*"						{ count(); return('*'); }

[ \t\v\f]		  { count(); }
.			{ count(); return('#'); /* ignore bad characters */ }

%%

int yywrap()
{
  return(1);
}


int column = 1;
int row = 1;

const int tab_width = 2;

void comment(){
  char c;
  int extend;

one_liner:
  extend = 0; // extend comment to the next line
  while((c = input()) != '\n' && c != '\0'){
    if(c == '\\') extend = 1;
    else extend = 0;

    if (c == '\t') column += tab_width - (column % tab_width);
    else column++;
  }

  // column = 1;
  // row++;

  if(extend) goto one_liner;
}

void count()
{
  int i;

  for (i = 0; yytext[i] != '\0'; i++)
    if (yytext[i] == '\n'){
      // column = 1;
      // row++;
    }
    else if (yytext[i] == '\t')
      column += tab_width - (column % tab_width);
    else
      column++;

}