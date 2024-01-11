#ifndef _EXPRESSION_H_
#define _EXPRESSION_H_

#include "util/vector.h"
#include "util/stack.h"

typedef struct AsmExpr{
  Vector* plus_symbols;  // LinkedList(char*)
  Vector* minus_symbols; // LinkedList(char*)
  int offset;
} AsmExpr;

extern AsmExpr* AsmExprCreateEmpty(void);
extern void AsmExprDrop(AsmExpr*);
extern void AsmExprDump(AsmExpr*);

extern int   list_size;

extern void ListElement(void);

extern void AdditiveExpressionAdd(void);
extern void AdditiveExpressionSub(void);
extern void UnaryExpressionPlus(void);
extern void UnaryExpressionMinus(void);
extern void PrimaryExpressionSymbol(void);
extern void PrimaryExpressionConstant(void);

extern AsmExpr* SymbolExpression(const char* symbol_name);

#endif