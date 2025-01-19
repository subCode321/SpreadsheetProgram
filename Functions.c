#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "Parser.h"

int arithmetic_eval(int v1, int v2, char op){
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

void min_func(char *a, int C, int R, int pos_equalto, int pos_end, int *arr)
{
    char colon = ':';
    char *colon_pos = strchr(a, colon);

    if (colon_pos == NULL)
    {
        printf("Invalid range: Missing ':'\n");
        return;
    }

    int range_start = cell_parser(a, C, R, pos_equalto + 5, colon_pos - a - 1);
    int range_end = cell_parser(a, C, R, colon_pos - a + 1, pos_end - 1);

    if (range_start == -1 || range_end == -1)
    {
        printf("Invalid range\n");
        return;
    }

    int min_value = arr[range_start];
    for (int idx = range_start + 1; idx <= range_end; idx++)
    {
        if (arr[idx] < min_value)
        {
            min_value = arr[idx];
        }
    }

    int dest_cell = cell_parser(a, C, R, 0, pos_equalto - 1);
    if (dest_cell == -1)
    {
        printf("Invalid destination cell\n");
        return;
    }

    arr[dest_cell] = min_value;

}

void max_func(char *a, int C, int R, int pos_equalto, int pos_end, int *arr) 
{
}

void avg_func(char *a, int C, int R, int pos_equalto, int pos_end, int *arr)
{
}

void sum_func(char *a, int C, int R, int pos_equalto, int pos_end, int *arr)
{
}

void stdev_func(char *a, int C, int R, int pos_equalto, int pos_end, int *arr)
{
}

void sleep_func(char *a, int C, int R, int pos_equalto, int pos_end, int *arr)
{
}

