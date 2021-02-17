#ifndef QUEUE_H_
#define QUEUE_H_

#include "types.h"

typedef struct Queue Queue;

/// Очередь
struct Queue {
  uint8_t* buffer;    /// Буфер для хранения байтов
  uint32_t size;      /// Используемый размер очереди
  uint32_t capacity;  /// Максимальный размер очереди

  uint32_t frontOfQueue;  /// Позиция начала очереди
  uint32_t backOfQueue;   /// Позиция конца очереди
};

/**
 * Создает пустую очередь
 * @param buffer указатель на буфер, используемый для хранения значений
 * @param capacity максимально допустимый размер очереди
 * @return Заполненую структуру Queue
 */
Queue Queue_Create(uint8_t* buffer, uint32_t capacity);

/**
 * Помещает байт в очередь
 * @param queue дескриптор Queue
 * @param value байт, который необходимо поместить
 * @return TRUE - если байт помещен; FALSE - иначе
 */
Boolean Queue_Push(Queue* queue, uint8_t value);

/**
 * Извлекает байт из очереди
 * @param queue дескриптор Queue
 * @param value указатель, куда будет записан байт из очереди
 * @return TRUE - если байт извлечен; FALSE - иначе
 */
Boolean Queue_Pop(Queue* queue, uint8_t* value);

/**
 * @brief Queue_Empty проверяет пуста ли очередь
 * @param queue дескриптор Queue
 * @return TRUE - если очередь пуста; FALSE - иначе
 */
Boolean Queue_Empty(Queue* queue);

/**
 * @brief Queue_Overflowed проверяет переполнена ли очередь
 * @param queue дескриптор Queue
 * @return TRUE - если очередь переполнена; FALSE - иначе
 */
Boolean Queue_Overflowed(Queue* queue);

/**
 * @brief Queue_Size возвращает занимаемый размер очередью
 * @param queue дескриптор Queue
 * @return занимаемый размер очередью
 */
uint32_t Queue_Size(Queue* queue);

/**
 * @brief Queue_Capacity возвращает максимальную вместимость очереди
 * @param queue дескриптор Queue
 * @return вместимость очереди
 */
uint32_t Queue_Capacity(Queue* queue);

#endif /* QUEUE_H_ */
