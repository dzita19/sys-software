#include "queue.h"

#include <stdlib.h>

void  QueueInsert(Queue* queue, void* info){
  QueueNode* node = malloc(sizeof(QueueNode));
  node->info = info;
  node->next = 0;

  if(queue->back)
    queue->back->next = node;
  else
    queue->front = node;
  queue->back = node;
}

void* QueueDelete(Queue* queue){
  QueueNode* old = queue->front;
  void* info = queue->front->info;

  if(!queue->front) return 0;

  if(queue->front == queue->back){
    queue->front = 0;
    queue->back = 0;
  }
  else queue->front = queue->front->next;

  free(old);
  return info;
}

int QueueEmpty(Queue* queue){
  return queue->front == 0;
}