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
    Formula newFormula = {op_type, c1, (op_type == 0) ? -1 : c2};
    formulaArray[cell] = newFormula;
}

// Optimized to return NULL on failure
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

    graph->adjLists_head = (Cell **)calloc(NUM_CELLS, sizeof(Cell *));
    if (!graph->adjLists_head)
    {
        printf("Cannot allocate memory for adjacency lists\n");
        free(graph);
        return NULL;
    }

    return graph;
}

// Optimized edge addition to prevent unnecessary traversal
Cell *Addedge(int cell1, Cell *head)
{
    for (Cell *current = head; current; current = current->next)
        if (current->cell == cell1)
            return head; // Cell already exists, no need to add

    Cell *new_cell = Addcell(cell1);
    if (!new_cell)
        return head;

    new_cell->next = head;
    return new_cell;
}

// Optimized deletion with early exits
Cell *Deletecell(int cell1, Cell *head)
{
    if (!head)
        return NULL;

    if (head->cell == cell1)
    {
        Cell *nextNode = head->next;
        free(head);
        return nextNode;
    }

    for (Cell *prev = head, *current = head->next; current; prev = current, current = current->next)
    {
        if (current->cell == cell1)
        {
            prev->next = current->next;
            free(current);
            break;
        }
    }

    return head;
}

// Optimized Deleteedge to avoid redundant calls
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
        int startRow = x.op_info1 / COLS, startCol = x.op_info1 % COLS;
        int endRow = x.op_info2 / COLS, endCol = x.op_info2 % COLS;

        for (int row = startRow; row <= endRow; ++row)
            for (int col = startCol; col <= endCol; ++col)
                graph->adjLists_head[row * COLS + col] = Deletecell(cell, graph->adjLists_head[row * COLS + col]);
    }
    return NULL;
}

// Optimized Addedge_formula with reduced function calls
Cell *Addedge_formula(Graph *graph, int cell, int COLS, Formula *formulaArray)
{
    Formula x = formulaArray[cell];

    if (x.op_type >= 1 && x.op_type <= 4)
    {
        graph->adjLists_head[x.op_info1] = Addedge(cell, graph->adjLists_head[x.op_info1]);
    }
    else if (x.op_type >= 5 && x.op_type <= 8)
    {
        graph->adjLists_head[x.op_info1] = Addedge(cell, graph->adjLists_head[x.op_info1]);
        graph->adjLists_head[x.op_info2] = Addedge(cell, graph->adjLists_head[x.op_info2]);
    }
    else if (x.op_type >= 9 && x.op_type <= 13)
    {
        int startRow = x.op_info1 / COLS, startCol = x.op_info1 % COLS;
        int endRow = x.op_info2 / COLS, endCol = x.op_info2 % COLS;

        for (int row = startRow; row <= endRow; ++row)
            for (int col = startCol; col <= endCol; ++col)
                graph->adjLists_head[row * COLS + col] = Addedge(cell, graph->adjLists_head[row * COLS + col]);
    }
    return NULL;
}

// Get nodes from the linked list
void getNodesFromList(Cell *head, int *nodes, int *count)
{
    while (head)
    {
        nodes[(*count)++] = head->cell;
        head = head->next;
    }
}

void dfs(Graph *graph, int cell, int *visited, int *onStack, int *result, int *resultIndex, int *hasCycleLocal)
{
    visited[cell] = onStack[cell] = 1;

    for (Cell *current = graph->adjLists_head[cell]; current; current = current->next)
    {
        int dependent = current->cell;
        if (!visited[dependent])
        {
            dfs(graph, dependent, visited, onStack, result, resultIndex, hasCycleLocal);
            if (*hasCycleLocal)
            {
                hasCycle = 1; // Update extern variable
                return;
            }
        }
        else if (onStack[dependent])
        {
            *hasCycleLocal = 1;
            hasCycle = 1; // Update extern variable
            return;
        }
    }

    onStack[cell] = 0;
    result[(*resultIndex)++] = cell;
}
int *topoSortFromCell(Graph *graph, int startCell, int *size, int *hasCycleLocal)
{
    *size = 0;
    *hasCycleLocal = 0;
    hasCycle = 0; // Reset the extern variable

    int *result = (int *)malloc(NUM_CELLS * sizeof(int));
    if (!result)
        return NULL;

    int *visited = (int *)calloc(NUM_CELLS, sizeof(int));
    int *onStack = (int *)calloc(NUM_CELLS, sizeof(int));
    if (!visited || !onStack)
    {
        free(result);
        free(visited);
        free(onStack);
        return NULL;
    }

    int resultIndex = 0;
    dfs(graph, startCell, visited, onStack, result, &resultIndex, hasCycleLocal);

    free(visited);
    free(onStack);

    if (*hasCycleLocal)
    {
        free(result);
        return NULL;
    }

    // Reverse result (in-place swap)
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
    int size, hasCycle;
    int *sortedCells = topoSortFromCell(graph, startCell, &size, &hasCycle);
    if (!sortedCells)
    {
        hasCycle = 1;
        printf("Error: Circular dependency detected. Command rejected.\n");
        return;
    }

    for (int i = 0; i < size; i++)
        arr[sortedCells[i]] = 0;

    for (int i = 0; i < size; i++)
    {
        int cell = sortedCells[i];
        Formula f = formulaArray[cell];

        if (f.op_type == 0)
        {
            arr[cell] = (f.op_info1 == INT_MIN) ? INT_MIN : f.op_info1;
            continue;
        }

        int v1 = arr[f.op_info1];
        int v2 = f.op_info2;

        if ((f.op_type >= 1 && f.op_type <= 4) || (f.op_type >= 5 && f.op_type <= 8))
        {
            if (v1 == INT_MIN || (f.op_type >= 5 && f.op_info2 != -1 && v2 == INT_MIN))
            {
                arr[cell] = INT_MIN;
                continue;
            }

            char op;
            switch (f.op_type)
            {
            case 1:
            case 5:
                op = '+';
                break;
            case 2:
            case 6:
                op = '-';
                break;
            case 3:
            case 7:
                op = '*';
                break;
            case 4:
            case 8:
                if (v2 == 0)
                {
                    arr[cell] = INT_MIN;
                    continue;
                }
                op = '/';
                break;
            default:
                continue;
            }

            arr[cell] = arithmetic_eval2(v1, v2, op);
            continue;
        }

        if (f.op_type >= 9 && f.op_type <= 13)
        {
            int sum = 0, count = 0, stdevSquared = 0;
            int minVal = INT_MAX, maxVal = INT_MIN;
            int hasError = 0;

            for (int idx = f.op_info1; idx <= f.op_info2; idx++)
            {
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
            {
                arr[cell] = INT_MIN;
                continue;
            }

            double mean = (double)sum / count;
            for (int idx = f.op_info1; idx <= f.op_info2; idx++)
                stdevSquared += (arr[idx] - mean) * (arr[idx] - mean);

            switch (f.op_type)
            {
            case 9:
                arr[cell] = minVal;
                break;
            case 10:
                arr[cell] = maxVal;
                break;
            case 11:
                arr[cell] = sum / count;
                break;
            case 12:
                arr[cell] = sum;
                break;
            case 13:
                arr[cell] = sqrt(stdevSquared / count);
                break;
            }
            continue;
        }

        if (f.op_type == 14)
        {
            int sleep_value = (f.op_info1 == cell) ? f.op_info2 : arr[f.op_info1];

            if (sleep_value == INT_MIN || sleep_value <= 0)
            {
                arr[cell] = INT_MIN;
                continue;
            }

            sleep(sleep_value);
            arr[cell] = sleep_value;
        }
    }

    free(sortedCells);
}

void FreeGraph(Graph *graph)
{
    if (!graph)
        return;

    for (int i = 0; i < NUM_CELLS; i++)
    {
        Cell *current = graph->adjLists_head[i];
        while (current)
        {
            Cell *temp = current;
            current = current->next;
            free(temp);
        }
    }

    free(graph->adjLists_head);
    free(graph);
}
