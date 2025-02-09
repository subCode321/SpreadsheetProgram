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

// void printer(int currx, int curry, int *arr, int C, int R)
// {
//     printf("   ");

//     // Print column headers (A, B, C...)
//     for (int i = 0; i < min(10, C - currx); i++)
//     {
//         int val = currx + i + 1;
//         char s[5];
//         int idx = 0;

//         while (val > 0)
//         {
//             val--;
//             s[idx++] = 'A' + (val % 26);
//             val /= 26;
//         }
//         s[idx] = '\0';
//         char s2[5];

//         for (int j = idx - 1; j >= 0; j--)
//         {
//             s2[idx - 1 - j] = s[j];
//         }
//         printf("%-10s", s2); // Print column header
//     }
//     printf("\n");

//     // Print row data
//     for (int j = 0; j < min(10, R - curry); j++)
//     {
//         printf("%-3d", curry + j + 1); // Print row numbers

//         for (int i = 0; i < min(10, C - currx); i++)
//         {
//             int value = arr[(currx + i) + C * (curry + j)];

//             if (value == INT_MIN)
//             {
//                 printf("%-10s", "Err"); // Print "Err" for error cells
//             }
//             else
//             {
//                 printf("%-10d", value); // Print valid cell values
//             }
//         }
//         printf("\n");
//     }
// }
void printer(int currx, int curry, int *arr, int C, int R)
{
    printf("   ");

    for (int i = 0; i < min(10, C - currx); i++)
    {
        int val = currx + i + 1;
        char s[5];
        int idx = 0;

        while (val > 0)
        {
            val--;
            s[idx++] = 'A' + (val % 26);
            val /= 26;
        }
        s[idx] = '\0';
        char s2[5];

        for (int j = idx - 1; j >= 0; j--)
        {
            s2[idx - 1 - j] = s[j];
        }
        printf("%-10s", s);
    }
    printf("\n");

    for (int j = 0; j < min(10, R - curry); j++)
    {
        printf("%-3d", curry + j + 1);
        for (int i = 0; i < min(10, C - currx); i++)
        {
            int value = arr[(currx + i) + C * (curry + j)];
            if (value == INT_MIN)
            {
                printf("%-10s", "Err"); // Print "Err" for error cells
            }
            else
            {
                printf("%-10d", value); // Print valid cell values
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
        if (*curry < 10)
        {
            flag = 1;
        }
        else
        {
            *curry -= 10;
            printer(*currx, *curry, arr, C, R);
        }
    }
    else if (a[0] == 'd' && a[1] == '\0')
    {
        if (*currx >= C - 10)
        {
            flag = 1;
        }
        else
        {
            *currx += 10;
            printer(*currx, *curry, arr, C, R);
        }
    }
    else if (a[0] == 'a' && a[1] == '\0')
    {
        if (*currx < 10)
        {
            flag = 1;
        }
        else
        {
            *currx -= 10;
            printer(*currx, *curry, arr, C, R);
        }
    }
    else if (a[0] == 's' && a[1] == '\0')
    {
        if (*curry >= R - 10)
        {
            flag = 1;
        }
        else
        {
            *curry += 10;
            printer(*currx, *curry, arr, C, R);
        }
    }

    else if (strncmp(a, "scroll_to ", 10) == 0)
    {
        int cell = cell_parser(a, C, R, 10, strlen(a) - 1, graph);
        if (cell == -1)
        {
            printf("Invalid cell input in scroll_to command\n");
            flag = 1;
        }
        else
        {
            int start_row = cell / C;
            int start_col = cell % C;
            printf("Parsed row: %d, col: %d\n", start_row, start_col); // Debugging output
            if (start_row >= R || start_col >= C)
            {
                printf("Scroll target out of bounds\n");
                flag = 1;
            }
            else
            {
                *currx = start_col;
                *curry = start_row;
                printer(*currx, *curry, arr, C, R);
            }
        }
    }
    else
    {
        printf("unrecognized command");
    }

    if (flag)
    {
        printf("Invalid\n");
    }
    else
    {
        printf("ok\n");
    }
}