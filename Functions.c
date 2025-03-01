#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "Parser.h"
#include "Graph.h"
#include <math.h>
#include <unistd.h> // For sleep function
#include <limits.h>

// Declare the external variable.
int invalidRange = 0;

// Helper function that sets invalidRange to 1 and returns -1.
int errorReturn()
{
    invalidRange = 1;
    return -1;
}

int validate_range(int range_start, int range_end, int C)
{
    int start_row = range_start / C;
    int start_col = range_start % C;
    int end_row = range_end / C;
    int end_col = range_end % C;

    if (start_row > end_row || (start_row == end_row && start_col > end_col))
    {
        // Invalid range: end cell comes before start cell
        return 0;
    }
    return 1;
}

int arithmetic_eval(int v1, int v2, char op)
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
        if (v2 == 0)
        {
            // Division by zero error.
            return INT_MIN; // Signal an error (not using -1 here)
        }
        return v1 / v2;
    }
    return INT_MIN; // Invalid operation
}

int return_optype(char op)
{
    if (op == '+')
    {
        return 1;
    }
    else if (op == '-')
    {
        return 2;
    }
    else if (op == '*')
    {
        return 3;
    }
    else if (op == '/')
    {
        return 4;
    }
    return INT_MIN;
}

int std(int *arr, int n)
{
    if (n <= 1)
        return 0; // Avoid division by zero

    int sum = 0;
    double variance = 0.0;

    // Calculate mean
    for (int i = 0; i < n; i++)
    {
        sum += arr[i];
    }
    int mean = sum / n;

    // Calculate variance
    for (int i = 0; i < n; i++)
    {
        variance += (arr[i] - mean) * (arr[i] - mean);
    }
    variance /= n;

    // Return integer standard deviation (rounded)
    return (int)round(sqrt(variance));
}

int min_func(char *a, int C, int R, int pos_equalto, int pos_end, int *arr, Graph *graph, Formula *formulaArray)
{
    int first_cell = cell_parser(a, C, R, 0, pos_equalto - 1, graph);
    if (first_cell == -1)
    {
        return errorReturn();
    }

    char *open_paren = strchr(a + pos_equalto, '(');
    char *close_paren = strchr(a + pos_equalto, ')');
    if (!open_paren || !close_paren || close_paren <= open_paren + 1)
    {
        return errorReturn();
    }

    char *colon_pos = strchr(open_paren + 1, ':');
    if (!colon_pos)
    {
        return errorReturn();
    }

    int range_start = cell_parser(a, C, R, open_paren - a + 1, colon_pos - a - 1, graph);
    int range_end = cell_parser(a, C, R, colon_pos - a + 1, close_paren - a - 1, graph);
    if (range_start == -1 || range_end == -1 || !validate_range(range_start, range_end, C))
    {
        return errorReturn();
    }
    AddFormula(graph, first_cell, range_start, range_end, 9, formulaArray);

    // Add the range to the graph
    AddRangeToGraph(graph, range_start, range_end, first_cell);

    int min_value = arr[range_start];

    int start_row = range_start / C;
    int start_col = range_start % C;
    int end_row = range_end / C;
    int end_col = range_end % C;

    if (start_row == end_row) // 1D range (same row)
    {
        for (int idx = range_start; idx <= range_end; idx++)
        {
            if (arr[idx] < min_value)
            {
                min_value = arr[idx];
            }
        }
    }
    else // 2D range
    {
        for (int row = start_row; row <= end_row; row++)
        {
            for (int col = start_col; col <= end_col; col++)
            {
                int idx = row * C + col;
                if (arr[idx] < min_value)
                {
                    min_value = arr[idx];
                }
            }
        }
    }
    arr[first_cell] = min_value;
    return 1;
}

int maxfunc(char *a, int C, int R, int pos_equalto, int pos_end, int *arr, Graph *graph, Formula *formulaArray)
{
    int first_cell = cell_parser(a, C, R, 0, pos_equalto - 1, graph);
    if (first_cell == -1)
    {
        return errorReturn();
    }

    char *open_paren = strchr(a + pos_equalto, '(');
    char *close_paren = strchr(a + pos_equalto, ')');
    if (!open_paren || !close_paren || close_paren <= open_paren + 1)
    {
        return errorReturn();
    }

    char *colon_pos = strchr(open_paren + 1, ':');
    if (!colon_pos)
    {
        return errorReturn();
    }

    int range_start = cell_parser(a, C, R, open_paren - a + 1, colon_pos - a - 1, graph);
    int range_end = cell_parser(a, C, R, colon_pos - a + 1, close_paren - a - 1, graph);
    if (range_start == -1 || range_end == -1 || !validate_range(range_start, range_end, C))
    {
        return errorReturn();
    }
    AddFormula(graph, first_cell, range_start, range_end, 10, formulaArray);

    // Add the range to the graph
    AddRangeToGraph(graph, range_start, range_end, first_cell);

    int max_value = arr[range_start];

    int start_row = range_start / C;
    int start_col = range_start % C;
    int end_row = range_end / C;
    int end_col = range_end % C;

    if (start_row == end_row)
    {
        for (int idx = range_start; idx <= range_end; idx++)
        {
            if (arr[idx] > max_value)
            {
                max_value = arr[idx];
            }
        }
    }
    else
    {
        for (int row = start_row; row <= end_row; row++)
        {
            for (int col = start_col; col <= end_col; col++)
            {
                int idx = row * C + col;
                if (arr[idx] > max_value)
                {
                    max_value = arr[idx];
                }
            }
        }
    }
    arr[first_cell] = max_value;
    return 1;
}

int avg_func(char *a, int C, int R, int pos_equalto, int pos_end, int *arr, Graph *graph, Formula *formulaArray)
{
    int first_cell = cell_parser(a, C, R, 0, pos_equalto - 1, graph);
    if (first_cell == -1)
    {
        return errorReturn();
    }

    char *open_paren = strchr(a + pos_equalto, '(');
    char *close_paren = strchr(a + pos_equalto, ')');

    if (!open_paren || !close_paren || close_paren <= open_paren + 1)
    {
        return errorReturn();
    }

    char *colon_pos = strchr(open_paren + 1, ':');
    if (!colon_pos)
    {
        return errorReturn();
    }

    int range_start = cell_parser(a, C, R, open_paren - a + 1, colon_pos - a - 1, graph);
    int range_end = cell_parser(a, C, R, colon_pos - a + 1, close_paren - a - 1, graph);
    if (range_start == -1 || range_end == -1 || !validate_range(range_start, range_end, C))
    {
        return errorReturn();
    }
    AddFormula(graph, first_cell, range_start, range_end, 11, formulaArray);

    // Add the range to the graph
    AddRangeToGraph(graph, range_start, range_end, first_cell);

    int start_row = range_start / C;
    int start_col = range_start % C;
    int end_row = range_end / C;
    int end_col = range_end % C;

    int sum = 0, count = 0;

    for (int row = start_row; row <= end_row; row++)
    {
        for (int col = start_col; col <= end_col; col++)
        {
            int idx = row * C + col;
            sum += arr[idx];
            count++;
        }
    }

    int avg_value = (count > 0) ? (sum / count) : 0;
    arr[first_cell] = avg_value;
    return 1;
}

int sum_func(char *a, int C, int R, int pos_equalto, int pos_end, int *arr, Graph *graph, Formula *formulaArray)
{
    int first_cell = cell_parser(a, C, R, 0, pos_equalto - 1, graph);
    if (first_cell == -1)
    {
        return errorReturn();
    }

    // Parse the range
    char *open_paren = strchr(a + pos_equalto, '(');
    char *close_paren = strchr(a + pos_equalto, ')');
    if (!open_paren || !close_paren || close_paren <= open_paren + 1)
    {
        return errorReturn();
    }

    char *colon_pos = strchr(open_paren + 1, ':');
    if (!colon_pos)
    {
        return errorReturn();
    }

    int range_start = cell_parser(a, C, R, open_paren - a + 1, colon_pos - a - 1, graph);
    int range_end = cell_parser(a, C, R, colon_pos - a + 1, close_paren - a - 1, graph);
    if (range_start == -1 || range_end == -1 || !validate_range(range_start, range_end, C))
    {
        return errorReturn();
    }

    AddFormula(graph, first_cell, range_start, range_end, 12, formulaArray);

    // Add the range to the graph
    AddRangeToGraph(graph, range_start, range_end, first_cell);

    int start_row = range_start / C;
    int start_col = range_start % C;
    int end_row = range_end / C;
    int end_col = range_end % C;
    int sum = 0;

    for (int row = start_row; row <= end_row; row++)
    {
        for (int col = start_col; col <= end_col; col++)
        {
            int idx = row * C + col;
            sum += arr[idx];
        }
    }
    arr[first_cell] = sum;
    return 1;
}

int stdev_func(char *a, int C, int R, int pos_equalto, int pos_end, int *arr, Graph *graph, Formula *formulaArray)
{
    int first_cell = cell_parser(a, C, R, 0, pos_equalto - 1, graph);
    if (first_cell == -1)
    {
        return errorReturn();
    }

    // Parse the range
    char *open_paren = strchr(a + pos_equalto, '(');
    char *close_paren = strchr(a + pos_equalto, ')');
    if (!open_paren || !close_paren || close_paren <= open_paren + 1)
    {
        return errorReturn();
    }

    char *colon_pos = strchr(open_paren + 1, ':');
    if (!colon_pos)
    {
        return errorReturn();
    }

    int range_start = cell_parser(a, C, R, open_paren - a + 1, colon_pos - a - 1, graph);
    int range_end = cell_parser(a, C, R, colon_pos - a + 1, close_paren - a - 1, graph);
    if (range_start == -1 || range_end == -1 || !validate_range(range_start, range_end, C))
    {
        return errorReturn();
    }

    AddFormula(graph, first_cell, range_start, range_end, 13, formulaArray);
    AddRangeToGraph(graph, range_start, range_end, first_cell);

    int start_row = range_start / C;
    int start_col = range_start % C;
    int end_row = range_end / C;
    int end_col = range_end % C;

    int values_count = (end_row - start_row + 1) * (end_col - start_col + 1);
    int *values = (int *)malloc(values_count * sizeof(int));
    if (!values)
    {
        return errorReturn(); // Memory allocation failed
    }

    int index = 0;
    for (int row = start_row; row <= end_row; row++)
    {
        for (int col = start_col; col <= end_col; col++)
        {
            values[index++] = arr[row * C + col];
        }
    }

    // Compute standard deviation using the std function
    arr[first_cell] = std(values, values_count);

    free(values);
    return 1;
}

int sleep_func(char *a, int C, int R, int pos_equalto, int pos_end, int *arr, Graph *graph, Formula *formulaArray)
{
    int target_cell = cell_parser(a, C, R, 0, pos_equalto - 1, graph);
    if (target_cell == -1)
    {
        return errorReturn();
    }

    char *open_paren = strchr(a + pos_equalto, '(');
    char *close_paren = strchr(a + pos_equalto, ')');
    if (!open_paren || !close_paren || close_paren <= open_paren + 1)
    {
        return errorReturn();
    }

    int sleep_value = -1;
    int ref_cell = cell_parser(a, C, R, open_paren - a + 1, close_paren - a - 1, graph);

    // Determine whether it's a cell reference or a direct value
    if (ref_cell != -1)
    {
        sleep_value = arr[ref_cell];
        if (sleep_value == INT_MIN)
        {
            printf("Referenced cell %d contains an error value\n", ref_cell);
            arr[target_cell] = INT_MIN; // Propagate error
            return 1;
        }
        graph->adjLists_head[ref_cell] = Addedge(target_cell, graph->adjLists_head[ref_cell]);
    }
    else
    {
        char *end_ptr;
        sleep_value = strtol(open_paren + 1, &end_ptr, 10);
        if (*end_ptr != ')' || sleep_value <= 0)
        {
            arr[target_cell] = sleep_value;
            AddFormula(graph, target_cell, ref_cell != -1 ? ref_cell : target_cell, sleep_value, 14, formulaArray);
            return 1;
        }
    }

    if (sleep_value <= 0)
    {
        arr[target_cell] = sleep_value;
        AddFormula(graph, target_cell, ref_cell != -1 ? ref_cell : target_cell, sleep_value, 14, formulaArray);
        return 1;
    }

    // Add formula and execute sleep
    AddFormula(graph, target_cell, ref_cell != -1 ? ref_cell : target_cell, sleep_value, 14, formulaArray);
    arr[target_cell] = sleep_value;
    return 1;
}
