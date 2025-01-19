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
    struct Cell** adjLists_head;
    struct Cell** adjLists_tail;

} Graph;

Cell *Addcell(int cell, int op_type, int op_info, Cell *prev)
{
    Cell* new_cell = (Cell *)malloc(sizeof(Cell));
    if (!new_cell) {
        printf("Cannot allocate memory for %d", cell);
        free(new_cell);
        return NULL;
    }

    new_cell->cell = cell;
    new_cell->next = NULL;
    new_cell->prev = prev;

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
    graph->adjLists_head = (Cell **)malloc(NUM_CELLS * sizeof(Cell *));
    graph->adjLists_tail = (Cell **)malloc(NUM_CELLS * sizeof(Cell *));

    if (!graph->adjLists_head)
    {
        printf("Cannot allocate memory for adjlists_heads");
        free(graph->adjLists_head);
        return NULL;
    }
    if (!graph->adjLists_tail)
    {
        printf("Cannot allocate memory for adjlists_tails");
        free(graph->adjLists_tail);
        return NULL;
    }
    for (int i = 0; i < NUM_CELLS; i++) {
        graph->adjLists_tail[i] = graph->adjLists_head[i];
    }
    return graph;
}

/*
Op                    op_type   op_info
(+)                      1        NULL
(-)                      2        NULL
(* with Cell)            3        Other Cell
(* with const)           4        const
(/ denom with const)     5        const
(/ num with const)       6        const
(/ denom with cell)      7        Other Cell
(/ num with cell)        8        Other Cell
(MIN)                    9        NULL
(MAX)                    10       NULL
(AVG)                    11      |Range|
(SUM)                    12       NULL
(STDEV)                  13      |Range|
(SLEEP)                  14       NULL
*/

void Addedge(int cell1, int cell2, int op_type, int op_info, Graph *graph) {
    Cell *new_cell = Addcell(cell1, op_type, op_info, graph->adjLists_tail[cell2]);

    if (graph->adjLists_head[cell2] == NULL)
    {
        graph->adjLists_head[cell2] = new_cell;
        graph->adjLists_tail[cell2] = new_cell;
    }
    else
    {
        graph->adjLists_tail[cell2]->next = new_cell;
        graph->adjLists_tail[cell2] = new_cell;
    }
}