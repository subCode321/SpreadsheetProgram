#include <stdio.h>
#define NUM_CELLS 18259722

typedef struct Cell
{
    int cell;      
    struct Cell *next;
    struct Cell *prev;
    int op_type;
    int op_info;

} Cell;

typedef struct Graph
{
    struct Cell** adjLists;

} Graph;

Cell* Addcell (int col, int row, int C, int R) 
{
    Cell* new_cell = (Cell *)malloc(sizeof(Cell));
    if (!new_cell) {
        printf("Cannot allocate memory for row = %d and col = %d", row, col);
        free(new_cell);
        return NULL;
    }

    new_cell->cell = row * C + col;
    new_cell->next = NULL;

    return new_cell;
}

Graph *CreateGraph()
{
    Graph* graph = (Graph *)malloc(sizeof(Graph));
    if (!graph) {
        printf("Cannot allocate memory for graph");
        free(graph);
        return NULL;
    }
    graph->adjLists = (Cell **)malloc(NUM_CELLS * sizeof(Cell*));

    if (!graph->adjLists)
    {
        printf("Cannot allocate memory for adjlists");
        free(graph->adjLists);
        return NULL;
    }

    return graph;
}

