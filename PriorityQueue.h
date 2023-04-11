#include <stdio.h>
#include <stdlib.h>

#define MAX_SIZE 10

struct Node
{
    int priority;
    int data;
    struct Node* next;
};

struct PriorityQueue {
    struct Node* front;
    int count;
};

typedef struct PriorityQueue PQ;

PQ create() {
    PQ* Q = (PQ*)malloc(sizeof(PQ));
    Q->front = NULL;
    Q->count = 0;
    return *Q;
}

int isEmpty(PQ* Q) {
    if(Q->count == 0) {
        return 1;
    } else {
        return 0;
    }
}

int size(PQ* Q) {
    return Q->count;
}

void insert(PQ* Q, int priority, int data)
{ 
    if(Q->count >= MAX_SIZE) {
        printf("Queue Overflow\n");
        return;
    }

    struct Node* temp = (struct Node*)malloc(sizeof(struct Node));
    temp->priority = priority;
    temp->data = data;
    temp->next = NULL;

    if(Q->count == 0 || Q->front->priority > temp->priority) {
        temp->next = Q->front ;
        Q->front = temp;
    }
    else 
    {
        struct Node* p, *q;
        q = NULL;
        p = Q->front;
        while(p && p->priority <= temp->priority) {
            q = p;
            p = p->next;
        }
        temp->next = p;
        q->next = temp;
    }
    Q->count++;
}

int dequeue(PQ* Q)
{
    if(isEmpty(Q)) {
        return -1;
    }
    int x = Q->front->data;
    Q->front = Q->front->next;
    Q->count--;
    return x;
}

int front(PQ* Q)
{
    if(isEmpty(Q)) {
        return -1;
    }
    return Q->front->data;
}
