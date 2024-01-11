%{
	#include <stdio.h>

  #include "assembler/expression.h"
  #include "assembler/instruction.h"
  #include "assembler/directive.h"
  #include "assembler/assembler.h"
  #include "assembler/literals.h"

	extern int yylex (void);
	void yyerror(char* s);
%}

%token _SECTION _END _TEXT _DATA _RODATA _GLOBL _EXTRN _DEF _BYTE _WORD _LONG _ZERO _SKIP _ASCIZ _NOTYP _FUNCT _OBJCT
%token REGISTER SYMBOL CONSTANT STRING_LITERAL

%start assembly_unit
%%

assembly_unit
  : assembly_unit '\n' assembly_line { Line(); }
  | assembly_line                    { Line(); }
  ;

assembly_line
  : label ':' directive_line
  | directive_line
  | label ':' instruction_line
  | instruction_line
  | label ':'
  |
  ;

label
  : SYMBOL { Label(); }
  ;

directive_line
  : _END                            { End(); YYACCEPT; }
  | _SECTION symbol                 { SectionSymbol(); }
  | _TEXT                           { SectionText();   }
  | _DATA                           { SectionData();   }
  | _RODATA                         { SectionRodata(); }
  | _GLOBL   symbol_list            { Global();        }
  | _EXTRN   symbol_list            { Extern();        }
  | _DEF     SYMBOL '=' expression  { Define();        }
  | _BYTE    expression_list        { ByteData();      }
  | _WORD    expression_list        { WordData();      }
  | _LONG    expression_list        { LongData();      }
  | _ZERO    expression             { ZeroData();      }
  | _SKIP    expression             { SkipData();      }
  | _ASCIZ   STRING_LITERAL         { Asciiz();        }
  | _FUNCT   symbol_list            { Function();      }
  | _OBJCT   symbol_list            { Object();        }
  | _NOTYP   symbol_list            { NoType();        }
  ;

symbol
  : SYMBOL
  | _TEXT     { SymbolText();   }
  | _DATA     { SymbolData();   }
  | _RODATA   { SymbolRodata(); }
  ;

symbol_list
  : symbol ',' symbol_list          { ListElement(); }
  | symbol                          { ListElement(); }
  ;

expression_list
  : expression ',' expression_list  { ListElement(); }
  | expression                      { ListElement(); }
  ;

expression
  : additive_expression
  ;

additive_expression
  : additive_expression '+' unary_expression  { AdditiveExpressionAdd(); }
  | additive_expression '-' unary_expression  { AdditiveExpressionSub(); }
  | unary_expression
  ;

unary_expression
  : '+' unary_expression                      { UnaryExpressionPlus(); }
  | '-' unary_expression                      { UnaryExpressionMinus(); }
  | primary_expression
  ;

primary_expression
  : symbol                                    { PrimaryExpressionSymbol(); }
  | CONSTANT                                  { PrimaryExpressionConstant(); }
  | '(' additive_expression ')'
  ;

instruction_line
  : instruction                       { SymbolToInstr(); NullaryInstr();        }
  | instruction regdir                { SymbolToInstr(); UnaryRegInstr();       }
  | instruction operand               { SymbolToInstr(); UnaryOpInstr();        }
  | instruction regdir  ',' regdir    { SymbolToInstr(); BinaryRegToRegInstr(); }
  | instruction operand ',' regdir    { SymbolToInstr(); BinaryOpToRegInstr();  }
  | instruction regdir  ',' operand   { SymbolToInstr(); BinaryRegToOpInstr();  }
  ;

instruction
  : SYMBOL  { /*SymbolToInstr();*/ }
  ;

regdir
  : REGISTER                          { OperandRegdir();               }
  ;

operand
  :         REGISTER '+' CONSTANT     { OperandRegdirConstAdd();       }
  |         REGISTER '-' CONSTANT     { OperandRegdirConstSub();       }
  |         CONSTANT                  { OperandImmedConstPlus();       }
  |     '+' CONSTANT                  { OperandImmedConstPlus();       }
  |     '-' CONSTANT                  { OperandImmedConstMinus();      }
  |         symbol                    { OperandImmedSymbol();          }
  |         symbol   '+' CONSTANT     { OperandImmedSymbolConstAdd();  }
  |         symbol   '-' CONSTANT     { OperandImmedSymbolConstSub();  }
  | '['     REGISTER              ']' { OperandRegind();               }
  | '['     REGISTER '+' CONSTANT ']' { OperandRegindConstAdd();       }
  | '['     REGISTER '-' CONSTANT ']' { OperandRegindConstSub();       }
  | '['     CONSTANT              ']' { OperandMemdirConstPlus();      }
  | '[' '+' CONSTANT              ']' { OperandMemdirConstPlus();      }
  | '[' '-' CONSTANT              ']' { OperandMemdirConstMinus();     }
  | '['     symbol                ']' { OperandMemdirSymbol();         }
  | '['     symbol   '+' CONSTANT ']' { OperandMemdirSymbolConstAdd(); }
  | '['     symbol   '-' CONSTANT ']' { OperandMemdirSymbolConstSub(); }
  ;

%%
#include <stdio.h>

extern char yytext[];
extern int row;
extern int column;

extern FILE* yyin;

void yyerror(char* s) {
  printf("Unrecoverable syntax error: (%d,%d)\n", row, column);
	// fflush(stdout);
	// printf("\n%*s\n%*s\n", column, "^", column, s);
}