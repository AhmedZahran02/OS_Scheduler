#pragma once
#include "headers.h"

// Define a node structure for the linked list
struct Node3
{
    Process data;
    struct Node3 *next;
};

// Define a queue structure
struct Queue2
{
    struct Node3 *front;
    struct Node3 *rear;
};

// Create a new node with the given data
struct Node3 *createNode(Process data)
{
    struct Node3 *newNode = (struct Node3 *)malloc(sizeof(struct Node3));
    newNode->data = data;
    newNode->next = NULL;
    return newNode;
}

// Initialize a new empty queue
struct Queue2 createQueue()
{
    struct Queue2 *queue = (struct Queue2 *)malloc(sizeof(struct Queue2));
    queue->front = NULL;
    queue->rear = NULL;
    return *queue;
}

// Check if the queue is empty
int isEmpty(struct Queue2 *queue)
{
    return (queue->front == NULL);
}

// Add an element to the rear of the queue
void enqueue(struct Queue2 *queue, Process data)
{
    struct Node3 *newNode = createNode(data);
    if (isEmpty(queue))
    {
        queue->front = newNode;
        queue->rear = newNode;
    }
    else
    {
        queue->rear->next = newNode;
        queue->rear = newNode;
    }
}

// Remove an element from the front of the queue
Process dequeue(struct Queue2 *queue)
{
    Process x;
    x.arrivalTime = -1;
    x.id = -1;
    x.Priority = -1;
    x.runTime = -1;
    if (isEmpty(queue))
    {
        printf("Queue is empty\n");
        return x;
    }
    struct Node3 *temp = queue->front;
    Process data = temp->data;
    queue->front = queue->front->next;
    if (queue->front == NULL)
    {
        queue->rear = NULL;
    }
    free(temp);
    return data;
}