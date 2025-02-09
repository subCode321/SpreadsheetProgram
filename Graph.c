#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <unistd.h> // For sleep function
#include <limits.h>


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

/*
FORMULA                         OPTYPE
CELL=CONSTANT                       0
CELL=CELL+CONSTANT                  1
CELL=CELL-CONSTANT                  2
CELL=CELL*CONSTANT                  3
CELL=CELL/CONSTANT                  4
CELL=CELL+CELL                      5
CELL=CELL-CELL                      6
CELL=CELL*CELL                      7
CELL=CELL/CELL                      8
CELL=MIN(RANGE)                     9
CELL=MAX(RANGE)                     10
CELL=AVG(RANGE)                     11
CELL=SUM(RANGE)                     12
CELL=STDEV(RANGE)                   13
CELL=SLEEP(CONSTANT)                14
CELL=SLEEP(CELL)                    15
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

void printAVLTree(Cell *root, int level)
{
    if (root == NULL)
        return;

    // Print right subtree first (will appear at the top)
    printAVLTree(root->right, level + 1);

    // Print current node with proper indentation
    for (int i = 0; i < level; i++)
        printf("    "); // 4 spaces for each level

    // Print current node value and its height
    printf("%d (h=%d)\n", root->cell, root->height);

    // Print left subtree
    printAVLTree(root->left, level + 1);
}

// Wrapper function to print the AVL tree for a specific cell from the graph
void printCellDependencies(Graph *graph, int cell)
{
    if (cell < 0 || cell >= NUM_CELLS || graph == NULL)
    {
        printf("Invalid cell number or graph is NULL\n");
        return;
    }

    Cell *root = graph->adjLists_head[cell];
    if (root == NULL)
    {
        printf("Cell %d has no dependencies\n", cell);
        return;
    }

    printf("Dependencies for cell %d:\n", cell);
    printAVLTree(root, 0);
}

Cell *Deletecell(int cell1, Cell *x);
Cell *Deleteedge(Graph *graph, int cell, int COLS)
{
    Formula x = formulaArray[cell];

    if (x.op_type >= 1 && x.op_type <= 4)
    {
        graph->adjLists_head[x.op_info1] = Deletecell(cell, graph->adjLists_head[x.op_info1]);
    }
    else if (x.op_type >= 5 && x.op_type <= 8)
    {
        graph->adjLists_head[x.op_info1] = Deletecell(cell, graph->adjLists_head[x.op_info1]);
        graph->adjLists_head[x.op_info2] = Deletecell(cell, graph->adjLists_head[x.op_info2]);
    }
    else if (x.op_type >= 9 && x.op_type <= 13)
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
                graph->adjLists_head[targetCell] = Deletecell(cell, graph->adjLists_head[targetCell]);
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
        return NULL;
    }

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
        result[i] = stack[stackSize - 1 - i]; 
    }

    *size = stackSize;

    free(visited);
    free(recStack);
    free(stack);

    return result;
}

void traverseAVLTree(Cell *root, Graph *graph, int *visited, int *recStack, int *stack, int *stackSize, int *hasCycle)
{
    if (!root || *hasCycle)
        return;

    int dependentCell = root->cell;

    // Process left and right subtrees first
    traverseAVLTree(root->left, graph, visited, recStack, stack, stackSize, hasCycle);
    traverseAVLTree(root->right, graph, visited, recStack, stack, stackSize, hasCycle);

    if (!visited[dependentCell])
    {
        visited[dependentCell] = 1;
        recStack[dependentCell] = 1;

        // Process dependencies through the adjacency list
        Cell *deps = graph->adjLists_head[dependentCell];
        if (deps)
        {
            traverseAVLTree(deps, graph, visited, recStack, stack, stackSize, hasCycle);
        }

        // Only add to stack and remove from recStack if we haven't detected a cycle
        if (!*hasCycle)
        {
            stack[(*stackSize)++] = dependentCell;
            recStack[dependentCell] = 0;
        }
    }
    else if (recStack[dependentCell])
    {
        // Check if this is actually forming a cycle
        // We need to verify if this cell is part of a genuine dependency cycle
        Formula f = formulaArray[dependentCell];
        int isCycle = 0;

        // Check if this cell depends on any cells currently in the recStack
        if (f.op_type >= 1 && f.op_type <= 4)
        {
            if (recStack[f.op_info1])
                isCycle = 1;
        }
        else if (f.op_type >= 5 && f.op_type <= 8)
        {
            if (recStack[f.op_info1] || recStack[f.op_info2])
                isCycle = 1;
        }

        if (isCycle)
        {
            *hasCycle = 1;
        }
    }
}
void dfsCollectCells(int cell, Graph *graph, int *visited, int *recStack, int *stack, int *stackSize, int *hasCycle)
{
    if (*hasCycle)
        return;

    printf("Starting DFS from cell: %d\n", cell);

    if (!visited[cell])
    {
        printf("Cell %d not visited, marking visited and adding to recStack\n", cell);
        visited[cell] = 1;
        recStack[cell] = 1;

        // Process dependencies through AVL tree
        if (graph->adjLists_head[cell])
        {
            printf("Processing dependencies of starting cell %d\n", cell);
            traverseAVLTree(graph->adjLists_head[cell], graph, visited, recStack, stack, stackSize, hasCycle);
        }

        // Add the starting cell to stack after its dependencies
        printf("Adding starting cell %d to stack and removing from recStack\n", cell);
        stack[(*stackSize)++] = cell;
        recStack[cell] = 0;
    }
    else if (recStack[cell])
    {
        printf("CYCLE DETECTED: Starting cell %d already in recStack\n", cell);
        *hasCycle = 1;
    }
    else
    {
        printf("Starting cell %d already visited but not in recStack\n", cell);
    }
}

/*
FORMULA                         OPTYPE
CELL=CONSTANT                       0
CELL=CELL+CONSTANT                  1
CELL=CELL-CONSTANT                  2
CELL=CELL*CONSTANT                  3
CELL=CELL/CONSTANT                  4
CELL=CELL+CELL                      5
CELL=CELL-CELL                      6
CELL=CELL*CELL                      7
CELL=CELL/CELL                      8
CELL=MIN(RANGE)                     9
CELL=MAX(RANGE)                     10
CELL=AVG(RANGE)                     11
CELL=SUM(RANGE)                     12
CELL=STDEV(RANGE)                   13
CELL=SLEEP(CONSTANT)                14
CELL=SLEEP(CELL)                    15
*/


void Recalc(Graph *graph, int C, int *arr, int startCell)
{
    int size, hasCycle;
    int *sortedCells = topoSortFromCell(graph, startCell, &size, &hasCycle);
    for (int i = 0; i < size; i++) {
        printf("%d ", sortedCells[i]);
        printCellDependencies(graph, sortedCells[i]);
        printf("\n");
    }
    printf("\n");
    if (hasCycle)
    {
        printf("Error: Circular dependency detected. Command rejected.\n");
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
            if (f.op_info1 == INT_MIN)
            {
                arr[cell] = INT_MIN; // Propagate error
            }
            else
            {
                arr[cell] = f.op_info1; // Assign valid value
            }
        }

        else if (f.op_type >= 1 && f.op_type <= 4) // Cell and constant case
        {
            int v1 = arr[f.op_info1]; 
            int v2 = f.op_info2;      

            if (v1 == INT_MIN)
            {
                arr[cell] = INT_MIN; // Propagate error
                continue;
            }

            
            char op = (f.op_type == 1) ? '+' : (f.op_type == 2) ? '-'
                                           : (f.op_type == 3)   ? '*'
                                                                : '/';

            
            if (op == '/' && v2 == 0)
            {
                printf("Error: Division by zero in cell %d\n", cell);
                arr[cell] = INT_MIN; // Propagate error
                continue;
            }

            arr[cell] = arithmetic_eval2(v1, v2, op); // Perform operation
        }

        else if (f.op_type >= 5 && f.op_type <= 8) // Cell and cell case
        {
            int v1 = arr[f.op_info1];
            int v2 = arr[f.op_info2];

            if (f.op_type == 8 && v2 == 0)
            {
                printf("Error: Division by zero in cell %d\n", cell);
                arr[cell] = INT_MIN; // Mark this cell as ERR
                continue;
            }

            if (v1 == INT_MIN || v2 == INT_MIN)
            {
                arr[cell] = INT_MIN; // Propagate error
                continue;
            }

            char op = (f.op_type == 5) ? '+' : (f.op_type == 6) ? '-'
                                           : (f.op_type == 7)   ? '*'
                                                                : '/';
            arr[cell] = arithmetic_eval2(v1, v2, op);
        }

        else if (f.op_type >= 9 && f.op_type <= 13) // Range operations
        {
            int startCell = f.op_info1;
            int endCell = f.op_info2;

            int startRow = startCell / C;
            int startCol = startCell % C;
            int endRow = endCell / C;
            int endCol = endCell % C;

            int sum = 0, count = 0, stdevSquared = 0;
            int minVal = INT_MAX, maxVal = INT_MIN;
            int hasError = 0; // Track if any cell in the range has an error

            for (int row = startRow; row <= endRow; row++)
            {
                for (int col = startCol; col <= endCol; col++)
                {
                    int idx = row * C + col;
                    int val = arr[idx];

                    if (val == INT_MIN)
                    {
                        hasError = 1;
                        break;
                    }

                    sum += val;
                    count++;
                    if (val < minVal)
                        minVal = val;
                    if (val > maxVal)
                        maxVal = val;
                }
                if (hasError)
                    break;
            }

            if (hasError)
            {
                arr[cell] = INT_MIN; // Propagate error
                continue;
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

            if (f.op_type == 9)
                arr[cell] = minVal;
            else if (f.op_type == 10)
                arr[cell] = maxVal;
            else if (f.op_type == 11)
                arr[cell] = sum / count;
            else if (f.op_type == 12)
                arr[cell] = sum;
            else if (f.op_type == 13)
                arr[cell] = sqrt(stdevSquared / count);
        }

        else if (f.op_type == 14) // Delay operations (SLEEP)
        {
            if (arr[f.op_info1] == INT_MIN)
            {
                arr[cell] = INT_MIN; // Propagate error
                continue;
            }

            int sleepTime = arr[f.op_info1];
            sleep(sleepTime);
            arr[cell] = sleepTime; // Assign the sleep value to the cell
        }
        else
        {
            printf("Error: Unsupported operation type %d in cell %d\n", f.op_type, cell);
            arr[cell] = INT_MIN; // Mark as error
        }
    }

    free(sortedCells);
}
