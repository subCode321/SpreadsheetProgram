#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define NUM_CELLS 18259722

int min2(int a, int b)
{
    if (a < b)
        return a;
    return b;
}

int max2(int a, int b)
{
    if (a > b)
        return a;
    return b;
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

typedef struct Cell
{
    int cell;
    struct Cell *left;
    struct Cell *right;
    int op_type;
    int op_info;
    int height;
} Cell;

typedef struct Graph
{
    struct Cell **adjLists_head;
} Graph;

int height(Cell *c)
{
    if (c == NULL)
        return 0;
    return c->height;
}

int balance(Cell *c)
{
    if (c == NULL)
        return 0;
    return height(c->left) - height(c->right);
}

Cell *LL(Cell *y)
{ 
    Cell *x = y->left;
    Cell *T2 = x->right;

    x->right = y;
    y->left = T2;

    y->height = max2(height(y->left), height(y->right)) + 1;
    x->height = max2(height(x->left), height(x->right)) + 1;

    return x;
}

Cell *RR(Cell *x){
    Cell *y = x->right;
    Cell *T2 = y->left;

    y->left = x;
    x->right = T2;

    x->height = max2(height(x->left), height(x->right)) + 1;
    y->height = max2(height(y->left), height(y->right)) + 1;

    return y;
}


Cell *Addcell(int cell, int op_type, int op_info)
{
    Cell *new_cell = (Cell *)malloc(sizeof(Cell));
    if (!new_cell)
    {
        printf("Cannot allocate memory for %d\n", cell);
        return NULL;
    }

    new_cell->cell = cell;
    new_cell->left = NULL;
    new_cell->right = NULL;
    new_cell->op_type = op_type;
    new_cell->op_info = op_info;
    new_cell->height = 1; 
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
    if (!graph->adjLists_head)
    {
        printf("Cannot allocate memory for adjacency lists\n");
        free(graph);
        return NULL;
    }

    memset(graph->adjLists_head, 0, NUM_CELLS * sizeof(Cell *));
    return graph;
}

Cell *Addedge(int cell1, Cell *x, int op_type, int op_info)
{
    if (x == NULL)
    {
        return Addcell(cell1, op_type, op_info);
    }

    if (cell1 < x->cell)
    {
        x->left = Addedge(cell1, x->left, op_type, op_info);
    }
    else if (cell1 > x->cell)
    {
        x->right = Addedge(cell1, x->right, op_type, op_info);
    }
    else
    {
        return x;
    }

    x->height = 1 + max2(height(x->left), height(x->right));
    int bal = balance(x);

    if (bal > 1 && cell1 < x->left->cell)
        return LL(x);

    if (bal < -1 && cell1 > x->right->cell)
        return RR(x);

    if (bal > 1 && cell1 > x->left->cell)
    {
        x->left = RR(x->left);
        return LL(x);
    }

    if (bal < -1 && cell1 < x->right->cell)
    {
        x->right = LL(x->right);
        return RR(x);
    }

    return x;
}

void countInDegrees(Cell *root, int *inDegree)
{
    if (!root)
        return;
    countInDegrees(root->left, inDegree);
    inDegree[root->cell]++;
    countInDegrees(root->right, inDegree);
}

void processAdjacent(Cell *root, int *inDegree, int *queue, int *rear)
{
    if (!root)
        return;
    processAdjacent(root->left, inDegree, queue, rear);

    inDegree[root->cell]--;
    if (inDegree[root->cell] == 0)
    {
        queue[(*rear)++] = root->cell;
    }

    processAdjacent(root->right, inDegree, queue, rear);
}

int *topoSort(Graph *graph, int *size, int *hasCycle)
{
    *hasCycle = 0; // Initialize to no cycle
    *size = 0;     // Initialize size to 0

    int *inDegree = (int *)calloc(NUM_CELLS, sizeof(int));
    if (!inDegree)
        return NULL;

    // Count in-degrees
    int totalVertices = 0;
    for (int i = 0; i < NUM_CELLS; i++)
    {
        if (graph->adjLists_head[i] != NULL)
        {
            countInDegrees(graph->adjLists_head[i], inDegree);
        }
    }

    // Count all non-null nodes as vertices
    for (int i = 0; i < NUM_CELLS; i++)
    {
        if (graph->adjLists_head[i] != NULL || inDegree[i] > 0)
        {
            totalVertices++;
        }
    }

    // Create queue
    int *queue = (int *)malloc(NUM_CELLS * sizeof(int));
    if (!queue)
    {
        free(inDegree);
        return NULL;
    }
    int front = 0, rear = 0;

    // Add vertices with 0 in-degree
    for (int i = 0; i < NUM_CELLS; i++)
    {
        if (inDegree[i] == 0 && (graph->adjLists_head[i] != NULL))
        {
            queue[rear++] = i;
        }
    }

    // Create result array
    int *result = (int *)malloc(NUM_CELLS * sizeof(int));
    if (!result)
    {
        free(inDegree);
        free(queue);
        return NULL;
    }
    int resultIndex = 0;

    // Process vertices
    while (front < rear)
    {
        int v = queue[front++];
        result[resultIndex++] = v;

        // Process adjacent vertices
        processAdjacent(graph->adjLists_head[v], inDegree, queue, &rear);
    }

    // Check for cycle
    if (resultIndex < totalVertices)
    {
        *hasCycle = 1;
        free(result);
        free(inDegree);
        free(queue);
        return NULL;
    }

    // Cleanup
    free(inDegree);
    free(queue);

    *size = resultIndex;
    return result;
}

// void Recalc(int cell, int new_value, Graph *graph, int *cell_values)
// {
//     int old_value = cell_values[cell];
//     cell_values[cell] = new_value;

   
//     Cell *x = graph->adjLists_head[cell];
//     if (x == NULL) {
//         // printf("Returning");
//         return;
//     }
//     // printf("%d\n", x->cell);
//     // printf("%d\n", new_value);
//     // printf("%d\n", old_value);
//     // printf("%d\n", cell_values[x->cell]);
//     while (x != NULL)
//     {
//         int dependent_new_value = cell_values[x->cell]; // Default value

//         if (x->op_type ==0){
//             dependent_new_value = new_value;
//         }

//         if (x->op_type == 1)
//         { // (+)
//             dependent_new_value = cell_values[x->cell] + new_value - old_value;
//         }
//         else if (x->op_type == 2)
//         { // (-)
//             dependent_new_value = cell_values[x->cell] - new_value + old_value;
//         }
//         else if (x->op_type == 3)
//         { // (*)
//             if (old_value != 0)
//             {
//                 dependent_new_value = (cell_values[x->cell] / old_value) * new_value;
//                 printf("%d\n", dependent_new_value);
//             }
//         }
//         else if (x->op_type == 4)
//         { // (/)
//             dependent_new_value = (cell_values[x->cell] * old_value) / new_value;
//         }
//         else if (x->op_type == 5)
//         {
//             dependent_new_value = cell_values[x->cell] / old_value * new_value;
//         }
//         else if (x->op_type == 6)
//         {
//             dependent_new_value = cell_values[x->cell] * old_value / new_value;
//         }
//         else if (x->op_type == 7)
//         {
//             dependent_new_value = cell_values[x->cell] / old_value * new_value;
//         }
//         else if(x->op_type == 8)
//         {
//             dependent_new_value = cell_values[x->cell] * old_value / new_value;
//         }
//         else if(x->op_type == 9)
//         {
//             dependent_new_value = min2(new_value, cell_values[x->cell]);

//         }
//         else if (x->op_type == 10)
//         {
//             dependent_new_value = max2(new_value, cell_values[x->cell]);
//         }
//         else if (x->op_type == 11)
//         {
//             dependent_new_value = (x->op_info * cell_values[x->cell] - old_value + new_value) / x->op_info;
//         }
//         else if (x->op_type == 12)
//         {
//             dependent_new_value = cell_values[x->cell] - old_value + new_value;
//         }
    

            

//         // Update dependent cell and recursively recalculate
//         Recalc(x->cell, dependent_new_value, graph, cell_values);
//         cell_values[x->cell] = dependent_new_value;

//         x = x->next;
//     }
// }

void Recalc()
{
    ;
}
