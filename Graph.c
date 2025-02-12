#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <unistd.h> // For sleep function
#include <limits.h>
#include "Graph.h"

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

int arithmetic_eval2(int v1, int v2, char op)
{
    if (op == '+')
    {
        return v1 + v2;
    }

    else if (op == '-')
    {
        return v1 - v2;
    }

    else if (op == '*')
    {
        return v1 * v2;
    }

    else if (op == '/')
    {
        return v1 / v2;
    }
    return INT_MIN;
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


void AddFormula(Graph *graph, Cell *cell, int c1, int c2, int op_type,Formula *formulaArray)
{
    Formula newFormula;
    // Assignment               1       Value         NULL
    // (Const + Const)          2       Value1       Value2
    // (Cell + Const)           3       Cell1        Value2
    // (Const + Cell)           4       Value1       Cell2
    // (Cell + Cell)            5       Cell1        Cell2
    newFormula.op_type = op_type;
    newFormula.op_info1 = -1;
    newFormula.op_info2 = -1;
    if (op_type == 0)
    {
        newFormula.op_info1 = c1;
    }
    else
    {
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

Cell *leftRotation(Cell *x)
{
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
Cell *Deleteedge(Graph *graph, int cell, int COLS,Formula *formulaArray)
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
        return leftRotation(x);

    if (bal < -1 && getBalance(x->right) > 0)
    {
        x->right = rightRotation(x->right);
        return leftRotation(x);
    }

    return x;
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

Queue *createQueue()
{
    Queue *q = (Queue *)malloc(sizeof(Queue));
    q->front = q->rear = NULL;
    return q;
}

void enqueue(Queue *q, int cell)
{
    QueueNode *newNode = (QueueNode *)malloc(sizeof(QueueNode));
    newNode->cell = cell;
    newNode->next = NULL;

    if (q->rear == NULL)
    {
        q->front = q->rear = newNode;
        return;
    }

    q->rear->next = newNode;
    q->rear = newNode;
}

int dequeue(Queue *q)
{
    if (q->front == NULL)
        return -1;

    QueueNode *temp = q->front;
    int cell = temp->cell;

    q->front = q->front->next;
    if (q->front == NULL)
        q->rear = NULL;

    free(temp);
    return cell;
}

void getNodesFromAVL(Cell *root, int *nodes, int *count)
{
    if (root == NULL)
        return;

    getNodesFromAVL(root->left, nodes, count);
    nodes[(*count)++] = root->cell;
    printf("    AVL traversal: added node %d at position %d\n", root->cell, *count - 1);
    getNodesFromAVL(root->right, nodes, count);
}

int *topoSortFromCell(Graph *graph, int startCell, int *size, int *hasCycle)
{
    printf("\n=== Starting topoSort from cell %d ===\n", startCell);
    *size = 0;
    *hasCycle = 0;

    // Create arrays for storing result and tracking visited/in-degree
    int *result = (int *)malloc(NUM_CELLS * sizeof(int));
    int *inDegree = (int *)calloc(NUM_CELLS, sizeof(int));
    int *reachable = (int *)calloc(NUM_CELLS, sizeof(int));
    Queue *q = createQueue();

    printf("\n--- Starting BFS Discovery Phase ---\n");
    // First discover all reachable nodes using BFS and calculate their in-degrees
    Queue *discovery = createQueue();
    enqueue(discovery, startCell);
    reachable[startCell] = 1;
    printf("Added start cell %d to discovery queue\n", startCell);

    while (discovery->front != NULL)
    {
        int current = dequeue(discovery);
        printf("\nProcessing node %d in discovery phase\n", current);

        if (graph->adjLists_head[current] != NULL)
        {
            printf("  Node %d has adjacency list, processing dependencies...\n", current);
            int *nodes = (int *)malloc(NUM_CELLS * sizeof(int));
            int count = 0;
            getNodesFromAVL(graph->adjLists_head[current], nodes, &count);

            for (int i = 0; i < count; i++)
            {
                int dependent = nodes[i];
                inDegree[dependent]++;
                printf("  Increased in-degree of node %d to %d\n", dependent, inDegree[dependent]);

                if (!reachable[dependent])
                {
                    reachable[dependent] = 1;
                    enqueue(discovery, dependent);
                    printf("  Marked node %d as reachable and added to discovery queue\n", dependent);
                }
            }
            free(nodes);
        }
        else
        {
            printf("  Node %d has no dependencies\n", current);
        }
    }
    free(discovery);

    printf("\n--- Starting Topological Sort Phase ---\n");
    printf("Adding start cell %d to processing queue\n", startCell);
    if (inDegree[startCell] > 0)
    {
        printf("CYCLE DETECTED: Node %d still has positive in-degree %d\n", startCell, inDegree[startCell]);
        *hasCycle = 1;
        free(result);
        free(inDegree);
        free(reachable);
        while (q->front != NULL)
        {
            dequeue(q);
        }
        free(q);
        return NULL;
    }
    enqueue(q, startCell);

    while (q->front != NULL)
    {
        int current = dequeue(q);
        result[(*size)++] = current;
        printf("\nProcessing node %d in topo sort (position %d in result)\n", current, *size - 1);

        if (graph->adjLists_head[current] != NULL)
        {
            printf("  Node %d has adjacency list, processing dependencies...\n", current);
            int *nodes = (int *)malloc(NUM_CELLS * sizeof(int));
            int count = 0;
            getNodesFromAVL(graph->adjLists_head[current], nodes, &count);

            for (int i = 0; i < count; i++)
            {
                int dependent = nodes[i];
                inDegree[dependent]--;
                printf("  Decreased in-degree of node %d to %d\n", dependent, inDegree[dependent]);

                if (inDegree[dependent] == 0)
                {
                    enqueue(q, dependent);
                    printf("  Node %d has in-degree 0, added to processing queue\n", dependent);
                }
            }
            free(nodes);
        }
        else
        {
            printf("  Node %d has no dependencies\n", current);
        }
    }

    printf("\n--- Checking for Cycles ---\n");
    for (int i = 0; i < NUM_CELLS; i++)
    {
        if (reachable[i])
        {
            printf("Node %d is reachable with final in-degree %d\n", i, inDegree[i]);
            if (inDegree[i] > 0)
            {
                printf("CYCLE DETECTED: Node %d still has positive in-degree %d\n", i, inDegree[i]);
                *hasCycle = 1;
                free(result);
                free(inDegree);
                free(reachable);
                while (q->front != NULL)
                {
                    dequeue(q);
                }
                free(q);
                return NULL;
            }
        }
    }

    printf("\n--- Topological Sort Completed Successfully ---\n");
    printf("Final order: ");
    for (int i = 0; i < *size; i++)
    {
        printf("%d ", result[i]);
    }
    printf("\n");

    free(inDegree);
    free(reachable);
    free(q);
    return result;
}

// void Recalc(Graph *graph, int C, int *arr, int startCell)
// {

//     int size, hasCycle;
//     int *sortedCells = topoSortFromCell(graph, startCell, &size, &hasCycle);
//     if (hasCycle)
//     {
//         printf("Error: Circular dependency detected. Command rejected.\n");
//         free(sortedCells);
//         return;
//     }

//     for (int i = 0; i < size; i++)
//     {
//         arr[sortedCells[i]] = 0;
//     }

//     for (int i = 0; i < size; i++)
//     {
//         int cell = sortedCells[i];
//         Formula f = formulaArray[cell];

//         if (f.op_type == 0)
//         {
//             if (f.op_info1 == INT_MIN)
//             {
//                 arr[cell] = INT_MIN; // Propagate error
//             }
//             else
//             {
//                 arr[cell] = f.op_info1; // Assign valid value
//             }
//         }

//         else if (f.op_type >= 1 && f.op_type <= 4) // Cell and constant case
//         {
//             int v1 = arr[f.op_info1];
//             int v2 = f.op_info2;

//             if (v1 == INT_MIN)
//             {
//                 arr[cell] = INT_MIN; // Propagate error
//                 continue;
//             }

//             char op = (f.op_type == 1) ? '+' : (f.op_type == 2) ? '-'
//                                            : (f.op_type == 3)   ? '*'
//                                                                 : '/';

//             if (op == '/' && v2 == 0)
//             {
//                 printf("Error: Division by zero in cell %d\n", cell);
//                 arr[cell] = INT_MIN; // Propagate error
//                 continue;
//             }

//             arr[cell] = arithmetic_eval2(v1, v2, op); // Perform operation
//         }

//         else if (f.op_type >= 5 && f.op_type <= 8) // Cell and cell case
//         {
//             int v1 = arr[f.op_info1];
//             int v2 = arr[f.op_info2];

//             if (f.op_type == 8 && v2 == 0)
//             {
//                 printf("Error: Division by zero in cell %d\n", cell);
//                 arr[cell] = INT_MIN; // Mark this cell as ERR
//                 continue;
//             }

//             if (v1 == INT_MIN || v2 == INT_MIN)
//             {
//                 arr[cell] = INT_MIN; // Propagate error
//                 continue;
//             }

//             char op = (f.op_type == 5) ? '+' : (f.op_type == 6) ? '-'
//                                            : (f.op_type == 7)   ? '*'
//                                                                 : '/';
//             arr[cell] = arithmetic_eval2(v1, v2, op);
//         }

//         else if (f.op_type >= 9 && f.op_type <= 13) // Range operations
//         {
//             int startCell = f.op_info1;
//             int endCell = f.op_info2;

//             int startRow = startCell / C;
//             int startCol = startCell % C;
//             int endRow = endCell / C;
//             int endCol = endCell % C;

//             int sum = 0, count = 0, stdevSquared = 0;
//             int minVal = INT_MAX, maxVal = INT_MIN;
//             int hasError = 0; // Track if any cell in the range has an error

//             for (int row = startRow; row <= endRow; row++)
//             {
//                 for (int col = startCol; col <= endCol; col++)
//                 {
//                     int idx = row * C + col;
//                     int val = arr[idx];

//                     if (val == INT_MIN)
//                     {
//                         hasError = 1;
//                         break;
//                     }

//                     sum += val;
//                     count++;
//                     if (val < minVal)
//                         minVal = val;
//                     if (val > maxVal)
//                         maxVal = val;
//                 }
//                 if (hasError)
//                     break;
//             }

//             if (hasError)
//             {
//                 arr[cell] = INT_MIN; // Propagate error
//                 continue;
//             }

//             double mean = (double)sum / count;
//             for (int row = startRow; row <= endRow; row++)
//             {
//                 for (int col = startCol; col <= endCol; col++)
//                 {
//                     int idx = row * C + col;
//                     stdevSquared += (arr[idx] - mean) * (arr[idx] - mean);
//                 }
//             }

//             if (f.op_type == 9)
//                 arr[cell] = minVal;
//             else if (f.op_type == 10)
//                 arr[cell] = maxVal;
//             else if (f.op_type == 11)
//                 arr[cell] = sum / count;
//             else if (f.op_type == 12)
//                 arr[cell] = sum;
//             else if (f.op_type == 13)
//                 arr[cell] = sqrt(stdevSquared / count);
//         }

//         else if (f.op_type == 14) // Handle SLEEP operation
//         {
//             int sleep_value;
//             if (f.op_info1 == cell) // SLEEP(CONSTANT)
//             {
//                 sleep_value = f.op_info2;
//             }
//             else // SLEEP(CELL)
//             {
//                 sleep_value = arr[f.op_info1]; // Referenced cell
//             }

//             if (sleep_value <= 0 || sleep_value == INT_MIN) // invalid value
//             {
//                 printf("Error: Invalid sleep value in cell %d\n", cell);
//                 arr[cell] = INT_MIN; // Propagate error
//                 continue;
//             }

//             sleep(sleep_value);      //perform sleep operation
//             arr[cell] = sleep_value; //update the cell value
//         }

//     }
//     // printf("Recalculating cell %d with op_type %d\n", arr[cell], f.op_type);

//     free(sortedCells);
// }

void Recalc(Graph *graph, int C, int *arr, int startCell,Formula *formulaArray)
{
    printf("\n--- Starting Recalculation from cell %d ---\n", startCell);

    int size, hasCycle;
    int *sortedCells = topoSortFromCell(graph, startCell, &size, &hasCycle);
    if (hasCycle)
    {
        printf("Error: Circular dependency detected. Command rejected.\n");
        free(sortedCells);
        return;
    }

    printf("Topologically sorted cells for recalculation: ");
    for (int i = 0; i < size; i++)
        printf("%d ", sortedCells[i]);
    printf("\n");

    for (int i = 0; i < size; i++)
    {
        printf("Resetting cell %d to 0\n", sortedCells[i]);
        arr[sortedCells[i]] = 0;
    }

    for (int i = 0; i < size; i++)
    {
        int cell = sortedCells[i];
        Formula f = formulaArray[cell];

        printf("\nRecalculating cell %d with formula: op_type=%d, op_info1=%d, op_info2=%d\n",
               cell, f.op_type, f.op_info1, f.op_info2);

        if (f.op_type == 0)
        {
            if (f.op_info1 == INT_MIN)
            {
                printf("  Error: Cell %d has an invalid constant value (INT_MIN)\n", cell);
                arr[cell] = INT_MIN; // Propagate error
            }
            else
            {
                arr[cell] = f.op_info1; // Assign valid value
                printf("  Assigned constant value to cell %d: %d\n", cell, arr[cell]);
            }
        }

        else if (f.op_type >= 1 && f.op_type <= 4) // Cell and constant case
        {
            int v1 = arr[f.op_info1];
            int v2 = f.op_info2;

            printf("  Performing operation: v1=%d, v2=%d\n", v1, v2);

            if (v1 == INT_MIN)
            {
                printf("  Error: Cell %d has invalid operand (v1 is INT_MIN)\n", f.op_info1);
                arr[cell] = INT_MIN; // Propagate error
                continue;
            }

            char op = (f.op_type == 1) ? '+' : (f.op_type == 2) ? '-'
                                           : (f.op_type == 3)   ? '*'
                                                                : '/';

            if (op == '/' && v2 == 0)
            {
                printf("  Error: Division by zero in cell %d\n", cell);
                arr[cell] = INT_MIN; // Propagate error
                continue;
            }

            arr[cell] = arithmetic_eval2(v1, v2, op); // Perform operation
            printf("  Result of operation for cell %d: %d\n", cell, arr[cell]);
        }

        else if (f.op_type >= 5 && f.op_type <= 8) // Cell and cell case
        {
            int v1 = arr[f.op_info1];
            int v2 = arr[f.op_info2];

            printf("  Performing operation between cells %d and %d: v1=%d, v2=%d\n",
                   f.op_info1, f.op_info2, v1, v2);

            if (f.op_type == 8 && v2 == 0)
            {
                printf("  Error: Division by zero in cell %d\n", cell);
                arr[cell] = INT_MIN; // Mark this cell as error
                continue;
            }

            if (v1 == INT_MIN || v2 == INT_MIN)
            {
                printf("  Error: One of the operands for cell %d is INT_MIN\n", cell);
                arr[cell] = INT_MIN; // Propagate error
                continue;
            }

            char op = (f.op_type == 5) ? '+' : (f.op_type == 6) ? '-'
                                           : (f.op_type == 7)   ? '*'
                                                                : '/';
            arr[cell] = arithmetic_eval2(v1, v2, op);
            printf("  Result of operation for cell %d: %d\n", cell, arr[cell]);
        }

        else if (f.op_type >= 9 && f.op_type <= 13) // Range operations
        {
            int startCell = f.op_info1;
            int endCell = f.op_info2;

            int startRow = startCell / C;
            int startCol = startCell % C;
            int endRow = endCell / C;
            int endCol = endCell % C;

            printf("  Performing range operation: startCell=%d, endCell=%d\n", startCell, endCell);

            int sum = 0, count = 0, stdevSquared = 0;
            int minVal = INT_MAX, maxVal = INT_MIN;
            int hasError = 0; // Track if any cell in the range has an error

            for (int row = startRow; row <= endRow; row++)
            {
                for (int col = startCol; col <= endCol; col++)
                {
                    int idx = row * C + col;
                    int val = arr[idx];

                    printf("    Cell %d value in range: %d\n", idx, val);

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
                printf("  Error: Range contains invalid values\n");
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

            printf("  Result of range operation for cell %d: %d\n", cell, arr[cell]);
        }

        else if (f.op_type == 14) // Handle SLEEP operation
        {
            int sleep_value = (f.op_info1 == cell) ? f.op_info2 : arr[f.op_info1];
            printf("  Performing SLEEP operation for cell %d with value %d\n", cell, sleep_value);

            if (sleep_value <= 0 || sleep_value == INT_MIN)
            {
                printf("  Error: Invalid sleep value in cell %d\n", cell);
                arr[cell] = INT_MIN; // Propagate error
                continue;
            }

            sleep(sleep_value);      // Perform sleep operation
            arr[cell] = sleep_value; // Update the cell value
            printf("  Sleep completed for cell %d\n", cell);
        }
    }

    printf("\n--- Recalculation Completed ---\n");
    printf("Final cell values after recalculation:\n");
    for (int i = 0; i < size; i++)
    {
        printf("Cell %d: %d\n", sortedCells[i], arr[sortedCells[i]]);
    }

    free(sortedCells);
}
