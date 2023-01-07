#include "pq.h"
#include "node.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <inttypes.h>

//This is the structure of the priority queue.
struct PriorityQueue {
    uint32_t
        top; //This variable should be the current size of the queue and should be the index to the node that has the most priority of being dequeued.
    uint32_t
        capacity; //This variable is the maximum space the array of nodes will be given. This is also used to check if the queue is full.
    Node **n; //This is the array of nodes that will be used for the priority queue.
};

//Declaration of the helper function insertion sort.
void insertion_sort(Node **n, uint32_t top);

//This is the constructor function for the priority queue. Sets the capacity to the capacity from the parameter and sets top to 0. Additionally it allocates memory for the array of nodes.
PriorityQueue *pq_create(uint32_t capacity) {
    PriorityQueue *q = (PriorityQueue *) malloc(sizeof(PriorityQueue));
    if (q) {
        q->capacity = capacity;
        q->top = 0;
        q->n = (Node **) calloc(capacity, sizeof(Node *));
        if (!q->n) {
            free(q);
            q = NULL;
        }
    }
    return q;
}

//This is the destructor function for the priority queue. Frees up all memories.
void pq_delete(PriorityQueue **q) {
    if (*q && (*q)->n) {
        free((*q)->n);
        free(*q);
        *q = NULL;
    }
    return;
}

//Returns true if the top index of the queue is 0 and false otherwise.
bool pq_empty(PriorityQueue *q) {
    if (q->top == 0) {
        return true;
    }
    return false;
}

//Returns true if the top index of the queue is equal to the capacity and false otherwise.
bool pq_full(PriorityQueue *q) {
    if (q->top == q->capacity) {
        return true;
    }
    return false;
}

//Returns the top index of the priority queue which would also be the size.
uint32_t pq_size(PriorityQueue *q) {
    return q->top;
}

//This function adds a node to the priority queue and then sorts the queue using insertion sort so that the top of the queue contains the node that has the highest priority. Returns false if the queue is full nd true if it successfully enqueued a node.
bool enqueue(PriorityQueue *q, Node *n) {
    if (pq_full(q)) {
        return false;
    }
    q->n[q->top] = n;
    q->top += 1;
    insertion_sort(q->n, q->top);
    return true;
}

//This function dequeues the top of the queue by passing the node with the most priority in the pointer provided in the parameter. returns false if the queue is empty and true if it successfully enqueued a node.
bool dequeue(PriorityQueue *q, Node **n) {
    if (pq_empty(q)) {
        return false;
    }
    q->top -= 1;
    *n = q->n[q->top];
    return true;
}

//This is the helper function that was completely ripped from assignment 3 and adjusted to fit the use of this ADT.
void insertion_sort(Node **n, uint32_t top) {
    for (uint32_t i = 1; i < top; i++) {
        uint32_t index = i;
        Node *memory = n[i];
        //This is where the comparisons and movements occurr for this sorting algorithm.
        while (index > 0 && memory->frequency > n[index - 1]->frequency) {

            n[index] = n[index - 1];
            index -= 1;
        }
        n[index] = memory;
    }
}
