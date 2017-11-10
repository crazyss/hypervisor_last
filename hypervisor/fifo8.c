#include "fifo8.h"


void fifo_init(struct fifo *fifo, int size, unsigned char *buf)
{
	fifo->buf = buf;
	fifo->size = size;
	fifo->free = size;
	fifo->flag = 0;
	fifo->head = fifo->tail = 0;
	return;
}
int fifo_put(struct fifo *fifo, unsigned char data)
{
	if (fifo->free > 0) {
		fifo->buf[fifo->tail] = data;
		fifo->tail++;
		fifo->tail%=fifo->size;
		fifo->free--;
		return 0;
	}else {
		fifo->flag |= FIFO_FULL;
		return -1;
	}

}
char fifo_get(struct fifo *fifo)
{
	unsigned char data;
	if (fifo->free == fifo->size) {
		return -1;
	}
	data = fifo->buf[fifo->head];
	fifo->head++;
	fifo->head%=fifo->size;
	fifo->free++;

	return data;
}
int fifo_status(struct fifo  *fifo)
{
	return fifo->size - fifo->free;	
}
