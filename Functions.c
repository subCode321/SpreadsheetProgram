#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "Parser.h"
#include "Graph.h"
#include <math.h>
#include <unistd.h> // For sleep function
#include <limits.h>

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
            printf("Error: Division by zero\n");
            return INT_MIN; // Signal an error
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

void min_func(char *a, int C, int R, int pos_equalto, int pos_end, int *arr, Graph *graph)
{
    int first_cell;

    first_cell = cell_parser(a, C, R, 0, pos_equalto - 1, graph);
    if (first_cell == -1)
    {
        printf("Invalid destination cell\n");
        return;
    }

    char *open_paren = strchr(a + pos_equalto, '(');
    char *close_paren = strchr(a + pos_equalto, ')');
    if (!open_paren || !close_paren || close_paren <= open_paren + 1)
    {
        printf("Invalid range: Missing or misplaced parentheses\n");
        return;
    }

    char *colon_pos = strchr(open_paren + 1, ':');
    if (!colon_pos)
    {
        printf("Invalid range: Missing ':'\n");
        return;
    }

    int range_start = cell_parser(a, C, R, open_paren - a + 1, colon_pos - a - 1, graph);
    int range_end = cell_parser(a, C, R, colon_pos - a + 1, close_paren - a - 1, graph);
    if (range_start == -1 || range_end == -1 || !validate_range(range_start, range_end, C))
    {
        printf("Invalid range\n");
        return;
    }
    AddFormula(graph, Addcell(first_cell), range_start, range_end, 9);

    int min_value = arr[range_start];

    int start_row = range_start / C;
    int start_col = range_start % C;
    int end_row = range_end / C;
    int end_col = range_end % C;

    if (start_row == end_row) // 1D range (same row)
    {
        for (int idx = range_start; idx <= range_end; idx++)
        {
            graph->adjLists_head[idx] = Addedge(first_cell, graph->adjLists_head[idx]);

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
            int col_start = start_col;
            int col_end = end_col;

            for (int col = col_start; col <= col_end; col++)
            {
                int idx = row * C + col;
                graph->adjLists_head[idx] = Addedge(first_cell, graph->adjLists_head[idx]);

                if (arr[idx] < min_value)
                {
                    min_value = arr[idx];
                }
            }
        }
    }

    arr[first_cell] = min_value;
}

void maxfunc(char *a, int C, int R, int pos_equalto, int pos_end, int *arr, Graph *graph)
{
    int first_cell;

    first_cell = cell_parser(a, C, R, 0, pos_equalto - 1, graph);
    if (first_cell == -1)
    {
        printf("Invalid destination cell\n");
        return;
    }

    char *open_paren = strchr(a + pos_equalto, '(');
    char *close_paren = strchr(a + pos_equalto, ')');
    if (!open_paren || !close_paren || close_paren <= open_paren + 1)
    {
        printf("Invalid range: Missing or misplaced parentheses\n");
        return;
    }

    char *colon_pos = strchr(open_paren + 1, ':');
    if (!colon_pos)
    {
        printf("Invalid range: Missing ':'\n");
        return;
    }

    int range_start = cell_parser(a, C, R, open_paren - a + 1, colon_pos - a - 1, graph);
    int range_end = cell_parser(a, C, R, colon_pos - a + 1, close_paren - a - 1, graph);
    if (range_start == -1 || range_end == -1 || !validate_range(range_start, range_end, C))
    {
        printf("Invalid range\n");
        return;
    }
    AddFormula(graph, Addcell(first_cell), range_start, range_end, 10);

    int max_value = arr[range_start];

    int start_row = range_start / C;
    int start_col = range_start % C;
    int end_row = range_end / C;
    int end_col = range_end % C;

    if (start_row == end_row)
    {
        for (int idx = range_start; idx <= range_end; idx++)
        {
            graph->adjLists_head[idx] = Addedge(first_cell, graph->adjLists_head[idx]);

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
            int col_start = start_col;
            int col_end = end_col;

            for (int col = col_start; col <= col_end; col++)
            {
                int idx = row * C + col;
                graph->adjLists_head[idx] = Addedge(first_cell, graph->adjLists_head[idx]);

                if (arr[idx] > max_value)
                {
                    max_value = arr[idx];
                }
            }
        }
    }

    // Store the maximum value in the destination cell
    arr[first_cell] = max_value;
}

void avg_func(char *a, int C, int R, int pos_equalto, int pos_end, int *arr, Graph *graph)
{
    int first_cell = cell_parser(a, C, R, 0, pos_equalto - 1, graph);
    if (first_cell == -1)
    {
        printf("Invalid destination cell\n");
        return;
    }

    char *open_paren = strchr(a + pos_equalto, '(');
    char *close_paren = strchr(a + pos_equalto, ')');

    if (!open_paren || !close_paren || close_paren <= open_paren + 1)
    {
        printf("Invalid range: Missing or misplaced parentheses\n");
        return;
    }

    char *colon_pos = strchr(open_paren + 1, ':');
    if (!colon_pos)
    {
        printf("Invalid range: Missing ':'\n");
        return;
    }

    int range_start = cell_parser(a, C, R, open_paren - a + 1, colon_pos - a - 1, graph);
    int range_end = cell_parser(a, C, R, colon_pos - a + 1, close_paren - a - 1, graph);
    if (range_start == -1 || range_end == -1 || !validate_range(range_start, range_end, C))
    {
        printf("Invalid range\n");
        return;
    }
    AddFormula(graph, Addcell(first_cell), range_start, range_end, 11);

    int start_row = range_start / C;
    int start_col = range_start % C;
    int end_row = range_end / C;
    int end_col = range_end % C;

    int sum = 0, count = 0;

    for (int row = start_row; row <= end_row; row++)
    {
        int col_start = start_col;
        int col_end = end_col;

        for (int col = col_start; col <= col_end; col++)
        {
            int idx = row * C + col;
            graph->adjLists_head[idx] = Addedge(first_cell, graph->adjLists_head[idx]);
            sum += arr[idx];
            count++;
        }
    }

    int avg_value = (count > 0) ? (sum / count) : 0;
    arr[first_cell] = avg_value;
}

void sum_func(char *a, int C, int R, int pos_equalto, int pos_end, int *arr, Graph *graph)
{
    int first_cell = cell_parser(a, C, R, 0, pos_equalto - 1, graph);
    if (first_cell == -1)
    {
        printf("Invalid destination cell\n");
        return;
    }

    // Parse the range
    char *open_paren = strchr(a + pos_equalto, '(');
    char *close_paren = strchr(a + pos_equalto, ')');
    if (!open_paren || !close_paren || close_paren <= open_paren + 1)
    {
        printf("Invalid range: Missing or misplaced parentheses\n");
        return;
    }

    char *colon_pos = strchr(open_paren + 1, ':');
    if (!colon_pos)
    {
        printf("Invalid range: Missing ':'\n");
        return;
    }

    int range_start = cell_parser(a, C, R, open_paren - a + 1, colon_pos - a - 1, graph);
    int range_end = cell_parser(a, C, R, colon_pos - a + 1, close_paren - a - 1, graph);
    if (range_start == -1 || range_end == -1 || !validate_range(range_start, range_end, C))
    {
        printf("Invalid range\n");
        return;
    }

    AddFormula(graph, Addcell(first_cell), range_start, range_end, 12);

    int start_row = range_start / C;
    int start_col = range_start % C;
    int end_row = range_end / C;
    int end_col = range_end % C;

    // Calculate SUM
    int sum = 0;

    for (int row = start_row; row <= end_row; row++)
    {
        int col_start = start_col;
        int col_end = end_col;

        for (int col = col_start; col <= col_end; col++)
        {
            int idx = row * C + col;
            graph->adjLists_head[idx] = Addedge(first_cell, graph->adjLists_head[idx]);
            sum += arr[idx];
        }
    }

    arr[first_cell] = sum;
}

void stdev_func(char *a, int C, int R, int pos_equalto, int pos_end, int *arr, Graph *graph)
{
    int first_cell = cell_parser(a, C, R, 0, pos_equalto - 1, graph);
    if (first_cell == -1)
    {
        printf("Invalid destination cell\n");
        return;
    }

    // Parse the range
    char *open_paren = strchr(a + pos_equalto, '(');
    char *close_paren = strchr(a + pos_equalto, ')');
    if (!open_paren || !close_paren || close_paren <= open_paren + 1)
    {
        printf("Invalid range: Missing or misplaced parentheses\n");
        return;
    }

    char *colon_pos = strchr(open_paren + 1, ':');
    if (!colon_pos)
    {
        printf("Invalid range: Missing ':'\n");
        return;
    }

    int range_start = cell_parser(a, C, R, open_paren - a + 1, colon_pos - a - 1, graph);
    int range_end = cell_parser(a, C, R, colon_pos - a + 1, close_paren - a - 1, graph);
    if (range_start == -1 || range_end == -1 || !validate_range(range_start, range_end, C))
    {
        printf("Invalid range\n");
        return;
    }

    AddFormula(graph, Addcell(first_cell), range_start, range_end, 13);

    int start_row = range_start / C;
    int start_col = range_start % C;
    int end_row = range_end / C;
    int end_col = range_end % C;

    int sum = 0;
    int count = 0;

    for (int row = start_row; row <= end_row; row++)
    {
        int col_start = start_col;
        int col_end = end_col;

        for (int col = col_start; col <= col_end; col++)
        {
            int idx = row * C + col;
            graph->adjLists_head[idx] = Addedge(first_cell, graph->adjLists_head[idx]);

            sum += arr[idx];
            count++;
        }
    }
    int avg = sum / count;
    int stdev_squared = 0;
    for (int row = start_row; row <= end_row; row++)
    {
        int col_start = start_col;
        int col_end = end_col;

        for (int col = col_start; col <= col_end; col++)
        {
            int idx = row * C + col;
            int prod = (arr[idx] - avg) * (arr[idx] - avg);
            stdev_squared += prod;
        }
    }
    int stdev = sqrt(stdev_squared / count);
    arr[first_cell] = stdev;
}

void sleep_func(char *a, int C, int R, int pos_equalto, int pos_end, int *arr, Graph *graph)
{
    int target_cell = cell_parser(a, C, R, 0, pos_equalto - 1, graph);
    if (target_cell == -1)
    {
        printf("Invalid destination cell\n");
        return;
    }

    char *open_paren = strchr(a + pos_equalto, '(');
    char *close_paren = strchr(a + pos_equalto, ')');
    if (!open_paren || !close_paren || close_paren <= open_paren + 1)
    {
        printf("Invalid SLEEP syntax: Missing or misplaced parentheses\n");
        return;
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
            return;
        }
        graph->adjLists_head[ref_cell] = Addedge(target_cell, graph->adjLists_head[ref_cell]);
    }
    else
    {
        char *end_ptr;
        sleep_value = strtol(open_paren + 1, &end_ptr, 10);
        if (*end_ptr != ')' || sleep_value <= 0)
        {
            printf("SLEEP value must evaluate to a positive integer\n");
            arr[target_cell] = INT_MIN; // Propagate error
            return;
        }
    }

    if (sleep_value <= 0)
    {
        printf("SLEEP value must evaluate to a positive integer\n");
        arr[target_cell] = INT_MIN; // Propagate error
        return;
    }

    printf("Parsed sleep value: %d\n", sleep_value);

    // Add formula and execute sleep
    AddFormula(graph, Addcell(target_cell), ref_cell != -1 ? ref_cell : target_cell, sleep_value, 14);

    arr[target_cell] = sleep_value;
}
