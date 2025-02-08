#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <unistd.h> // For sleep function
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

int arithmetic_eval2(int v1, int v2, char op){
    if (op == '+'){
        return v1 + v2;
    }
    
    else if (op == '-'){
        return v1 - v2;
    }
    
    else if (op == '*'){
        return v1 * v2;
    }
    
    else if (op == '/'){
        return v1 / v2;
    }
}

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

void AddFormula(Graph *graph, Cell *cell, int c1, int c2, int op_type){
    Formula newFormula;
    // Assignment               1       Value         NULL
    // (Const + Const)          2       Value1       Value2
    // (Cell + Const)           3       Cell1        Value2
    // (Const + Cell)           4       Value1       Cell2
    // (Cell + Cell)            5       Cell1        Cell2
    newFormula.op_type = op_type;
    newFormula.op_info1 = -1;
    newFormula.op_info2 = -1;
    if(op_type == 0){
        newFormula.op_info1 = c1;
    }
    else{
        newFormula.op_info1 = c1;
        newFormula.op_info2 = c2;
    }
    formulaArray[cell->cell] = newFormula;
    
    
}

int getHeight(Cell *c)
{
    if (c == NULL)
        return 0;
    return c->height;
}

int getBalance(Cell *c)
{
    if (c == NULL)
        return 0;
    return getHeight(c->left) - getHeight(c->right);
}

Cell *rightRotation(Cell *y)
{ 
    Cell *x = y->left;
    Cell *T2 = x->right;

    x->right = y;
    y->left = T2;

    y->height = max2(getHeight(y->left), getHeight(y->right)) + 1;
    x->height = max2(getHeight(x->left), getHeight(x->right)) + 1;

    return x;
}

Cell *leftRotation(Cell *x){
    Cell *y = x->right;
    Cell *T2 = y->left;

    y->left = x;
    x->right = T2;

    x->height = max2(getHeight(x->left), getHeight(x->right)) + 1;
    y->height = max2(getHeight(y->left), getHeight(y->right)) + 1;

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

    x->height = 1 + max2(getHeight(x->left), getHeight(x->right));
    int bal = getBalance(x);

    if (bal > 1 && cell1 < x->left->cell)
        return rightRotation(x);

    if (bal < -1 && cell1 > x->right->cell)
        return leftRotation(x);

    if (bal > 1 && cell1 > x->left->cell)
    {
        x->left = leftRotation(x->left);
        return rightRotation(x);
    }

    if (bal < -1 && cell1 < x->right->cell)
    {
        x->right = rightRotation(x->right);
        return leftRotation(x);
    }

    return x;
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
(SLEEP Const)            23       NULL            `NULL
(SLEEP Cell)             24       Cell1            NULL

*/

Cell *Deletecell(int cell1, Cell *x);

Cell *Deleteedge(Graph *graph, int cell, int COLS)
{
    Formula x = formulaArray[cell];

    if (x.op_type == 3 || x.op_type == 7 || x.op_type == 11 || x.op_type == 15 || x.op_type == 24)
    {
        Cell *y = graph->adjLists_head[x.op_info1];
        Deletecell(cell, y);
    }
    else if(x.op_type == 4 || x.op_type == 8 || x.op_type == 12 || x.op_type == 16){
        Cell *y = graph->adjLists_head[x.op_info2];
        Deletecell(cell, y);
    }
    else if(x.op_type==5 || x.op_type == 9 || x.op_type == 13 || x.op_type == 17){
        Cell *y = graph->adjLists_head[x.op_info1];
        Deletecell(cell, y);
        Cell *z = graph->adjLists_head[x.op_info2];
        Deletecell(cell, z);
    }
    else if (x.op_type >= 18 && x.op_type <= 22) 
    {
        int startCell = x.op_info1;
        int endCell = x.op_info2;

        int startRow = startCell / COLS; 
        int startCol = startCell % COLS;
        int endRow = endCell / COLS;
        int endCol = endCell % COLS;

        
        for (int row = startRow; row <= endRow; ++row)
        {
            for (int col = startCol; col <= endCol; ++col)
            {
                int targetCell = row * COLS + col; 
                if (graph->adjLists_head[targetCell])
                {
                    Deletecell(cell, graph->adjLists_head[targetCell]);
                }
            }
        }
    }
    return NULL;
}



Cell *Deletecell(int cell1, Cell *x)
{
    if (x == NULL)
        return x;

    if (cell1 < x->cell)
        x->left = Deletecell(cell1, x->left);
    else if (cell1 > x->cell)
        x->right = Deletecell(cell1, x->right);
    else
    {
        if (x->left == NULL || x->right == NULL)
        {
            Cell *temp = x->left ? x->left : x->right;
            if (temp == NULL)
            {
                temp = x;
                x = NULL;
            }
            else
                *x = *temp;
            free(temp);
        }
        else
        {
            Cell *temp = x->right;
            while (temp->left != NULL)
                temp = temp->left;
            x->cell = temp->cell;
            x->right = Deletecell(temp->cell, x->right);
        }
    }

    if (x == NULL)
        return x;

    x->height = 1 + max2(getHeight(x->left), getHeight(x->right));
    int bal = getBalance(x);

    if (bal > 1 && getBalance(x->left) >= 0)
        return rightRotation(x);

    if (bal > 1 && getBalance(x->left) < 0)
    {
        x->left = leftRotation(x->left);
        return rightRotation(x);
    }

    if (bal < -1 && getBalance(x->right) <= 0)
        return leftRotation (x);

    if (bal < -1 && getBalance(x->right) > 0)
    {
        x->right = rightRotation(x->right);
        return leftRotation(x);
    }

    return x;
}

void dfsCollectCells(int cell, Graph *graph, int *visited, int *recStack, int *stack, int *stackSize, int *hasCycle);

int *topoSortFromCell(Graph *graph, int startCell, int *size, int *hasCycle)
{
    *hasCycle = 0;
    *size = 0;

    int *visited = (int *)calloc(NUM_CELLS, sizeof(int));
    int *recStack = (int *)calloc(NUM_CELLS, sizeof(int)); // Recursion stack for cycle detection
    int *stack = (int *)malloc(NUM_CELLS * sizeof(int));

    if (!visited || !recStack || !stack)
    {
        free(visited);
        free(recStack);
        free(stack);
        return NULL;
    }

    int stackSize = 0;

    // Start DFS from the modified cell and traverse its AVL dependency tree
    dfsCollectCells(startCell, graph, visited, recStack, stack, &stackSize, hasCycle);

    if (*hasCycle)
    {
        // If a cycle is detected, clean up and return NULL
        free(visited);
        free(recStack);
        free(stack);
        return NULL;
    }

    // No need to reverse the stack since we want the starting cell processed first
    int *result = (int *)malloc(stackSize * sizeof(int));
    if (!result)
    {
        free(visited);
        free(recStack);
        free(stack);
        return NULL;
    }

    for (int i = 0; i < stackSize; i++)
    {
        result[i] = stack[i];
    }

    *size = stackSize;

    free(visited);
    free(recStack);
    free(stack);

    return result;
}

// Helper function to perform in-order traversal of the AVL tree
void traverseAVLTree(Cell *root, Graph *graph, int *visited, int *recStack, int *stack, int *stackSize, int *hasCycle)
{
    if (!root || *hasCycle)
        return;

    // Process the current node (dependent cell) before traversing its dependencies
    int dependentCell = root->cell;
    if (!visited[dependentCell])
    {
        dfsCollectCells(dependentCell, graph, visited, recStack, stack, stackSize, hasCycle);
    }
    else if (recStack[dependentCell])
    {
        *hasCycle = 1; // Cycle detected
        return;
    }

    // Traverse the left and right subtrees
    traverseAVLTree(root->left, graph, visited, recStack, stack, stackSize, hasCycle);
    traverseAVLTree(root->right, graph, visited, recStack, stack, stackSize, hasCycle);
}

// Main DFS function that starts from a given cell and traverses its AVL tree of dependencies
void dfsCollectCells(int cell, Graph *graph, int *visited, int *recStack, int *stack, int *stackSize, int *hasCycle)
{
    visited[cell] = 1;
    recStack[cell] = 1; // Mark the current node in the recursion stack

    stack[(*stackSize)++] = cell; // Add current cell to stack before processing dependencies

    // Traverse the entire AVL tree of dependencies for this cell
    traverseAVLTree(graph->adjLists_head[cell], graph, visited, recStack, stack, stackSize, hasCycle);

    recStack[cell] = 0; // Remove from recursion stack after processing
}

void Recalc(Graph *graph, int C, int *arr, int startCell)
{
    int size, hasCycle;
    int *sortedCells = topoSortFromCell(graph, startCell, &size, &hasCycle);

    if (hasCycle)
    {
        printf("cycle");
        free(sortedCells);
        return;
    }

    // Reset all dependent cells before recalculation
    for (int i = 0; i < size; i++)
    {
        arr[sortedCells[i]] = 0;
    }

    for (int i = 0; i < size; i++)
    {
        int cell = sortedCells[i];
        Formula f = formulaArray[cell];

        if (f.op_type == 0)
        {
            arr[cell] = f.op_info1;
        }

        else if (f.op_type >= 1 && f.op_type <= 4) // Cell and constant case
        {
            int v1 = arr[f.op_info1];
            int v2 = f.op_info2;
            char op = (f.op_type == 1) ? '+' : (f.op_type == 2) ? '-'
                                           : (f.op_type == 3)   ? '*'
                                                                : '/';
            arr[cell] = arithmetic_eval2(v1, v2, op);
        }

        else if (f.op_type >= 5 && f.op_type <= 8) // Cell and cell case
        {
            int v1 = arr[f.op_info1];
            int v2 = arr[f.op_info2];
            char op = (f.op_type == 5) ? '+' : (f.op_type == 6) ? '-'
                                           : (f.op_type == 7)   ? '*'
                                                                : '/';
            arr[cell] = arithmetic_eval2(v1, v2, op);
        }

        else if (f.op_type >= 18 && f.op_type <= 22) // Range operations
        {
            int startCell = f.op_info1;
            int endCell = f.op_info2;

            int startRow = startCell / C;
            int startCol = startCell % C;
            int endRow = endCell / C;
            int endCol = endCell % C;

            int sum = 0, count = 0, minVal = arr[startCell], maxVal = arr[startCell], stdevSquared = 0;

            for (int row = startRow; row <= endRow; row++)
            {
                for (int col = startCol; col <= endCol; col++)
                {
                    int idx = row * C + col;
                    int val = arr[idx];

                    sum += val;
                    count++;
                    if (val < minVal)
                        minVal = val;
                    if (val > maxVal)
                        maxVal = val;
                }
            }

            double mean = (double)sum / count;
            for (int row = startRow; row <= endRow; row++)
            {
                for (int col = startCol; col <= endCol; col++)
                {
                    int idx = row * C + col;
                    stdevSquared += (arr[idx] - mean) * (arr[idx] - mean);
                }
            }

            if (f.op_type == 18)
                arr[cell] = minVal;
            else if (f.op_type == 19)
                arr[cell] = maxVal;
            else if (f.op_type == 20)
                arr[cell] = sum / count;
            else if (f.op_type == 21)
                arr[cell] = sum;
            else if (f.op_type == 22)
                arr[cell] = sqrt(stdevSquared / count);
        }

        else if (f.op_type == 23 || f.op_type == 24) // Delay operations
        {
            sleep(arr[f.op_info1]);
        }
    }

    free(sortedCells);
}
