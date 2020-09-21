#include "queue.h"


struct Queue {
	uint8_t* buffer;
	uint32_t size;
	uint32_t capacity;

	uint32_t frontOfQueue;
	uint32_t backOfQueue;

};


Queue Queue_Create(uint8_t* buffer, uint32_t capacity)
{
	//TODO Implement
	return Queue();
}

Boolean Queue_Push(Queue* queue, uint8_t value)
{
	//TODO Implement
	return FALSE;
}

Boolean Queue_Pop(Queue* queue, uint8_t* value)
{
	//TODO Implement
	return FALSE;
}


Boolean Queue_Empty(Queue* queue)
{
	//TODO Implement
	return FALSE;
}

Boolean Queue_Overflowed(Queue* queue)
{
	//TODO Implement
	return FALSE;
}

// Returns used size of queue
uint32_t Queue_Size(Queue* queue)
{
	//TODO Implement
	return FALSE;
}

// Returns max capacity of queue
uint32_t Queue_Capacity(Queue* queue)
{
	//TODO Implement
	return 0;
}

