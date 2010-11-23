#ifndef _RINGBUF_H_
#define _RINGBUF_H_

#include <stdint.h>

#ifndef RINGBUF_SIZE
#define RINGBUF_SIZE (255)
#endif

struct ringbuf {
	uint8_t data[RINGBUF_SIZE];
	uint8_t size;
	uint8_t *head;
	uint8_t *tail;
};

static inline void rb_init(struct ringbuf *rb)
{
	rb->size = 0;
	rb->head = &rb->data[0];
	rb->tail = &rb->data[0];
}

static inline uint8_t rb_is_full(struct ringbuf *rb)
{
	return rb->size == RINGBUF_SIZE;
}

static inline uint8_t rb_is_empty(struct ringbuf *rb)
{
	return rb->size == 0;
}

static inline void __rb_insert_tail(struct ringbuf *rb, uint8_t data)
{
	*rb->tail = data;
	rb->tail++;
	if (rb->tail == &rb->data[RINGBUF_SIZE])
		rb->tail = rb->data;
}

static inline void rb_insert_tail(struct ringbuf *rb, uint8_t data)
{
	__rb_insert_tail(rb, data);	
	rb->size++;
}

static inline uint8_t __rb_remove_head(struct ringbuf *rb)
{
	uint8_t data = *rb->head;
	rb->head++;
	if (rb->head == &rb->data[RINGBUF_SIZE])
		rb->head = rb->data;
	return data;
}

static inline uint8_t rb_remove_head(struct ringbuf *rb)
{
	rb->size--;
	return __rb_remove_head(rb);
}

#endif
