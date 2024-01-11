#ifndef _STACK_H_
#define _STACK_H_

typedef struct StackNode {
  void* info;
  struct StackNode* next;
} StackNode;

typedef struct{
  StackNode* top;
} Stack; 

extern void   StackPush (Stack* stack, void* info);
extern void*  StackPop  (Stack* stack);
extern void*  StackPeek (Stack* stack);
extern int    StackEmpty(Stack* stack);
extern int    StackSize (Stack* stack);

#endif