/*
 * Part of: Operating Systems Assignment
 * Description: This file contains the implementation of the FIFO queue data structure
 * Author: Kalana Tharusha
 * Date: 01/05/2023
*/

#include <stdio.h>
#include <stdlib.h>
#include "queue.h"
#include "customStructs.h"

//Initialise queue
void initQueue(struct Queue *q, int length) {
    q->front = NULL; //Set front to NULL
    q->rear = NULL; //Set rear to NULL
    q->MAX_LENGTH = length; //Set max length
    q->CURR_LENGTH = 0; //Set current length to 0
}

//Check if queue is empty
int isQueueEmpty(struct Queue *q) {
    return (q->front == NULL); //If front is NULL, queue is empty
}

//Check if queue is full
int isQueueFull(struct Queue *q) {
    return q->CURR_LENGTH == q->MAX_LENGTH; //If current length is equal to max length, queue is full
}

//Add data to queue
void enqueue(struct Queue *q, cstmr data) {

    // If queue is full, return.
    if (q->CURR_LENGTH == q->MAX_LENGTH) {
        printf("Error: Queue is full\n");
        return;
    }

    struct Node *newNode = (struct Node *) malloc(sizeof(struct Node)); //Create new node
    newNode->data = data; //Set data to new node
    newNode->next = NULL; //Set next node to NULL

    // If queue is empty, set front and rear to new node.
    if (isQueueEmpty(q)) {
        q->front = newNode;
        q->rear = newNode;
        q->CURR_LENGTH++; //Increase queue length by 1
        return;
    }

    q->rear->next = newNode; // If queue is not empty, add new node to rear.
    q->rear = newNode; //Set rear to new node
    q->CURR_LENGTH++; //Increase queue length by 1
}

//Remove data from queue
cstmr dequeue(struct Queue *q) {

    // If queue is empty, return NULL customer.
    if (isQueueEmpty(q)) {
        printf("Error: Queue is empty\n");
        cstmr nullCustomer = {};
        return nullCustomer;
    }

    cstmr data = q->front->data; //Get data from front node
    struct Node *temp = q->front; //Store front node in temp
    q->front = q->front->next; //Move front node to next node
    free(temp); //Free temp node
    q->CURR_LENGTH--; //Reduce queue length by 1

    // If queue is empty, set rear to NULL.
    if (q->front == NULL) {
        q->rear = NULL;
    }

    return data;
}
