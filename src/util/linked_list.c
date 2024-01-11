#include "linked_list.h"

#include <stdlib.h>
#include "memory_safety.h"

Node* NodeCreateEmpty(){
  Node* node = malloc(sizeof(Node));
  node->info = 0;
  node->next = 0;
  node->prev = 0;

  node_alloc++;

  return node;
}

void  NodeDrop(Node* node){
  node_free++;

  free(node);
}

LinkedList* LinkedListCreateEmpty(){
  LinkedList* list = malloc(sizeof(LinkedList));
  list->first = 0;
  list->last = 0;

  linked_list_alloc++;

  return list;
}

void LinkedListDrop(LinkedList* list){
  LinkedListDelete(list);

  free(list);

  linked_list_free++;
}

void  LinkedListInsertFirst(LinkedList* list, Node* node){
  node->next = list->first;
  node->prev = 0;
  if(list->first) list->first->prev = node;
  else list->last = node;
  list->first = node;
}

void  LinkedListInsertLast(LinkedList* list, Node* node){
  node->prev = list->last;
  node->next = 0;
  if(list->last) list->last->next = node;
  else list->first = node;
  list->last = node;
}

void  LinkedListInsertAfter(LinkedList* list, Node* insert_after, Node* node){
  if(list->last == insert_after) list->last = node;
  node->prev = insert_after;
  node->next = insert_after->next; 
  if(node->prev) node->prev->next = node;
  if(node->next) node->next->prev = node;
}

void  LinkedListInsertBefore(LinkedList* list, Node* insert_before, Node* node){
  if(list->first == insert_before) list->first = node;
  node->prev = insert_before->prev;
  node->next = insert_before;
  if(node->prev) node->prev->next = node;
  if(node->next) node->next->prev = node;
}

Node* LinkedListRemoveFirst(LinkedList* list){
  Node* node = list->first;
  if(list->first == list->last) {
    list->first = 0;
    list->last = 0; 
  }
  else {
    list->first = list->first->next;
    list->first->prev = 0;
  }

  node->prev = 0;
  node->next = 0;
  return node;
}

Node* LinkedListRemoveLast(LinkedList* list){
  Node* node = list->last;
  if(list->first == list->last) {
    list->first = 0;
    list->last = 0;
  }
  else{
    list->last = list->last->prev;
    list->last->next = 0;
  }
  
  node->prev = 0;
  node->next = 0;
  return node;
}

Node* LinkedListRemoveFrom(LinkedList* list, Node* node){
  if(list->first == node){
    list->first = node->next;
  }
  else{
    node->prev->next = node->next;
  }

  if(list->last == node){
    list->last = node->prev;
  }
  else{
    node->next->prev = node->prev;
  }

  node->prev = 0;
  node->next = 0;
  return node;
}

void LinkedListDelete(LinkedList* list){
  while(list->first){
    list->last = list->first;
    list->first = list->first->next;

    NodeDrop(list->last);
  }
  list->first = 0;
  list->last = 0;
}

LinkedList LinkedListDuplicate(LinkedList* list){
  LinkedList duplicate = { 0, 0 };

  for(Node* current_node = list->first; current_node; current_node = current_node->next){
    Node* duplicate_node = NodeCreateEmpty();
    duplicate_node->info = current_node->info;
    LinkedListInsertLast(&duplicate, duplicate_node);
  }

  return duplicate;
}

void LinkedListInsertFirstInfo(LinkedList* list, void* info){
  Node* node = NodeCreateEmpty();
  node->info = info;
  LinkedListInsertFirst(list, node);
}

void LinkedListInsertLastInfo (LinkedList* list, void* info){
  Node* node = NodeCreateEmpty();
  node->info = info;
  LinkedListInsertLast(list, node);
}