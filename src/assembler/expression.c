#include "expression.h"

#include <stdio.h>
#include <stdlib.h>
#include "util/memory_safety.h"
#include "util/vector.h"
#include "literals.h"
#include "assembler.h"

AsmExpr* AsmExprCreateEmpty(void){
  AsmExpr* expr = malloc(sizeof(AsmExpr));
  expr->plus_symbols  = VectorInit();
  expr->minus_symbols = VectorInit();
  expr->offset        = 0;

  asm_expr_alloc++;

  return expr;
}

void AsmExprDrop(AsmExpr* expr){
  for(int i = 0; i < VectorSize(expr->plus_symbols); i++){
    char* symbol_name = VectorGet(expr->plus_symbols, i);
    if(symbol_name) StringDrop(symbol_name);
  }
  VectorDrop(expr->plus_symbols);

  for(int i = 0; i < VectorSize(expr->minus_symbols); i++){
    char* symbol_name = VectorGet(expr->minus_symbols, i);
    if(symbol_name) StringDrop(symbol_name);
  }
  VectorDrop(expr->minus_symbols);

  free(expr);

  asm_expr_free++;
}

void AsmExprDump(AsmExpr* expr){
  for(int i = 0; i < VectorSize(expr->plus_symbols); i++){
    if(i > 0) printf("+");
    printf("%s", (const char*)VectorGet(expr->plus_symbols, i));
  }
  for(int i = 0; i < VectorSize(expr->minus_symbols); i++){
    printf("-%s", (const char*)VectorGet(expr->minus_symbols, i));
  }
  if(expr->offset > 0) {
    if(VectorSize(expr->plus_symbols) + VectorSize(expr->minus_symbols) > 0) printf("+");
    printf("%d", +expr->offset);
  }
  else if(expr->offset == 0){
    if(VectorSize(expr->plus_symbols) + VectorSize(expr->minus_symbols) == 0) printf("0");
  }
  else if(expr->offset < 0) printf("-%d", -expr->offset);
}

int   list_size        = 0;

void ListElement(void){
  list_size++;
}

void TransferSymbol(Vector* destination, Vector* source, int index_in_source){
  VectorPush(destination, VectorGet(source, index_in_source));
  VectorSet(source, index_in_source, NULL);
}

void AdditiveExpressionAdd(void){
  AsmExpr* operation     = AsmExprCreateEmpty();
  AsmExpr* right_operand = StackPop(&expression_stack);
  AsmExpr* left_operand  = StackPop(&expression_stack);

  // left operand, plus symbols
  for(int i = 0; i < VectorSize(left_operand->plus_symbols); i++){
    TransferSymbol(operation->plus_symbols, left_operand->plus_symbols, i);
  }
  // left operand, minus symbols
  for(int i = 0; i < VectorSize(left_operand->minus_symbols); i++){
    TransferSymbol(operation->minus_symbols, left_operand->minus_symbols, i);
  }
  // left operand, offset
  operation->offset += left_operand->offset;

  // right operand, plus symbols
  for(int i = 0; i < VectorSize(right_operand->plus_symbols); i++){
    TransferSymbol(operation->plus_symbols, right_operand->plus_symbols, i);
  }
  // left operand, minus symbols
  for(int i = 0; i < VectorSize(right_operand->minus_symbols); i++){
    TransferSymbol(operation->minus_symbols, right_operand->minus_symbols, i);
  }
  operation->offset += right_operand->offset;

  AsmExprDrop(left_operand);
  AsmExprDrop(right_operand);
  StackPush(&expression_stack, operation);
}

void AdditiveExpressionSub(void){
  AsmExpr* operation     = AsmExprCreateEmpty();
  AsmExpr* right_operand = StackPop(&expression_stack);
  AsmExpr* left_operand  = StackPop(&expression_stack);

  // left operand, plus symbols
  for(int i = 0; i < VectorSize(left_operand->plus_symbols); i++){
    TransferSymbol(operation->plus_symbols, left_operand->plus_symbols, i);
  }
  // left operand, minus symbols
  for(int i = 0; i < VectorSize(left_operand->minus_symbols); i++){
    TransferSymbol(operation->minus_symbols, left_operand->minus_symbols, i);
  }
  // left operand, offset
  operation->offset += left_operand->offset;

  // right operand, plus symbols
  for(int i = 0; i < VectorSize(right_operand->minus_symbols); i++){
    TransferSymbol(operation->plus_symbols, right_operand->minus_symbols, i);
  }
  // left operand, minus symbols
  for(int i = 0; i < VectorSize(right_operand->plus_symbols); i++){
    TransferSymbol(operation->minus_symbols, right_operand->plus_symbols, i);
  }
  operation->offset -= right_operand->offset;

  AsmExprDrop(left_operand);
  AsmExprDrop(right_operand);
  StackPush(&expression_stack, operation);
}

void UnaryExpressionPlus(void){
  AsmExpr* operation = AsmExprCreateEmpty();
  AsmExpr* operand   = StackPop(&expression_stack);

  // plus symbols
  for(int i = 0; i < VectorSize(operand->plus_symbols); i++){
    TransferSymbol(operation->plus_symbols, operand->plus_symbols, i);
  }
  // minus symbols
  for(int i = 0; i < VectorSize(operand->minus_symbols); i++){
    TransferSymbol(operation->minus_symbols, operand->minus_symbols, i);
  }
  // offset
  operation->offset = +operand->offset;

  AsmExprDrop(operand);
  StackPush(&expression_stack, operation);
}

void UnaryExpressionMinus(void){
  AsmExpr* operation = AsmExprCreateEmpty();
  AsmExpr* operand   = StackPop(&expression_stack);

  // plus symbols
  for(int i = 0; i < VectorSize(operand->minus_symbols); i++){
    TransferSymbol(operation->plus_symbols, operand->minus_symbols, i);
  }
  // minus symbols
  for(int i = 0; i < VectorSize(operand->plus_symbols); i++){
    TransferSymbol(operation->minus_symbols, operand->plus_symbols, i);
  }
  // offset
  operation->offset = -operand->offset;

  AsmExprDrop(operand);
  StackPush(&expression_stack, operation);
}

void PrimaryExpressionSymbol(void){
  AsmExpr* expr = AsmExprCreateEmpty();
  VectorPush(expr->plus_symbols, StackPop(&symbol_stack));
  StackPush(&expression_stack, expr);
}

void PrimaryExpressionConstant(void){
  AsmExpr* expr = AsmExprCreateEmpty();
  expr->offset = (int)(long)StackPop(&numlit_stack);
  StackPush(&expression_stack, expr);
}


AsmExpr* SymbolExpression(const char* symbol_name){
  StackPush(&symbol_stack, (char*)symbol_name);
  PrimaryExpressionSymbol();
  return StackPop(&expression_stack);
}