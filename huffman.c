#include "node.h"
#include "io.h"
#include "code.h"
#include "stack.h"
#include "pq.h"
#include "defines.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <inttypes.h>

static Code c;
//Place holder for a left node used in Build Tree and Rebuild Tree function.
Node *left = NULL;
Node *right = NULL;
Node *root = NULL;
Node *parent = NULL;
uint8_t bit;
uint8_t leaf = 'L';
uint8_t interior = 'I';

//Helper function to keep c static.
void help_build_codes(Node *root, Code table[static ALPHABET], Code c);

Node *build_tree(uint64_t hist[static ALPHABET]) {
    PriorityQueue *q = pq_create(ALPHABET);

    for (int i = 0; i < ALPHABET; i++) {
        if (hist[i] > 0) {
            Node *n = node_create(i, hist[i]);
            enqueue(q, n);
        }
    }

    while (pq_size(q) > 1) {
        dequeue(q, &left);
        dequeue(q, &right);
        parent = node_join(left, right);
        enqueue(q, parent);
    }
    dequeue(q, &root);

    pq_delete(&q);

    return root;
}

void build_codes(Node *root, Code table[static ALPHABET]) {
    c = code_init();
    help_build_codes(root, table, c);
    return;
}

void dump_tree(int outfile, Node *root) {
    if (root != NULL) {
        dump_tree(outfile, root->left);
        dump_tree(outfile, root->right);

        if (root->left == NULL && root->right == NULL) {
            write_bytes(outfile, &leaf, 1);
            write_bytes(outfile, &root->symbol, 1);
        } else {
            write_bytes(outfile, &interior, 1);
        }
    }
}

//This function is based off the instructions given in the assignment pdf at 10.2.
Node *rebuild_tree(uint16_t nbytes, uint8_t tree[static nbytes]) {
    Stack *s = stack_create(MAX_TREE_SIZE);
    for (int i = 0; i < nbytes; i++) {
        if (tree[i] == 'L') {
            Node *n = node_create(tree[i + 1], i);
            i += 1;
            stack_push(s, n);
        } else if (tree[i] == 'I') {
            stack_pop(s, &right);
            stack_pop(s, &left);
            parent = node_join(left, right);
            stack_push(s, parent);
        }
    }
    stack_pop(s, &root);

    stack_delete(&s);
    return root;
}

void delete_tree(Node **root) {

    //If the node is a leaf, delete the node.
    if ((*root)->left == NULL && (*root)->right == NULL) {
        node_delete(root);
    } else {

        //If the left node of the root exists, go down that path.
        if ((*root)->left != NULL) {
            delete_tree(&(*root)->left);
        }

        //If the right node of the root exists, go down that path.
        if ((*root)->right != NULL) {
            delete_tree(&(*root)->right);
        }

        node_delete(root);
    }
}

void help_build_codes(Node *root, Code table[static ALPHABET], Code c) {
    if (root != NULL) {
        if (root->left == NULL && root->right == NULL) {
            table[root->symbol] = c;
        } else {
            code_push_bit(&c, 0);
            help_build_codes(root->left, table, c);
            code_pop_bit(&c, &bit);

            code_push_bit(&c, 1);
            help_build_codes(root->right, table, c);
            code_pop_bit(&c, &bit);
        }
    }
    return;
}
