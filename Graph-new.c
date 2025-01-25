#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "math.h"
#include "avl.h"

#define NUM_CELLS 18259722

typedef struct Cell
{
    int cell;
    struct Cell *next;
    struct Cell *prev;
    int op_type;
    int op_info;
    struct Node *root;

} Cell;

typedef struct Graph
{
    struct Cell **adjLists_head;
    struct Cell **adjLists_tail;
} Graph;

Cell *Addcell(int cell, int op_type, int op_info, Cell *prev)
{
    Cell *new_cell = (Cell *)malloc(sizeof(Cell));
    if (!new_cell)
    {
        printf("Cannot allocate memory for %d\n", cell);
        return NULL;
    }

    new_cell->cell = cell;
    new_cell->next = NULL;
    new_cell->prev = prev;
    new_cell->op_type = op_type;
    new_cell->op_info = op_info;
    new_cell->root = createNode(cell);

    if (!new_cell->root)
    {
        printf("Cannot allocate memory for AVL tree root for %d\n", cell);
        free(new_cell);
        return NULL;
    }

    return new_cell;
}

Graph *CreateGraph()
{
    Graph *graph = (Graph *)malloc(sizeof(Graph));
    if (!graph)
    {
        printf("Cannot allocate memory for graph\n");
        return NULL;
    }

    graph->adjLists_head = (Cell **)malloc(NUM_CELLS * sizeof(Cell *));
    graph->adjLists_tail = (Cell **)malloc(NUM_CELLS * sizeof(Cell *));
    if (!graph->adjLists_head || !graph->adjLists_tail)
    {
        printf("Cannot allocate memory for adjacency lists\n");
        free(graph->adjLists_head);
        free(graph->adjLists_tail);
        free(graph);
        return NULL;
    }

    for (int i = 0; i < NUM_CELLS; i++)
    {
        graph->adjLists_head[i] = NULL;
        graph->adjLists_tail[i] = NULL;
    }
    return graph;
}

void Addedge(int cell1, int cell2, int op_type, int op_info, Graph *graph)
{
    Cell *new_cell = Addcell(cell1, op_type, op_info, graph->adjLists_tail[cell2]);
    Cell *head_cell = Addcell(cell2, op_type, op_info, NULL);

    if (!new_cell)
        return;

    if (graph->adjLists_head[cell2] == NULL)
    {
        // printf("%d %d \n", cell1, cell2);
        graph->adjLists_head[cell2] = head_cell;
        graph->adjLists_tail[cell2] = new_cell;
        
    }
    else
    {
        // printf("%d %d", cell1, cell2);
        graph->adjLists_tail[cell2]->next = new_cell;
        graph->adjLists_tail[cell2] = new_cell;
    }
    graph->adjLists_head[cell2]->root = insert(graph->adjLists_head[cell2]->root, cell1);
}
/*
Op                    op_type   op_info
(+)                      1        NULL
(-)                      2        NULL
(* with Cell)            3        NULL
(* with const)           4        NULL
(/ denom with const)     5        NULL
(/ num with const)       6        NULL
(/ denom with cell)      7        NULL
(/ num with cell)        8        NULL
(MIN)                    9        NULL
(MAX)                    10       NULL
(AVG)                    11      |Range|
(SUM)                    12       NULL
(STDEV)                  13      |Range|
(SLEEP)                  14       NULL
*/
#include <stdio.h>
#include <stdlib.h>

// A helper function for DFS traversal to perform topological sort
void DFS(int node, Graph *graph, int *visited, int *topoArray, int *index)
{
    visited[node] = 1;

    // Traverse through all adjacent cells of the node
    Cell *curr = graph->adjLists_head[node];
    while (curr != NULL)
    {
        if (!visited[curr->cell])
        {
            DFS(curr->cell, graph, visited, topoArray, index);
        }
        curr = curr->next;
    }

    // After exploring all the adjacent nodes, add the current node to the topological array
    topoArray[(*index)++] = node;
}

// Function to perform topological sort
void toposort(Graph *graph, int *topoArray)
{
    int *visited = (int *)malloc(NUM_CELLS * sizeof(int));
    int index = 0;

    // Initialize all nodes as unvisited
    for (int i = 0; i < NUM_CELLS; i++)
    {
        visited[i] = 0;
    }

    // Perform DFS for every unvisited node
    for (int i = 0; i < NUM_CELLS; i++)
    {
        if (!visited[i])
        {
            DFS(i, graph, visited, topoArray, &index);
        }
    }

    // Reverse the topoArray to get the correct topological order
    for (int i = 0; i < NUM_CELLS / 2; i++)
    {
        int temp = topoArray[i];
        topoArray[i] = topoArray[NUM_CELLS - i - 1];
        topoArray[NUM_CELLS - i - 1] = temp;
    }

    free(visited);
}

void Recalc(int cell, int new_value, Graph *graph, int *cell_values)
{
    int old_value = cell_values[cell];
    cell_values[cell] = new_value;

    Cell *x = graph->adjLists_head[cell];
    if (x == NULL)
    {
        // printf("Returning");
        return;
    }
    // printf("%d\n", x->cell);
    // printf("%d\n", new_value);
    // printf("%d\n", old_value);
    // printf("%d\n", cell_values[x->cell]);
    while (x != NULL)
    {
        int dependent_new_value = cell_values[x->cell]; // Default value

        if (x->op_type == 0)
        {
            dependent_new_value = new_value;
        }

        if (x->op_type == 1)
        { // (+)
            dependent_new_value = cell_values[x->cell] + new_value - old_value;
        }
        else if (x->op_type == 2)
        { // (-)
            dependent_new_value = cell_values[x->cell] - new_value + old_value;
        }
        else if (x->op_type == 3)
        { // (*)
            if (old_value != 0)
            {
                dependent_new_value = (cell_values[x->cell] / old_value) * new_value;
                printf("%d\n", dependent_new_value);
            }
        }
        else if (x->op_type == 4)
        { // (/)
            dependent_new_value = (cell_values[x->cell] * old_value) / new_value;
        }
        else if (x->op_type == 5)
        {
            dependent_new_value = cell_values[x->cell] / old_value * new_value;
        }
        else if (x->op_type == 6)
        {
            dependent_new_value = cell_values[x->cell] * old_value / new_value;
        }
        else if (x->op_type == 7)
        {
            dependent_new_value = cell_values[x->cell] / old_value * new_value;
        }
        else if (x->op_type == 8)
        {
            dependent_new_value = cell_values[x->cell] * old_value / new_value;
        }
        else if (x->op_type == 9)
        {
            dependent_new_value = min(new_value, cell_values[x->cell]);
        }
        else if (x->op_type == 10)
        {
            dependent_new_value = max(new_value, cell_values[x->cell]);
        }
        else if (x->op_type == 11)
        {
            dependent_new_value = (x->op_info * cell_values[x->cell] - old_value + new_value) / x->op_info;
        }
        else if (x->op_type == 12)
        {
            dependent_new_value = cell_values[x->cell] - old_value + new_value;
        }

        // Update dependent cell and recursively recalculate
        Recalc(x->cell, dependent_new_value, graph, cell_values);
        cell_values[x->cell] = dependent_new_value;

        x = x->next;
    }
}