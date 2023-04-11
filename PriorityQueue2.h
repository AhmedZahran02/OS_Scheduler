#include <headers.h>

#define MAX_SIZE 10

struct Node
{
    int priority;
    struct Process data;
    struct Node *next;
};

struct PriorityQueue
{
    struct Node *front;
    int count;
};

typedef struct PriorityQueue PQ;

PQ create()
{
    PQ *Q = (PQ *)malloc(sizeof(PQ));
    Q->front = NULL;
    Q->count = 0;
    return *Q;
}

int isEmpty(PQ *Q)
{
    if (Q->count == 0)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

int size(PQ *Q)
{
    return Q->count;
}

void insert(PQ *Q, int priority, struct Process data)
{
    if (Q->count >= MAX_SIZE)
    {
        printf("Queue Overflow\n");
        return;
    }

    struct Node *temp = (struct Node *)malloc(sizeof(struct Node));
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
        struct Node *p, *q;
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

struct Process dequeue(PQ *Q)
{
    struct Process x;
    x.arrivalTime = -1;
    x.id = -1;
    x.Priority = -1;
    x.runTime = -1;
    if (isEmpty(Q))
    {
        return x;
    }
    x = Q->front->data;
    Q->front = Q->front->next;
    Q->count--;
    return x;
}

struct Process front(PQ *Q)
{
    struct Process x;
    x.arrivalTime = -1;
    x.id = -1;
    x.Priority = -1;
    x.runTime = -1;
    if (isEmpty(Q))
    {
        return x;
    }
    return Q->front->data;
}
