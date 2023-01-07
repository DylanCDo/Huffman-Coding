#include "node.h"

#include <stdio.h>
#include <stdlib.h>

//This is the constructor function for the node ADT. Sets the left and right child of the ndoe to NULL and the symbol to the symbol passed in the parameters and frequency the frequency passed in the parameters. Returns the node afterwards.
Node *node_create(uint8_t symbol, uint64_t frequency) {
    Node *n = (Node *) malloc(sizeof(Node));
    n->left = NULL;
    n->right = NULL;
    n->symbol = symbol;
    n->frequency = frequency;
    return n;
}

//This is the destructor function for the node ADT. Frees the memory allocated to the node.
void node_delete(Node **n) {
    free(*n);
    *n = NULL;
    return;
}

//This constructor function acts a way to create a new node by using its left and right child. sets the symbol to '$' and the frequency the sum of the left and right frequency. Also set sthe left and right child to the left and right nodes that were passed in the parameter. Returns the newly joined node.
Node *node_join(Node *left, Node *right) {
    Node *n = node_create('$', (left->frequency + right->frequency));
    n->left = left;
    n->right = right;
    return n;
}
