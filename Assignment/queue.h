/*
 * Part of: Operating Systems Assignment
 * Description: Header file for queue.c
 * Author: Kalana Tharusha
 * Date: 01/05/2023
*/
#ifndef QUEUE_H
#define QUEUE_H

#include <time.h>
#include "customStructs.h"

//Struct for node
struct Node {
    cstmr data;
    struct Node *next;
};

//Struct for queue
struct Queue {
    struct Node *front;
    struct Node *rear;
    int MAX_LENGTH;
    int CURR_LENGTH;
};

void initQueue(struct Queue *q, int length);

int isQueueEmpty(struct Queue *q);

int isQueueFull(struct Queue *q);

void enqueue(struct Queue *q, cstmr data);

cstmr dequeue(struct Queue *q);

#endif
