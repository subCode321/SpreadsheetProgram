#include <stdio.h>
#include <stdlib.h>
#include "Graph.h"
#include "display.h"
#include "Parser.h"
#include <string.h>
#include <limits.h>

int min(int x, int y)
{
    if (x > y)
        return y;
    else
        return x;
}

int max(int x, int y)
{
    if (x > y)
        return x;
    else
        return y;
}

void printer(int currx, int curry, int *arr, int C, int R)
{
    printf("      ");

    for (int i = 0; i < (C - currx < 10 ? C - currx : 10); i++)
    {
        int val = currx + i + 1;
        char s[5], s2[5];
        int idx = 0;

        while (val > 0)
        {
            val--;
            s[idx++] = 'A' + (val % 26);
            val /= 26;
        }
        s[idx] = '\0';

        // Reverse the string for correct order
        for (int j = 0; j < idx; j++)
        {
            s2[j] = s[idx - j - 1];
        }
        s2[idx] = '\0'; // Ensure null termination

        printf("%-10s", s2); // Changed to left-align the column headers
    }
    printf("\n");

    for (int j = 0; j < (R - curry < 10 ? R - curry : 10); j++)
    {
        printf("%-3d", curry + j + 1); // Row number (left-aligned)
        printf("   ");
        for (int i = 0; i < (C - currx < 10 ? C - currx : 10); i++)
        {
            int value = arr[(currx + i) + C * (curry + j)];
            if (value == INT_MIN)
            {
                printf("%-10s", "ERR"); // Left-aligned "ERR"
            }
            else
            {
                printf("%-10d", value); // Left-aligned numbers
            }
        }
        printf("\n");
    }
}

void scroller(char *a, int *arr, int *currx, int *curry, int C, int R, Graph *graph)
{
    int flag = 0;
    if (a[0] == 'w' && a[1] == '\0')
    {
        // If there are less than 10 rows above, scroll to the top
        if (*curry < 10)
        {
            if (*curry > 0) // If we're not already at the top
            {
                *curry = 0;
            }
            else
            {
                flag = 1;
            }
        }
        else
        {
            *curry -= 10;
        }
    }
    else if (a[0] == 'd' && a[1] == '\0')
    {
        // Calculate how many columns we can scroll right
        int remaining_cols = C - *currx - 10;
        if (remaining_cols <= 0)
        {
            flag = 1;
        }
        else if (remaining_cols < 10)
        {
            *currx += remaining_cols;
        }
        else
        {
            *currx += 10;
        }
    }
    else if (a[0] == 'a' && a[1] == '\0')
    {
        // If there are less than 10 columns to the left, scroll to the leftmost
        if (*currx < 10)
        {
            if (*currx > 0) // If we're not already at the leftmost
            {
                *currx = 0;
            }
            else
            {
                flag = 1;
            }
        }
        else
        {
            *currx -= 10;
        }
    }
    else if (a[0] == 's' && a[1] == '\0')
    {
        // Calculate how many rows we can scroll down
        int remaining_rows = R - *curry - 10;
        if (remaining_rows <= 0)
        {
            flag = 1;
        }
        else if (remaining_rows < 10)
        {
            *curry += remaining_rows;
        }
        else
        {
            *curry += 10;
        }
    }
    else if (strncmp(a, "scroll_to ", 10) == 0)
    {
        int cell = cell_parser(a, C, R, 10, strlen(a) - 1, graph);
        if (cell == -1)
        {
            flag = 1;
        }
        else
        {
            int start_row = cell / C;
            int start_col = cell % C;
            if (start_row >= R || start_col >= C)
            {
                // printf("Scroll target out of bounds\n");
                flag = 1;
            }
            else
            {
                *currx = start_col;
                *curry = start_row;
            }
        }
    }
    else
    {
        printf("unrecognized command");
    }
    if (flag)
    {
        ; // Do nothing on invalid scroll
    }
}