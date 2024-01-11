#include "stack.h"

#include <stdlib.h>

void StackPush(Stack* stack, void* info){
  StackNode* node = malloc(sizeof(StackNode));
  node->info = info;
  node->next = stack->top;
  stack->top = node;
}

void* StackPop(Stack* stack){
  StackNode* old = stack->top;
  void* info = stack->top->info;

  if(stack->top == 0) return 0;
  stack->top = stack->top->next;
  
  free(old);
  return info;
}

void* StackPeek(Stack* stack){
  return stack->top->info;
}

int StackEmpty(Stack* stack){
  return stack->top == 0;
}

int StackSize (Stack* stack){
  StackNode* current = stack->top;
  int size = 0;
  while(current){
    size++;
    current = current->next;
  }

  return size;
}