#include <stdio.h>
#include <stdlib.h>
#include "display.h"
#include <time.h>
#include "Parser.h"
#include "Graph.h"
#include "string.h"

int main(int argc, char *argv[])
{
    Graph *graph = CreateGraph();
    if (argc != 3)
    {
        printf("Usage: %s <rows> <columns>\n", argv[0]);
        return 1;
    }

    int R = atoi(argv[1]);
    int C = atoi(argv[2]);

    int *arr = (int *)calloc(R * C, sizeof(int));
    if (arr == NULL)
    {
        printf("Memory allocation failed\n");
        return 1;
    }

    int currx = 0;
    int curry = 0;
    int output_disabled = 0; // Flag to track output state

    // Initial display only if output is enabled
    if (!output_disabled)
    {
        printer(currx, curry, arr, C, R);
    }

    char *a = (char *)malloc(200 * sizeof(char));
    if (a == NULL)
    {
        printf("Memory allocation failed\n");
        free(arr);
        return 1;
    }

    while (1)
    {
        // Always show prompt regardless of output state
        printf("> ");
        fgets(a, 100, stdin);
        a[strcspn(a, "\n")] = '\0';

        if (a[0] == 'q' && a[1] == '\0')
        {
            break;
        }

        clock_t start = clock();
        int status = 1;

        // Handle output control commands
        if (strcmp(a, "disable_output") == 0)
        {
            output_disabled = 1;
            continue;
        }
        else if (strcmp(a, "enable_output") == 0)
        {
            output_disabled = 0;
            // Show current state when output is re-enabled
            printer(currx, curry, arr, C, R);
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
            status = parser(a, C, R, arr, graph);
        }

        clock_t end = clock();
        double time_taken = ((double)(end - start)) / CLOCKS_PER_SEC;

        // Only display output if not disabled
        if (!output_disabled)
        {
            printer(currx, curry, arr, C, R);
            if (status > 0)
            {
                printf("[%.6f] (ok)  ", time_taken);
            }
            else
            {
                printf("[%.6f] (unrecognized command) ", time_taken);
            }
        }
    }

    free(a);
    free(arr);
    return 0;
}