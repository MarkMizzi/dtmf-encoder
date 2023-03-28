#pragma once

#define QUEUE_N 4096

extern int queue_size;
extern int queue[QUEUE_N];

void enqueue(int x);
int check_and_dequeue(void);