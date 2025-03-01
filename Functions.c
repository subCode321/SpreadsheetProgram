#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "Parser.h"
#include "Graph.h"
#include <math.h>
#include <unistd.h>
#include <limits.h>

int validate_range(int range_start, int range_end, int C)
{
    return (range_start <= range_end);
}

int arithmetic_eval(int v1, int v2, char op)
{
    if (op == '/' && v2 == 0)
        return INT_MIN; // Prevent division by zero
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

int return_optype(char op)
{
    switch (op)
    {
    case '+':
        return 1;
    case '-':
        return 2;
    case '*':
        return 3;
    case '/':
        return 4;
    default:
        return INT_MIN;
    }
}

void process_range(int range_start, int range_end, int C, Graph *graph, int first_cell, int *arr, int *result, int operation)
{
    *result = (operation == 9) ? INT_MAX : (operation == 10) ? INT_MIN
                                                             : 0;
    int count = 0;

    for (int idx = range_start; idx <= range_end; idx++)
    {
        graph->adjLists_head[idx] = Addedge(first_cell, graph->adjLists_head[idx]);
        if (operation == 9)
            *result = (*result > arr[idx]) ? arr[idx] : *result; // Min
        if (operation == 10)
            *result = (*result < arr[idx]) ? arr[idx] : *result; // Max
        if (operation == 11 || operation == 12)
            *result += arr[idx]; // Sum or Avg
        count++;
    }
    if (operation == 11)
        *result /= count; // Average
    if (operation == 13)
    { // Standard Deviation
        int mean = *result / count, sum_squares = 0;
        for (int idx = range_start; idx <= range_end; idx++)
            sum_squares += (arr[idx] - mean) * (arr[idx] - mean);
        *result = sqrt(sum_squares / count);
    }
}

void process_func(char *a, int C, int R, int pos_equalto, int operation, int *arr, Graph *graph, Formula *formulaArray)
{
    int first_cell = cell_parser(a, C, R, 0, pos_equalto - 1, graph);
    if (first_cell == -1)
        return;

    char *open_paren = strchr(a + pos_equalto, '(');
    char *close_paren = strchr(a + pos_equalto, ')');
    char *colon_pos = strchr(open_paren + 1, ':');
    if (!open_paren || !close_paren || close_paren <= open_paren + 1 || !colon_pos)
        return;

    int range_start = cell_parser(a, C, R, open_paren - a + 1, colon_pos - a - 1, graph);
    int range_end = cell_parser(a, C, R, colon_pos - a + 1, close_paren - a - 1, graph);
    if (range_start == -1 || range_end == -1 || !validate_range(range_start, range_end, C))
        return;

    AddFormula(graph, first_cell, range_start, range_end, operation, formulaArray);
    process_range(range_start, range_end, C, graph, first_cell, arr, &arr[first_cell], operation);
}

void min_func(char *a, int C, int R, int pos_equalto, int pos_end, int *arr, Graph *graph, Formula *formulaArray)
{
    process_func(a, C, R, pos_equalto, 9, arr, graph, formulaArray);
}

void maxfunc(char *a, int C, int R, int pos_equalto, int pos_end, int *arr, Graph *graph, Formula *formulaArray)
{
    process_func(a, C, R, pos_equalto, 10, arr, graph, formulaArray);
}

void avg_func(char *a, int C, int R, int pos_equalto, int pos_end, int *arr, Graph *graph, Formula *formulaArray)
{
    process_func(a, C, R, pos_equalto, 11, arr, graph, formulaArray);
}

void sum_func(char *a, int C, int R, int pos_equalto, int pos_end, int *arr, Graph *graph, Formula *formulaArray)
{
    process_func(a, C, R, pos_equalto, 12, arr, graph, formulaArray);
}

void stdev_func(char *a, int C, int R, int pos_equalto, int pos_end, int *arr, Graph *graph, Formula *formulaArray)
{
    process_func(a, C, R, pos_equalto, 13, arr, graph, formulaArray);
}

void sleep_func(char *a, int C, int R, int pos_equalto, int pos_end, int *arr, Graph *graph, Formula *formulaArray)
{
    int target_cell = cell_parser(a, C, R, 0, pos_equalto - 1, graph);
    if (target_cell == -1)
        return;

    char *open_paren = strchr(a + pos_equalto, '(');
    char *close_paren = strchr(a + pos_equalto, ')');
    if (!open_paren || !close_paren || close_paren <= open_paren + 1)
        return;

    int sleep_value = -1, ref_cell = cell_parser(a, C, R, open_paren - a + 1, close_paren - a - 1, graph);
    if (ref_cell != -1)
    {
        sleep_value = arr[ref_cell];
        if (sleep_value == INT_MIN)
            return;
        graph->adjLists_head[ref_cell] = Addedge(target_cell, graph->adjLists_head[ref_cell]);
    }
    else
    {
        char *end_ptr;
        sleep_value = strtol(open_paren + 1, &end_ptr, 10);
        if (*end_ptr != ')' || sleep_value <= 0)
            return;
    }

    arr[target_cell] = sleep_value;
    AddFormula(graph, target_cell, ref_cell != -1 ? ref_cell : target_cell, sleep_value, 14, formulaArray);
}