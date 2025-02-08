#ifndef GRAPH_H
#define GRAPH_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define NUM_CELLS 18259722 // Define the maximum number of cells

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

// Function prototypes
Graph *CreateGraph();
Cell *Addcell(int cell);
Cell *Addedge(int cell1, Cell *x);
Cell *Deleteedge(Graph *graph, int cell, int COLS);
Cell *Deletecell(int cell1, Cell *x);

int getHeight(Cell *c);
int getBalance(Cell *c);
Cell *rightRotation(Cell *y);
Cell *leftRotation(Cell *x);

void AddFormula(Graph *graph, Cell *cell, int c1, int c2, int op_type);

void traverseAVLTree(Cell *root, Graph *graph, int *visited, int *recStack, int *stack, int *stackSize, int *hasCycle);
void dfsCollectCells(int cell, Graph *graph, int *visited, int *recStack, int *stack, int *stackSize, int *hasCycle);
int *topoSortFromCell(Graph *graph, int startCell, int *size, int *hasCycle);
void Recalc(Graph *graph, int C, int *arr, int startCell);
extern Formula formulaArray[NUM_CELLS];
int min2(int a, int b);
int max2(int a, int b);

// Add this if arithmetic evaluation or square root is defined elsewhere

#endif // GRAPH_H
