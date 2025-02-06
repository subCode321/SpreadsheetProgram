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
Op                    op_type   op_info1    op_info2
Assignment               1       Value         NULL
(Const + Const)          2       Value1       Value2
(Cell + Const)           3       Cell1        Value2
(Const + Cell)           4       Value1       Cell2
(Cell + Cell)            5       Cell1        Cell2

(Const - Const)          6       Value1       Value2
(Cell - Const)           7       Cell1        Value2
(Const - Cell)           8       Value1       Cell2
(Cell - Cell)            9       Cell1        Cell2

(Const * Const)          10       Value1       Value2
(Cell * Const)           11       Cell1        Value2
(Const * Cell)           12       Value1       Cell2
(Cell * Cell)            13       Cell1        Cell2

(Const / Const)          14       Value1       Value2
(Cell / Const)           15       Cell1        Value2
(Const / Cell)           16       Value1       Cell2
(Cell / Cell)            17       Cell1        Cell2

(MIN)                    18      Starting Cell  Ending Cell
(MAX)                    19      Starting Cell  Ending Cell
(AVG)                    20      Starting Cell  Ending Cell
(SUM)                    21      Starting Cell  Ending Cell
(STDEV)                  22      Starting Cell  Ending Cell
(SLEEP Const)            23       NULL            NULL
(SLEEP Cell)             24       Cell1            NULL

*/
typedef struct Formula
{
    int op_type;
    int op_info1;
    int op_info2;

} Formula;

typedef struct Cell
{
    int cell;
    struct Cell *left;
    struct Cell *right;
    int height;
} Cell;

typedef struct Graph
{
    struct Cell **adjLists_head;
} Graph;

Formula formulaArray[NUM_CELLS];

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


Cell *Addcell(int cell)
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

Cell *Addedge(int cell1, Cell *x)
{
    if (x == NULL)
    {
        return Addcell(cell1);
    }

    if (cell1 < x->cell)
    {
        x->left = Addedge(cell1, x->left);
    }
    else if (cell1 > x->cell)
    {
        x->right = Addedge(cell1, x->right);
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

Cell *Deleteedge(int cell) {
    Formula x = formulaArray[cell];

    
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

void Recalc(Graph *graph, int cell)
{

}
