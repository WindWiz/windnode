#include <stdlib.h>	
#include <stdint.h>
#include "ringbuf.h"
#include "errno.h"

int rb_init(struct ringbuf *rb, uint8_t capacity)
{
	uint8_t *data = (uint8_t *) malloc(capacity);

	if (!data)
		return -ENOMEM;
	
	rb->size = 0;
	rb->data = data;
	rb->capacity = capacity;
	rb->head = &rb->data[0];
	rb->tail = &rb->data[0];

	return 0;
}

void rb_free(struct ringbuf *rb)
{
	free(rb->data);
}

uint8_t rb_is_full(struct ringbuf *rb)
{
	return rb->size == rb->capacity;
}

uint8_t rb_is_empty(struct ringbuf *rb)
{
	return rb->size == 0;
}

static inline void __rb_insert_tail(struct ringbuf *rb, uint8_t data)
{
	*rb->tail = data;
	rb->tail++;
	if (rb->tail == &rb->data[rb->capacity])
		rb->tail = rb->data;
}

void rb_insert_tail(struct ringbuf *rb, uint8_t data)
{
	__rb_insert_tail(rb, data);	
	rb->size++;
}

static inline uint8_t __rb_remove_head(struct ringbuf *rb)
{
	uint8_t data = *rb->head;
	rb->head++;
	if (rb->head == &rb->data[rb->capacity])
		rb->head = rb->data;
	return data;
}

uint8_t rb_remove_head(struct ringbuf *rb)
{
	rb->size--;
	return __rb_remove_head(rb);
}
