#ifndef GRAPH_H
#define GRAPH_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

extern int NUM_CELLS; // Define the maximum number of cells

// Formula structure to represent different types of operations
typedef struct Formula
{
    int op_type;
    int op_info1;
    int op_info2;
} Formula;

// Cell structure representing a node in an AVL tree
typedef struct Cell
{
    int cell;
    struct Cell *left;
    struct Cell *right;
    int height;
} Cell;

// Graph structure holding the adjacency list (as AVL trees)
typedef struct Graph
{
    struct Cell **adjLists_head;
} Graph;

typedef struct QueueNode
{
    int cell;
    struct QueueNode *next;
} QueueNode;

typedef struct Queue
{
    QueueNode *front, *rear;
} Queue;

// Function prototypes
Graph *CreateGraph();
Cell *Addcell(int cell);
Cell *Addedge(int cell1, Cell *x);
Cell *Deletecell(int cell1, Cell *x);
Queue *createQueue();
void enqueue(Queue *q, int cell);
int dequeue(Queue *q);

    int getHeight(Cell *c);
int getBalance(Cell *c);
Cell *rightRotation(Cell *y);
Cell *leftRotation(Cell *x);


void getNodesFromAVL(Cell *root, int *nodes, int *count) ;
int *topoSortFromCell(Graph *graph, int startCell, int *size, int *hasCycle);
extern Formula *formulaArray;
void Recalc(Graph *graph, int C, int *arr, int startCell, Formula *formulaArray);
void AddFormula(Graph *graph, int cell, int c1, int c2, int op_type,Formula *formulaArray);
Cell *Deleteedge(Graph *graph, int cell, int COLS,Formula *formulaArray);

int min2(int a, int b);
int max2(int a, int b);
void printAVLTree(Cell *root, int level);
void printCellDependencies(Graph *graph, int cell);

// Add this if arithmetic evaluation or square root is defined elsewhere

#endif // GRAPH_H
