#ifndef QUEUE_H
#define QUEUE_H

#define QUEUE_N 2048

extern int queue_size;
extern int queue[QUEUE_N];

void enqueue(int x);
int check_and_dequeue(void);

#endif // QUEUE_H
