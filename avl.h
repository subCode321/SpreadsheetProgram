#ifndef AVL_H
#define AVL_H

#include <stdio.h>
#include <stdlib.h>

typedef struct Node
{
    int key;           
    struct Node *left;  
    struct Node *right; 
    int height;         
} Node;

int getHeight(struct Node *n);

struct Node *createNode(int key);

int max(int a, int b);

int getBalanceFactor(struct Node *n);

struct Node *rightRotate(struct Node *y);

struct Node *leftRotate(struct Node *x);

struct Node *insert(struct Node *node, int key);

void inOrder(struct Node *root);

#endif 
