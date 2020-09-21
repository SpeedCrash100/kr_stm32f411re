#ifndef QUEUE_H_
#define QUEUE_H_

#include "types.h"

typedef struct Queue Queue;

/**
 * Creates empty static byte queue.
 * \param buffer The buffer is used for storing bytes
 * \param capacity The max usage of buffer
 * \return filled Queue structure
 */
Queue Queue_Create(uint8_t* buffer, uint32_t capacity);

/**
 * Pushes byte to queue
 * \param queue The byte is tried to insert in this queue
 * \param value The byte value
 * \return TRUE if byte has been inserted, FALSE otherwise
 */
Boolean Queue_Push(Queue* queue, uint8_t value);

/**
 * Pops byte from queue
 * \param queue The byte is tried to pop from this queue
 * \param value The pointer to place where value should be stored
 * \return TRUE if byte has been popped, FALSE otherwise
 */
Boolean Queue_Pop(Queue* queue, uint8_t* value);


// Checks if queue is empty
Boolean Queue_Empty(Queue* queue);

// Checks if queue has no space for storing new values
Boolean Queue_Overflowed(Queue* queue);

// Returns used size of queue
uint32_t Queue_Size(Queue* queue);

// Returns max capacity of queue
uint32_t Queue_Capacity(Queue* queue);





#endif /* QUEUE_H_ */
