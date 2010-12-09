#ifndef _RINGBUF_H_
#define _RINGBUF_H_

#include <stdint.h>

struct ringbuf {
	uint8_t *data;
	uint8_t capacity;
	uint8_t size;
	uint8_t *head;
	uint8_t *tail;
};

int rb_init(struct ringbuf *rb, uint8_t capacity);
void rb_free(struct ringbuf *rb);
uint8_t rb_is_full(struct ringbuf *rb);
uint8_t rb_is_empty(struct ringbuf *rb);
void rb_insert_tail(struct ringbuf *rb, uint8_t data);
uint8_t rb_remove_head(struct ringbuf *rb);

#endif
