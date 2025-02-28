#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <unistd.h> // For sleep function
#include <limits.h>
#include "Graph.h"

extern int hasCycle;

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

void AddFormula(Graph *graph, int cell, int c1, int c2, int op_type, Formula *formulaArray)
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
    formulaArray[cell] = newFormula;
}

// Modified to use linked list instead of AVL tree
Cell *Addcell(int cell)
{
    Cell *new_cell = (Cell *)malloc(sizeof(Cell));
    if (!new_cell)
    {
        printf("Cannot allocate memory for %d\n", cell);
        return NULL;
    }

    new_cell->cell = cell;
    new_cell->next = NULL;
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

// Add an edge to the linked list
Cell *Addedge(int cell1, Cell *head)
{
    // Check if the cell already exists in the list
    Cell *current = head;
    while (current != NULL)
    {
        if (current->cell == cell1)
            return head; // Cell already exists, no need to add
        current = current->next;
    }

    // Add new cell at the beginning of the list
    Cell *new_cell = Addcell(cell1);
    if (new_cell == NULL)
        return head;

    new_cell->next = head;
    return new_cell;
}

// Delete a specific cell from the linked list
Cell *Deletecell(int cell1, Cell *head)
{
    if (head == NULL)
        return NULL;

    // If the head node itself holds the cell to be deleted
    if (head->cell == cell1)
    {
        Cell *temp = head->next;
        free(head);
        return temp;
    }

    // Search for the cell to be deleted
    Cell *current = head;
    Cell *prev = NULL;

    while (current != NULL && current->cell != cell1)
    {
        prev = current;
        current = current->next;
    }

    // If cell was not found
    if (current == NULL)
        return head;

    // Unlink the node from linked list
    prev->next = current->next;
    free(current);

    return head;
}

Cell *Deleteedge(Graph *graph, int cell, int COLS, Formula *formulaArray)
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

Cell *Addedge_formula(Graph *graph, int cell, int COLS, Formula *formulaArray)
{
    Formula x = formulaArray[cell];

    // For operations 1-4 (single cell operations)
    if (x.op_type >= 1 && x.op_type <= 4)
    {
        graph->adjLists_head[x.op_info1] = Addedge(cell, graph->adjLists_head[x.op_info1]);
    }
    // For operations 5-8 (two cell operations)
    else if (x.op_type >= 5 && x.op_type <= 8)
    {
        graph->adjLists_head[x.op_info1] = Addedge(cell, graph->adjLists_head[x.op_info1]);
        graph->adjLists_head[x.op_info2] = Addedge(cell, graph->adjLists_head[x.op_info2]);
    }
    // For operations 9-13 (range operations)
    else if (x.op_type >= 9 && x.op_type <= 13)
    {
        int startCell = x.op_info1;
        int endCell = x.op_info2;
        int startRow = startCell / COLS;
        int startCol = startCell % COLS;
        int endRow = endCell / COLS;
        int endCol = endCell % COLS;

        // Add edges for all cells in the range
        for (int row = startRow; row <= endRow; ++row)
        {
            for (int col = startCol; col <= endCol; ++col)
            {
                int targetCell = row * COLS + col;
                graph->adjLists_head[targetCell] = Addedge(cell, graph->adjLists_head[targetCell]);
            }
        }
    }
    return NULL;
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

// Get nodes from the linked list
void getNodesFromList(Cell *head, int *nodes, int *count)
{
    Cell *current = head;
    while (current != NULL)
    {
        nodes[(*count)++] = current->cell;
        current = current->next;
    }
}

// DFS function for topological sort
void dfs(Graph *graph, int cell, int *visited, int *onStack, int *result, int *resultIndex, int *hasCycle)
{
    // Mark the current node as visited and add to recursion stack
    visited[cell] = 1;
    onStack[cell] = 1;

    // Visit all adjacent vertices
    if (graph->adjLists_head[cell] != NULL)
    {
        Cell *current = graph->adjLists_head[cell];
        while (current != NULL)
        {
            int dependent = current->cell;

            // If not visited, then recursively process it
            if (!visited[dependent])
            {
                dfs(graph, dependent, visited, onStack, result, resultIndex, hasCycle);
                // If cycle was detected, return immediately
                if (*hasCycle)
                    return;
            }
            // If already in recursion stack, then there's a cycle
            else if (onStack[dependent])
            {
                *hasCycle = 1;
                return;
            }

            current = current->next;
        }
    }

    // Remove from recursion stack and add to result
    onStack[cell] = 0;
    result[(*resultIndex)++] = cell;
}

// Replace BFS with DFS for topological sort
int *topoSortFromCell(Graph *graph, int startCell, int *size, int *hasCycle)
{
    *size = 0;
    *hasCycle = 0;

    // Create arrays for storing result and tracking visited/on-stack
    int *result = (int *)malloc(NUM_CELLS * sizeof(int));
    int *visited = (int *)calloc(NUM_CELLS, sizeof(int));
    int *onStack = (int *)calloc(NUM_CELLS, sizeof(int));
    int resultIndex = 0;

    // Perform DFS starting from the startCell
    dfs(graph, startCell, visited, onStack, result, &resultIndex, hasCycle);

    free(visited);
    free(onStack);

    if (*hasCycle)
    {
        free(result);
        return NULL;
    }

    // Reverse the result (DFS produces reverse topological order)
    for (int i = 0; i < resultIndex / 2; i++)
    {
        int temp = result[i];
        result[i] = result[resultIndex - i - 1];
        result[resultIndex - i - 1] = temp;
    }

    *size = resultIndex;
    return result;
}

void Recalc(Graph *graph, int C, int *arr, int startCell, Formula *formulaArray)
{
    int size;
    int *sortedCells = topoSortFromCell(graph, startCell, &size, &hasCycle);
    if (hasCycle)
    {
        printf("Error: Circular dependency detected. Command rejected.\n");
        free(sortedCells);
        return;
    }

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
                printf("  Error: Cell %d has an invalid constant value (INT_MIN)\n", cell);
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
                printf("  Error: Cell %d has invalid operand (v1 is INT_MIN)\n", f.op_info1);
                arr[cell] = INT_MIN; // Propagate error
                continue;
            }

            char op = (f.op_type == 1) ? '+' : (f.op_type == 2) ? '-'
                                           : (f.op_type == 3)   ? '*'
                                                                : '/';

            if (op == '/' && v2 == 0)
            {
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

        else if (f.op_type == 14) // Handle SLEEP operation
        {
            int sleep_value = (f.op_info1 == cell) ? f.op_info2 : arr[f.op_info1];

            if (sleep_value == INT_MIN)
            {
                printf("  Error: Invalid sleep value in cell %d\n", cell);
                arr[cell] = INT_MIN; // Propagate error
                continue;
            }
            else if (sleep_value <= 0)
            {
                arr[cell] = sleep_value;
                continue;
            }

            sleep(sleep_value);      // Perform sleep operation
            arr[cell] = sleep_value; // Update the cell value
        }
    }
    free(sortedCells);
}