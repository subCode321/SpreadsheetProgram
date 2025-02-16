#include <stdio.h>
#include <stdlib.h>
#include "display.h"
#include <time.h>
#include "Parser.h"
#include "Graph.h"
#include "string.h"

int NUM_CELLS;
Formula *formulaArray;

int main(int argc, char *argv[])
{
    // Validate command line arguments
    if (argc != 3)
    {
        printf("Usage: %s <rows> <columns>\n", argv[0]);
        return 1;
    }

    // Convert and validate command line arguments
    int R = atoi(argv[1]);
    int C = atoi(argv[2]);
    if (R <= 0 || C <= 0)
    {
        printf("Error: Rows and columns must be positive integers\n");
        return 1;
    }

    // Initialize global variables and structures
    NUM_CELLS = R * C;
    Graph *graph = CreateGraph();
    if (graph == NULL)
    {
        printf("Error: Failed to create graph\n");
        return 1;
    }

    formulaArray = malloc(NUM_CELLS * sizeof(Formula));
    if (formulaArray == NULL)
    {
        printf("Error: Memory allocation failed for formula array\n");
       return 1;
    }

    // Allocate and initialize the main array
    int *arr = (int *)calloc(R * C, sizeof(int));
    if (arr == NULL)
    {
        printf("Error: Memory allocation failed for main array\n");
        free(formulaArray);
        return 1;
    }

    // Initialize cursor position and output state
    int currx = 0;
    int curry = 0;
    int output_disabled = 0;

    // Show initial display
    if (!output_disabled)
    {
        printer(currx, curry, arr, C, R);
    }

    // Open and validate input file
    FILE *file = fopen("input.txt", "r");
    if (!file)
    {
        printf("Error: Unable to open input.txt\n");
        free(arr);
        free(formulaArray);
        return 1;
    }

    // Allocate command buffer
    char *command = (char *)malloc(200 * sizeof(char));
    if (command == NULL)
    {
        printf("Error: Memory allocation failed for command buffer\n");
        fclose(file);
        free(arr);
        free(formulaArray);
        return 1;
    }

    // Main command processing loop
    while (fgets(command, 200, file))
    {
        printf("> ");
        command[strcspn(command, "\n")] = '\0';

        // Check for quit command
        if (command[0] == 'q' && command[1] == '\0')
        {
            break;
        }

        clock_t start = clock();
        int status = 1;

        // Handle output control commands
        if (strcmp(command, "disable_output") == 0)
        {
            output_disabled = 1;
            continue;
        }
        else if (strcmp(command, "enable_output") == 0)
        {
            output_disabled = 0;
            printer(currx, curry, arr, C, R);
            continue;
        }

        // Process movement and other commands
        if (command[0] == 'w' || command[0] == 'a' ||
            (command[0] == 's' && command[1] == '\0') ||
            (command[0] == 'd' && command[1] == '\0') ||
            strncmp(command, "scroll_to ", 10) == 0)
        {
            scroller(command, arr, &currx, &curry, C, R, graph);
        }
        else
        {
            status = parser(command, C, R, arr, graph, formulaArray);
        }

        // Display results if output is enabled
        if (!output_disabled)
        {
            printer(currx, curry, arr, C, R);
            clock_t end = clock();
            double time_taken = ((double)(end - start)) / CLOCKS_PER_SEC;
            printf("[%.6f] (%s)  ", time_taken,
                   status > 0 ? "ok" : "unrecognized command");
        }
    }

    // Cleanup
    fclose(file);
    free(command);
    free(arr);
    free(formulaArray);
    return 0;
}