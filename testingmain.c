#include <stdio.h>
#include <stdlib.h>
#include "display.h"
#include <time.h>
#include "Parser.h"
#include "Graph.h"
#include <string.h>
#include <sys/time.h>

int NUM_CELLS;
int hasCycle;
int invalidRange;
Formula *formulaArray;

void trim_spaces(char *str)
{
    int len = strlen(str);
    while (len > 0 && (str[len - 1] == ' ' || str[len - 1] == '\t' || str[len - 1] == '\n' || str[len - 1] == '\r'))
    {
        str[--len] = '\0'; // Remove trailing spaces, tabs, and newlines
    }
}

int main(int argc, char *argv[])
{
    struct timeval start, end;
    gettimeofday(&start, NULL);

    if (argc != 3)
    {
        printf("Usage: %s <rows> <columns>\n", argv[0]);
        return 1;
    }

    int R = atoi(argv[1]);
    int C = atoi(argv[2]);
    if (R <= 0 || R > 999 || C <= 0 || C > 18278)
    {
        printf("Error: Invalid grid size. Rows must be 1-999, Columns must be 1-18278.\n");
        return 1;
    }

    NUM_CELLS = R * C;
    Graph *graph = CreateGraph();
    formulaArray = malloc(NUM_CELLS * sizeof(Formula));
    int *arr = (int *)calloc(R * C, sizeof(int));
    if (!arr || !formulaArray)
    {
        printf("Memory allocation failed\n");
        return 1;
    }

    int currx = 0, curry = 0;
    int output_disabled = 0;

    FILE *inputFile = fopen("input.txt", "r");
    if (!inputFile)
    {
        printf("Error: Could not open input.txt\n");
        free(arr);
        free(formulaArray);
        FreeGraph(graph);
        return 1;
    }

    if (!output_disabled)
    {
        printer(currx, curry, arr, C, R);
    }
    gettimeofday(&end, NULL);
    printf("[%.6f] (ok) \n", (end.tv_sec - start.tv_sec) + (end.tv_usec - start.tv_usec) / 1000000.0);

    char a[200];
    while (fgets(a, sizeof(a), inputFile))
    {
        trim_spaces(a);
        printf("Processing command: \"%s\"\n", a);

        if (strcmp(a, "q") == 0)
            break;
        gettimeofday(&start, NULL);
        int status = 1;

        if (strcmp(a, "disable_output") == 0)
        {
            output_disabled = 1;
        }
        else if (strcmp(a, "enable_output") == 0)
        {
            output_disabled = 0;
            printer(currx, curry, arr, C, R);
        }
        else if (a[0] == 'w' || a[0] == 'a' || a[0] == 's' || a[0] == 'd')
        {
            scroller(a, arr, &currx, &curry, C, R, graph);
        }
        else if (strncmp(a, "scroll_to ", 10) == 0)
        {
            scroller(a, arr, &currx, &curry, C, R, graph);
        }
        else
        {
            status = parser(a, C, R, arr, graph, formulaArray);
        }

        gettimeofday(&end, NULL);
        double time_taken = (end.tv_sec - start.tv_sec) + (end.tv_usec - start.tv_usec) / 1000000.0;

        if (!output_disabled)
        {
            printer(currx, curry, arr, C, R);
        }

        if (status > 0)
        {
            printf("[%.6f] (ok) \n", time_taken);
        }
        else
        {
            if (hasCycle)
            {
                printf("[%.6f] (Circular dependency detected) \n", time_taken);
                hasCycle = 0;
            }
            else if (invalidRange)
            {
                printf("[%.6f] (Invalid range) \n", time_taken);
                invalidRange = 0;
            }
            else
            {
                printf("[%.6f] (unrecognized command) \n", time_taken);
            }
        }
    }

    fclose(inputFile);
    free(arr);
    free(formulaArray);
    FreeGraph(graph);
    return 0;
}