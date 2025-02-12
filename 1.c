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

    int check = 0;

    printer(currx, curry, arr, C, R);

    char *a = (char *)malloc(200 * sizeof(char));

    if (a == NULL)
    {
        printf("Memory allocation failed\n");
        free(arr);
        return 1;
    }

    while (1)
    {
        printf("> ");
        fgets(a, 100, stdin);
        a[strcspn(a, "\n")] = '\0';

        if (a[0] == 'q' && a[1] == '\0')
        {
            break;
        }

        if (check == 1 && strncmp(a, "enable_output", 13) != 0)
        {
            continue;
        }

        // else
        // {
        //     scroller(a, arr, &currx, &curry, C, R);
        // }
        clock_t start = clock(); // Start timing

        int status = 1;

        if (a[0] == 'w' || a[0] == 'a' || (a[0] == 's' && a[1] == '\0') || (a[0] == 'd' && a[1] == '\0'))
        {
            scroller(a, arr, &currx, &curry, C, R, graph);
            continue;
        }

        else if (a[0] == 's' && a[1] == 'c' && a[2] == 'r' && a[3] == 'o' && a[4] == 'l' && a[5] == 'l' && a[6] == '_' && a[7] == 't' && a[8] == 'o' && a[9] == ' ')
        {
            scroller(a, arr, &currx, &curry, C, R, graph);
            continue;
        }

        else if (a[0] == 'd' && a[1] == 'i' && a[2] == 's' && a[3] == 'a' && a[4] == 'b' && a[5] == 'l' && a[6] == 'e' &&
                 a[7] == '_' && a[8] == 'o' && a[9] == 'u' && a[10] == 't' && a[11] == 'p' && a[12] == 'u' && a[13] == 't')
        {
            check = 1;
            continue;
        }
        else if (a[0] == 'e' && a[1] == 'n' && a[2] == 'a' && a[3] == 'b' && a[4] == 'l' && a[5] == 'e' &&
                 a[6] == '_' && a[7] == 'o' && a[8] == 'u' && a[9] == 't' && a[10] == 'p' && a[11] == 'u' && a[12] == 't')
        {
            check = 0;
            continue;
        }

        else
        {
            status = parser(a, C, R, arr, graph);
        }

        clock_t end = clock();
        double time_taken = ((double)(end - start)) / CLOCKS_PER_SEC;

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

    free(a);
    free(arr);

    return 0;
}
