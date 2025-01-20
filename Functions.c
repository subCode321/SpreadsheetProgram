#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "Parser.h"
#include <math.h>

int validate_range(int range_start, int range_end, int C) {
    
    int start_row = range_start / C;
    int start_col = range_start % C;
    int end_row = range_end / C;
    int end_col = range_end % C;

    if (start_row > end_row || (start_row == end_row && start_col > end_col)) {
        // Invalid range: end cell comes before start cell
        return 0;
    }
    return 1;
}


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
    int first_cell;

    
    first_cell = cell_parser(a, C, R, 0, pos_equalto - 1);
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

    int range_start = cell_parser(a, C, R, open_paren - a + 1, colon_pos - a - 1);
    int range_end = cell_parser(a, C, R, colon_pos - a + 1, close_paren - a - 1);
    if (range_start == -1 || range_end == -1 || !validate_range(range_start, range_end, C))
    {
        printf("Invalid range\n");
        return;
    }

    
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
            int col_start = start_col;
            int col_end = end_col;

            for (int col = col_start; col <= col_end; col++)
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

}

void maxfunc(char *a, int C, int R, int pos_equalto, int pos_end, int *arr)
{
    int first_cell;

    
    first_cell = cell_parser(a, C, R, 0, pos_equalto - 1);
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

    int range_start = cell_parser(a, C, R, open_paren - a + 1, colon_pos - a - 1);
    int range_end = cell_parser(a, C, R, colon_pos - a + 1, close_paren - a - 1);
    if (range_start == -1 || range_end == -1 || !validate_range(range_start, range_end, C))
    {
        printf("Invalid range\n");
        return;
    }

    
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
            int col_start = start_col;
            int col_end = end_col;

            for (int col = col_start; col <= col_end; col++)
            {
                int idx = row * C + col;
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


void avg_func(char *a, int C, int R, int pos_equalto, int pos_end, int *arr)
{
    int first_cell = cell_parser(a, C, R, 0, pos_equalto - 1);
    if (first_cell == -1) {
        printf("Invalid destination cell\n");
        return;
    }

    
    char *open_paren = strchr(a + pos_equalto, '(');
    char *close_paren = strchr(a + pos_equalto, ')');

    if (!open_paren || !close_paren || close_paren <= open_paren + 1) {
        printf("Invalid range: Missing or misplaced parentheses\n");
        return;
    }

    char *colon_pos = strchr(open_paren + 1, ':');
    if (!colon_pos) {
        printf("Invalid range: Missing ':'\n");
        return;
    }

    int range_start = cell_parser(a, C, R, open_paren - a + 1, colon_pos - a - 1);
    int range_end = cell_parser(a, C, R, colon_pos - a + 1, close_paren - a - 1);
    if (range_start == -1 || range_end == -1 || !validate_range(range_start, range_end, C)) {
        printf("Invalid range\n");
        return;
    }

    int start_row = range_start / C;
    int start_col = range_start % C;
    int end_row = range_end / C;
    int end_col = range_end % C;

    int sum = 0, count = 0;

    for (int row = start_row; row <= end_row; row++) {
        int col_start = start_col;
        int col_end = end_col;

        for (int col = col_start; col <= col_end; col++) {
            int idx = row * C + col;
            sum += arr[idx];
            count++;
        }
    }

    int avg_value = (count > 0) ? (sum / count) : 0;
    arr[first_cell] = avg_value;
}

void sum_func(char *a, int C, int R, int pos_equalto, int pos_end, int *arr)
{
    int first_cell = cell_parser(a, C, R, 0, pos_equalto - 1);
    if (first_cell == -1) {
        printf("Invalid destination cell\n");
        return;
    }

    // Parse the range
    char *open_paren = strchr(a + pos_equalto, '(');
    char *close_paren = strchr(a + pos_equalto, ')');
    if (!open_paren || !close_paren || close_paren <= open_paren + 1) {
        printf("Invalid range: Missing or misplaced parentheses\n");
        return;
    }

    char *colon_pos = strchr(open_paren + 1, ':');
    if (!colon_pos) {
        printf("Invalid range: Missing ':'\n");
        return;
    }

    int range_start = cell_parser(a, C, R, open_paren - a + 1, colon_pos - a - 1);
    int range_end = cell_parser(a, C, R, colon_pos - a + 1, close_paren - a - 1);
    if (range_start == -1 || range_end == -1 || !validate_range(range_start, range_end, C)) {
        printf("Invalid range\n");
        return;
    
    }
    int start_row = range_start / C;
    int start_col = range_start % C;
    int end_row = range_end / C;
    int end_col = range_end % C;

    // Calculate SUM
    int sum = 0;


    for (int row = start_row; row <= end_row; row++) {
        int col_start = start_col;
        int col_end = end_col;

        for (int col = col_start; col <= col_end; col++) {
            int idx = row * C + col;
            sum += arr[idx];
        }
    }

    arr[first_cell] = sum;
}

void stdev_func(char *a, int C, int R, int pos_equalto, int pos_end, int *arr)
{
    int first_cell = cell_parser(a, C, R, 0, pos_equalto - 1);
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

    int range_start = cell_parser(a, C, R, open_paren - a + 1, colon_pos - a - 1);
    int range_end = cell_parser(a, C, R, colon_pos - a + 1, close_paren - a - 1);
    if (range_start == -1 || range_end == -1 || !validate_range(range_start, range_end, C))
    {
        printf("Invalid range\n");
        return;
    }
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
            sum += arr[idx];
            count++;
        }
    }
    int avg = sum/count ;
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
    int stdev = sqrt(stdev_squared/count);
    arr[first_cell] = stdev;
}

void sleep_func(char *a, int C, int R, int pos_equalto, int pos_end, int *arr)
{
     
}

