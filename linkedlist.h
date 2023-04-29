#include "headers.h"

struct ListNode
{
    int start;
    int end;
    ListNode *next;
};

ListNode *createHead(int start, int end)
{
    ListNode *head = (ListNode *)malloc(sizeof(ListNode));
    head->start = start;
    head->end = end;
    head->next = NULL;
    return head;
}

ListNode *insertNode(ListNode *head, int start, int end, bool dir)
{
    ListNode *node = (ListNode *)malloc(sizeof(ListNode));
    node->start = start;
    node->end = end;
    node->next = NULL;

    if (dir) // dir == 0 => insert from the back
    {
        if (head == NULL)
        {
            head = node;
        }
        else
        {
            ListNode *trav = head;
            while (trav->next != NULL)
            {
                trav = trav->next;
            }
            trav->next = node;
        }
    }
    else // dir == 0 => insert from the front
    {
        node->next = head;
        head = node;
    }
    return head;
}

ListNode *deleteNode(ListNode *head, ListNode *node)
{
    ListNode *trav = head;
    ListNode *toBeDel;
    if (node == head)
    {
        toBeDel = head;

        head = trav->next;
    }
    else
    {
        while (trav->next != node)
        {
            trav = trav->next;
        }

        toBeDel = trav->next;
        trav->next = trav->next->next;
    }

    return head;
}

ListNode *find(ListNode *head, int parameter, bool dir)
{
    ListNode *trav = head;

    if (dir == false) // dir == 0 then find start
    {
        while (trav != NULL)
        {
            if (trav->start == parameter)
            {

                break;
            }

            trav = trav->next;
        }
    }
    else // dir == 1 then find end
    {
        while (trav != NULL)
        {
            if (trav->end == parameter)
            {
                break;
            }
            trav = trav->next;
        }
    }
    return trav;
}

ListNode *insertSorted(ListNode *head, int start, int end)
{
    ListNode *node = (ListNode *)malloc(sizeof(ListNode));
    node->start = start;
    node->end = end;
    node->next = NULL;
    ListNode *trav;

    if (head == NULL)
    {
        head = node;
    }
    else if (start < head->start)
    {

        node->next = head;
        head = node;
    }
    else
    {

        ListNode *trav = head;
        while (trav)
        {
            if (trav->next != NULL && trav->next->start < start)
            {
                trav = trav->next;
            }
            else
            {
                break;
            }
        }
        node->next = trav->next;
        trav->next = node;
    }
    return head;
}

ListNode *findFirstFit(ListNode *head, int size)
{
    ListNode *trav = head;
    while (trav != NULL)
    {
        if (trav->end - trav->start + 1 >= size)
        {
            break;
        }
        trav = trav->next;
    }
    return trav;
}
ListNode *findBestFit(ListNode *head, int size)
{
    ListNode * trav = head;
    ListNode * best_node = NULL ;
    int best = 1025 ; // Assuming the maximum size is 1024
    while (trav != NULL)
    {
        int current_size = trav->end - trav->start + 1 ;
        if (current_size >= size && current_size < best )
        {
            best = current_size ;
            best_node = trav ;
        }
        trav = trav->next;
    }
    return best_node;
}
ListNode *deleteNodeByValues(ListNode *head, ListNode *node)
{
    ListNode *trav = head;
    ListNode *toBeDel;
    if (node == head)
    {
        toBeDel = head;
        head = trav->next;
    }
    else
    {
        while (trav->next->start != node->start && trav->next->end != node->end)
        {
            trav = trav->next;
        }
        toBeDel = trav->next;
        trav->next = trav->next->next;
    }
    return head;
}




void PrintList(ListNode * head){
    ListNode * trav = head;
    while (trav != NULL)
    {
        printf("St: %d , End:%d\n" , trav->start , trav->end);
        trav = trav->next;
    }
}

void destroyList(ListNode *head)
{
    ListNode *trav1 = head;
    ListNode *trav2 = NULL;
    while (trav1 != NULL)
    {
        trav2 = trav1->next;
        free(trav1);
        trav1 = trav2;
    }
}