#include <stdio.h>
#include <stdlib.h>
#include "display.h"
#include <time.h>
#include "Parser.h"
#include "Graph.h"
#include "string.h"
#include <sys/time.h>

int NUM_CELLS;
Formula *formulaArray;

int main(int argc, char *argv[])
{
    struct timeval start, end;
    gettimeofday(&start, NULL); // Start timing from the very beginning

    if (argc != 3)
    {
        printf("Usage: %s <rows> <columns>\n", argv[0]);
        return 1;
    }

    int R = atoi(argv[1]);
    int C = atoi(argv[2]);

    NUM_CELLS = R * C;
    Graph *graph = CreateGraph();
    formulaArray = malloc(NUM_CELLS * sizeof(Formula));

    int *arr = (int *)calloc(R * C, sizeof(int));
    if (arr == NULL)
    {
        printf("Memory allocation failed\n");
        return 1;
    }

    int currx = 0;
    int curry = 0;
    int output_disabled = 0; // Flag to track output state

    // Open the input file
    FILE *inputFile = fopen("input.txt", "r");
    if (inputFile == NULL)
    {
        printf("Error: Could not open input.txt\n");
        free(arr);
        free(formulaArray);
        FreeGraph(graph);
        return 1;
    }

    // Initial display only if output is enabled
    if (!output_disabled)
    {
        printer(currx, curry, arr, C, R);
    }

    // Print timing and status always
    gettimeofday(&end, NULL);
    double time_taken = (end.tv_sec - start.tv_sec) + (end.tv_usec - start.tv_usec) / 1000000.0;
    printf("[%.6f] (ok) \n", time_taken);

    char a[200];

    while (fgets(a, sizeof(a), inputFile))
    {
        a[strcspn(a, "\n")] = '\0'; // Remove newline character

        if (a[0] == 'q' && a[1] == '\0')
        {
            break;
        }

        gettimeofday(&start, NULL);
        int status = 1;

        // Handle output control commands
        if (strcmp(a, "disable_output") == 0)
        {
            output_disabled = 1;
            gettimeofday(&end, NULL);
            double time_taken = (end.tv_sec - start.tv_sec) + (end.tv_usec - start.tv_usec) / 1000000.0;
            printf("[%.6f] (ok) \n", time_taken);
            continue;
        }
        else if (strcmp(a, "enable_output") == 0)
        {
            output_disabled = 0;
            gettimeofday(&end, NULL);
            double time_taken = (end.tv_sec - start.tv_sec) + (end.tv_usec - start.tv_usec) / 1000000.0;
            printer(currx, curry, arr, C, R);
            printf("[%.6f] (ok) \n", time_taken);
            continue;
        }

        // Process commands regardless of output state
        if (a[0] == 'w' || a[0] == 'a' || (a[0] == 's' && a[1] == '\0') || (a[0] == 'd' && a[1] == '\0'))
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

        // Only display spreadsheet if output is not disabled
        if (!output_disabled)
        {
            printer(currx, curry, arr, C, R);
        }

        // Always print time & status regardless of output setting
        if (status > 0)
        {
            printf("[%.6f] (ok) \n", time_taken);
        }
        else
        {
            printf("[%.6f] (unrecognized command) \n", time_taken);
        }
    }

    fclose(inputFile);
    free(arr);
    free(formulaArray);
    FreeGraph(graph);

    return 0;
}
