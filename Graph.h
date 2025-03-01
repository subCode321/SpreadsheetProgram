#ifndef GRAPH_H
#define GRAPH_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <math.h>

extern int NUM_CELLS;

// Structure for a cell in the adjacency list
typedef struct Cell
{
    int cell;
    struct Cell *next;
} Cell;

// Queue node structure for BFS
typedef struct QueueNode
{
    int cell;
    struct QueueNode *next;
} QueueNode;

// Queue structure
typedef struct Queue
{
    QueueNode *front;
    QueueNode *rear;
} Queue;

// Formula structure to represent different types of operations
typedef struct Formula
{
    int op_type;
    int op_info1;
    int op_info2;
} Formula;

// Graph structure holding the adjacency list
typedef struct Graph
{
    Cell **adjLists_head;
} Graph;

// Function prototypes
Graph *CreateGraph();
Cell *Addcell(int cell);
Cell *Addedge(int cell1, Cell *head);
Cell *Deletecell(int cell1, Cell *head);
Cell *Deleteedge(Graph *graph, int cell, int COLS, Formula *formulaArray);
Cell *Addedge_formula(Graph *graph, int cell, int COLS, Formula *formulaArray);

// Helper functions for nodes retrieval
void getNodesFromList(Cell *head, int *nodes, int *count);

// DFS functions for topological sort and cycle detection
void dfs(Graph *graph, int cell, int *visited, int *onStack, int *result, int *resultIndex, int *hasCycle);
int *topoSortFromCell(Graph *graph, int startCell, int *size, int *hasCycle);

// Operation functions
void Recalc(Graph *graph, int C, int *arr, int startCell, Formula *formulaArray);
void AddFormula(Graph *graph, int cell, int c1, int c2, int op_type, Formula *formulaArray);
void FreeGraph(Graph *graph);
    // Helper functions
int min2(int a, int b);
int max2(int a, int b);
int arithmetic_eval2(int v1, int v2, char op);

// External variables
extern int hasCycle;

#endif // GRAPH_H