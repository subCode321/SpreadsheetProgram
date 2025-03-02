#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <unistd.h> // For sleep function
#include <limits.h>
#include "Graph.h"
#include "Functions.h"

extern int hasCycle;

int min2(int a, int b)
{
    return (a < b) ? a : b;
}

int max2(int a, int b)
{
    return (a > b) ? a : b;
}

int arithmetic_eval2(int v1, int v2, char op)
{
    switch (op)
    {
    case '+':
        return v1 + v2;
    case '-':
        return v1 - v2;
    case '*':
        return v1 * v2;
    case '/':
        return v1 / v2;
    default:
        return INT_MIN;
    }
}

void AddFormula(Graph *graph, int cell, int c1, int c2, int op_type, Formula *formulaArray)
{
    Formula newFormula;
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

// Create a Range node
Range *AddRange(int startCell, int endCell, int dependentCell)
{
    Range *new_range = (Range *)malloc(sizeof(Range));
    if (!new_range)
    {
        printf("Cannot allocate memory for range\n");
        return NULL;
    }

    new_range->startCell = startCell;
    new_range->endCell = endCell;
    new_range->dependentCell = dependentCell;
    new_range->next = NULL;
    return new_range;
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

    // Initialize range list
    graph->ranges_head = NULL;

    return graph;
}

// Add an edge to the linked list
Cell *Addedge(int cell1, Cell *head)
{
    // Fast path if list is empty
    if (head == NULL)
    {
        return Addcell(cell1);
    }

    // Fast path if cell1 should be at the beginning
    if (head->cell == cell1)
    {
        return head; // Cell already exists, no need to add
    }

    // Check if the cell already exists in the list
    Cell *current = head;
    while (current->next != NULL)
    {
        if (current->next->cell == cell1)
            return head; // Cell already exists, no need to add
        current = current->next;
    }

    // Add new cell at the end of the list for better cache locality
    Cell *new_cell = Addcell(cell1);
    if (new_cell == NULL)
        return head;

    current->next = new_cell;
    return head;
}

// Add a range to the range list
void AddRangeToGraph(Graph *graph, int startCell, int endCell, int dependentCell)
{
    Range *new_range = AddRange(startCell, endCell, dependentCell);
    if (!new_range)
        return;

    // Add to the beginning of the list for simplicity
    new_range->next = graph->ranges_head;
    graph->ranges_head = new_range;
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

// Delete range from the range list
void DeleteRangeFromGraph(Graph *graph, int dependentCell)
{
    Range *current = graph->ranges_head;
    Range *prev = NULL;

    while (current != NULL)
    {
        if (current->dependentCell == dependentCell)
        {
            // Remove this range
            if (prev == NULL)
            {
                // It's the head node
                graph->ranges_head = current->next;
                free(current);
                current = graph->ranges_head;
            }
            else
            {
                // Middle or end node
                prev->next = current->next;
                free(current);
                current = prev->next;
            }
        }
        else
        {
            prev = current;
            current = current->next;
        }
    }
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
        // For range operations, just delete the range entry
        DeleteRangeFromGraph(graph, cell);
    }
    else if (x.op_type == 14) // SLEEP operation
    {
        // If op_info1 is not the cell itself, it's a reference to another cell
        if (x.op_info1 != cell)
        {
            graph->adjLists_head[x.op_info1] = Deletecell(cell, graph->adjLists_head[x.op_info1]);
        }
        // If op_info1 is the cell itself, it's a constant sleep - no edge to delete
    }
    else if (x.op_type == 15) // CONSTANT/CELL operation
    {
        // Delete edge from the referenced cell (op_info2)
        graph->adjLists_head[x.op_info2] = Deletecell(cell, graph->adjLists_head[x.op_info2]);
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
        AddRangeToGraph(graph, startCell, endCell, cell);
    }
    // For operation 14 (SLEEP)
    else if (x.op_type == 14)
    {
        // If op_info1 is not the cell itself, it's a reference to another cell
        if (x.op_info1 != cell)
        {
            graph->adjLists_head[x.op_info1] = Addedge(cell, graph->adjLists_head[x.op_info1]);
        }
        // If op_info1 is the cell itself, it's a constant sleep - no edge needed
    }
    // For operation 15 (CONSTANT/CELL)
    else if (x.op_type == 15)
    {
        // Add edge from the referenced cell (op_info2)
        graph->adjLists_head[x.op_info2] = Addedge(cell, graph->adjLists_head[x.op_info2]);
    }

    return NULL;
} 
void getNodesFromList(Cell *head, int *nodes, int *count)
{
    Cell *current = head;
    while (current != NULL)
    {
        nodes[(*count)++] = current->cell;
        current = current->next;
    }
}

// Modified DFS function for topological sort to consider ranges
void dfs(Graph *graph, int cell, int *visited, int *onStack, int *result, int *resultIndex, int *hasCycle, int COLS)
{
    // Early exit if cycle already detected
    if (*hasCycle)
        return;

    // Mark the current node as visited and add to recursion stack
    visited[cell] = 1;
    onStack[cell] = 1;

    // Visit all adjacent vertices from direct dependencies
    Cell *current = graph->adjLists_head[cell];
    while (current != NULL && !(*hasCycle))
    {
        int dependent = current->cell;

        // If not visited, then recursively process it
        if (!visited[dependent])
        {
            dfs(graph, dependent, visited, onStack, result, resultIndex, hasCycle, COLS);
        }
        // If already in recursion stack, then there's a cycle
        else if (onStack[dependent])
        {
            *hasCycle = 1;
            return;
        }

        current = current->next;
    }

    // Check if this cell is part of any range and add dependencies
    Range *range = graph->ranges_head;
    while (range != NULL && !(*hasCycle))
    {
        int startCell = range->startCell;
        int endCell = range->endCell;
        int dependent = range->dependentCell;

        int startRow = startCell / COLS;
        int startCol = startCell % COLS;
        int endRow = endCell / COLS;
        int endCol = endCell % COLS;

        // Optimize by swapping if start > end
        if (startRow > endRow)
        {
            int temp = startRow;
            startRow = endRow;
            endRow = temp;
        }
        if (startCol > endCol)
        {
            int temp = startCol;
            startCol = endCol;
            endCol = temp;
        }

        int cellRow = cell / COLS;
        int cellCol = cell % COLS;

        // Check if the cell is within the range
        if (cellRow >= startRow && cellRow <= endRow &&
            cellCol >= startCol && cellCol <= endCol)
        {
            // If dependent is not visited, recursively process it
            if (!visited[dependent])
            {
                dfs(graph, dependent, visited, onStack, result, resultIndex, hasCycle, COLS);
            }
            // If already in recursion stack, then there's a cycle
            else if (onStack[dependent])
            {
                *hasCycle = 1;
                return;
            }
        }

        range = range->next;
    }

    // Remove from recursion stack and add to result
    onStack[cell] = 0;
    result[(*resultIndex)++] = cell;
}

// Replace BFS with DFS for topological sort
int *topoSortFromCell(Graph *graph, int startCell, int *size, int *hasCycle, int COLS)
{
    *size = 0;
    *hasCycle = 0;

    // Create arrays for storing result and tracking visited/on-stack
    int *result = (int *)malloc(NUM_CELLS * sizeof(int));
    int *visited = (int *)calloc(NUM_CELLS, sizeof(int));
    int *onStack = (int *)calloc(NUM_CELLS, sizeof(int));

    if (!result || !visited || !onStack)
    {
        // Handle memory allocation failure
        if (result)
            free(result);
        if (visited)
            free(visited);
        if (onStack)
            free(onStack);
        *hasCycle = 1; // Use hasCycle to indicate error
        return NULL;
    }

    int resultIndex = 0;

    // Perform DFS starting from the startCell
    dfs(graph, startCell, visited, onStack, result, &resultIndex, hasCycle, COLS);

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
    int *sortedCells = topoSortFromCell(graph, startCell, &size, &hasCycle, C);
    if (hasCycle)
    {
        // printf("Error: Circular dependency detected. Command rejected.\n");
        free(sortedCells);
        return;
    }

    // Initialize all affected cells to 0
    for (int i = 0; i < size; i++)
    {
        arr[sortedCells[i]] = 0;
    }

    // Process cells in topological order
    for (int i = 0; i < size; i++)
    {
        int cell = sortedCells[i];
        Formula f = formulaArray[cell];

        switch (f.op_type)
        {
        case 0: // CELL=CONSTANT
            arr[cell] = (f.op_info1 == INT_MIN) ? (printf("  Error: Cell %d has an invalid constant value (INT_MIN)\n", cell), INT_MIN) : f.op_info1;
            break;

        case 1: // CELL=CELL+CONSTANT
        case 2: // CELL=CELL-CONSTANT
        case 3: // CELL=CELL*CONSTANT
        case 4: // CELL=CELL/CONSTANT
        {
            int v1 = arr[f.op_info1];
            int v2 = f.op_info2;

            if (v1 == INT_MIN)
            {
                // printf("  Error: Cell %d has invalid operand (v1 is INT_MIN)\n", f.op_info1);
                arr[cell] = INT_MIN;
                continue;
            }

            char op = (f.op_type == 1) ? '+' : (f.op_type == 2) ? '-'
                                           : (f.op_type == 3)   ? '*'
                                                                : '/';

            if (op == '/' && v2 == 0)
            {
                arr[cell] = INT_MIN;
                continue;
            }

            arr[cell] = arithmetic_eval2(v1, v2, op);
            break;
        }

        case 5: // CELL=CELL+CELL
        case 6: // CELL=CELL-CELL
        case 7: // CELL=CELL*CELL
        case 8: // CELL=CELL/CELL
        {
            int v1 = arr[f.op_info1];
            int v2 = arr[f.op_info2];

            if (v1 == INT_MIN || v2 == INT_MIN)
            {
                printf("  Error: One of the operands for cell %d is INT_MIN\n", cell);
                arr[cell] = INT_MIN;
                continue;
            }

            if (f.op_type == 8 && v2 == 0)
            {
                arr[cell] = INT_MIN;
                continue;
            }

            char op = (f.op_type == 5) ? '+' : (f.op_type == 6) ? '-'
                                           : (f.op_type == 7)   ? '*'
                                                                : '/';
            arr[cell] = arithmetic_eval2(v1, v2, op);
            break;
        }

        case 9:  // CELL=MIN(RANGE)
        case 10: // CELL=MAX(RANGE)
        case 11: // CELL=AVG(RANGE)
        case 12: // CELL=SUM(RANGE)
        case 13: // CELL=STDEV(RANGE)
        {
            int startCell = f.op_info1;
            int endCell = f.op_info2;

            int startRow = startCell / C;
            int startCol = startCell % C;
            int endRow = endCell / C;
            int endCol = endCell % C;

            // Optimize by swapping if start > end
            if (startRow > endRow)
            {
                int temp = startRow;
                startRow = endRow;
                endRow = temp;
            }
            if (startCol > endCol)
            {
                int temp = startCol;
                startCol = endCol;
                endCol = temp;
            }

            int sum = 0, count = 0;
            int minVal = INT_MAX, maxVal = INT_MIN;
            int hasError = 0;

            // First pass: Calculate sum, min, max and check for errors
            for (int row = startRow; row <= endRow && !hasError; row++)
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
                    minVal = (val < minVal) ? val : minVal;
                    maxVal = (val > maxVal) ? val : maxVal;
                }
            }

            if (hasError || count == 0)
            {
                arr[cell] = INT_MIN;
                continue;
            }

            // Handle different range operations
            switch (f.op_type)
            {
            case 9: // MIN
                arr[cell] = minVal;
                break;
            case 10: // MAX
                arr[cell] = maxVal;
                break;
            case 11: // AVG
                arr[cell] = sum / count;
                break;
            case 12: // SUM
                arr[cell] = sum;
                break;
            case 13: // STDEV
            {
                int values_count = (endRow - startRow + 1) * (endCol - startCol + 1);
                int *values = (int *)malloc(values_count * sizeof(int));

                int index = 0;
                for (int row = startRow; row <= endRow; row++)
                {
                    for (int col = startCol; col <= endCol; col++)
                    {
                        values[index++] = arr[row * C + col];
                    }
                }

                // Compute standard deviation using the std function
                arr[cell] = std(values, values_count);

                free(values);
                break;
            }
            }
            break;
        }

        case 14: // CELL=SLEEP(CONSTANT) or CELL=SLEEP(CELL)
        {
            int sleep_value = f.op_info2; // Assuming it's a constant

            // If op_info1 is not the cell itself, it's a reference to another cell
            if (f.op_info1 != cell)
            {
                sleep_value = arr[f.op_info1];
            }

            if (sleep_value == INT_MIN)
            {
                printf("  Error: Invalid sleep value in cell %d\n", cell);
                arr[cell] = INT_MIN;
                continue;
            }

            if (sleep_value > 0)
            {
                // printf("Sleeping for %d seconds...\n", sleep_value);
                sleep(sleep_value);
                // printf("Awake!\n");
            }

            arr[cell] = sleep_value;
            break;
        }
        case 15: //CELL=CONSTANT/CELL
        {
            int v1 = f.op_info1;
            int v2 = arr[f.op_info2];

            if (v2 == INT_MIN)
            {
                // printf("  Error: Cell %d has invalid operand (v1 is INT_MIN)\n", f.op_info1);
                arr[cell] = INT_MIN;
                continue;
            }

            char op = (f.op_type == 1) ? '+' : (f.op_type == 2) ? '-'
                                           : (f.op_type == 3)   ? '*'
                                                                : '/';

            if (op == '/' && v2 == 0)
            {
                arr[cell] = INT_MIN;
                continue;
            }

            arr[cell] = arithmetic_eval2(v1, v2, op);
            break;
        }


        default:
            // Handle unknown operation type
            arr[cell] = INT_MIN;
            printf("  Error: Unknown operation type %d for cell %d\n", f.op_type, cell);
            break;
        }
    }

    free(sortedCells);
}

void FreeGraph(Graph *graph)
{
    if (!graph)
        return;

    // Free cell adjacency lists
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

    // Free range list
    Range *current = graph->ranges_head;
    while (current)
    {
        Range *temp = current;
        current = current->next;
        free(temp);
    }

    free(graph->adjLists_head);
    free(graph);
}