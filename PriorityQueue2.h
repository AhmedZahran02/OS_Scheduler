#pragma once
#include "headers.h"

#define MAX_SIZE 10

struct Node2
{
    int priority;

    struct Process data;

    struct Node2 *next;
};

struct PriorityQueue2
{
    struct Node2 *front;
    int count;
};

typedef struct PriorityQueue2 PQ2;

PQ2 create()
{
    PQ2 *Q = (PQ2 *)malloc(sizeof(PQ2));
    Q->front = NULL;
    Q->count = 0;
    return *Q;
}

bool isEmpty2(PQ2 *Q)
{
    if (Q->count == 0)
    {
        return true;
    }
    else
    {
        return false;
    }
}

int size(PQ2 *Q)
{
    return Q->count;
}

void insert(PQ2 *Q, int priority, struct Process data)
{
    if (Q->count >= MAX_SIZE)
    {
        printf("Queue Overflow\n");
        return;
    }

    struct Node2 *temp = (struct Node2 *)malloc(sizeof(struct Node2));
    temp->priority = priority;
    temp->data = data;
    temp->next = NULL;

    if (Q->count == 0 || Q->front->priority > temp->priority)
    {
        temp->next = Q->front;
        Q->front = temp;
    }
    else
    {
        struct Node2 *p, *q;
        q = NULL;
        p = Q->front;
        while (p && p->priority <= temp->priority)
        {
            q = p;
            p = p->next;
        }
        temp->next = p;
        q->next = temp;
    }
    Q->count++;
}

struct Process dequeue2(PQ2 *Q)
{
    struct Process x;
    x.id = -1;
    x.arrivalTime = -1;
    x.Priority = -1;
    x.runTime = -1;
    if (isEmpty2(Q))
    {
        return x;
    }
    x = Q->front->data;
    Q->front = Q->front->next;
    Q->count--;
    return x;
}

struct Process front(PQ2 *Q)
{
    struct Process x;
    x.id = -1;
    x.arrivalTime = -1;
    x.Priority = -1;
    x.runTime = -1;
    if (isEmpty2(Q))
    {
        return x;
    }
    return Q->front->data;
}
