
#define FIFO_FULL 1

struct fifo{
	unsigned char *buf;
	unsigned char head;
	unsigned char tail;
	unsigned char len;
	unsigned char free;
	unsigned char flag;
	unsigned char size;
};

void fifo_init(struct fifo *fifo, int size, unsigned char *buf);
int fifo_put(struct fifo *fifo, unsigned char data);
char fifo_get(struct fifo *fifo);
int fifo_status(struct fifo  *fifo);
