#ifndef GRAPH_H
#define GRAPH_H

typedef struct Stack
{
    int data;
    struct Stack *next_in_stack;
} Stack;

typedef struct Cell
{
    int cell_id;
    int op_type;
    int op_info;
    struct Cell *prev;
} Cell;

typedef struct Graph
{
    int num_cells;
    int num_edges;
    Cell **adjacency_list;
} Graph;

Stack *createStackNode(int data);
Cell *Addcell(int cell, int op_type, int op_info, Cell *prev);
Graph *CreateGraph(void);
void Addedge(int cell1, int cell2, int op_type, int op_info, Graph *graph);
void Toposort(Graph *graph, int v, int visited[], Stack **stack);
void Recalc(Stack *stack, Graph *graph, int *cell_values, int new_value, int givencell);

#define OP_ADD 1
#define OP_SUBTRACT 2
#define OP_MULT_CELL 3
#define OP_MULT_CONST 4
#define OP_DIV_DENOM_CONST 5
#define OP_DIV_NUM_CONST 6
#define OP_DIV_DENOM_CELL 7
#define OP_DIV_NUM_CELL 8
#define OP_MIN 9
#define OP_MAX 10
#define OP_AVG 11
#define OP_SUM 12
#define OP_STDEV 13
#define OP_SLEEP 14

#endif