#include "queue.h"

#include <stdlib.h>

Queue Queue_Create(uint8_t* buffer, uint32_t capacity) {
  Queue outQueue;
  outQueue.buffer = buffer;
  outQueue.capacity = capacity;
  outQueue.size = 0;
  outQueue.frontOfQueue = 0;
  outQueue.backOfQueue = 0;
  return outQueue;
}

Boolean Queue_Push(Queue* queue, uint8_t value) {
  if (!Queue_Overflowed(queue)) {
    queue->size += 1;
    queue->buffer[queue->backOfQueue] = value;
    queue->backOfQueue = (queue->backOfQueue + 1) % queue->capacity;
  } else {
    return FALSE;
  }
  return TRUE;
}

Boolean Queue_Pop(Queue* queue, uint8_t* value) {
  if (!Queue_Empty(queue)) {
    // value = (uint8_t*)&queue->buffer[queue->frontOfQueue];
    //*value = &queue->buffer[queue->frontOfQueue];
    if (value != NULL) {
      *value = queue->buffer[queue->frontOfQueue];
    }
    queue->frontOfQueue = (queue->frontOfQueue + 1) % queue->capacity;
    queue->size--;
    return TRUE;
  }
  return FALSE;
}

Boolean Queue_Empty(Queue* queue) {
  if (queue->size == 0) {
    return TRUE;
  }
  return FALSE;
}

Boolean Queue_Overflowed(Queue* queue) {
  if (queue->size == queue->capacity) {
    return TRUE;
  }
  return FALSE;
}

// Returns used size of queue
uint32_t Queue_Size(Queue* queue) { return queue->size; }

// Returns max capacity of queue
uint32_t Queue_Capacity(Queue* queue) { return queue->capacity; }
