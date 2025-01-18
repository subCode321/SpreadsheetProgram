#include <stdio.h>
#include <stdlib.h>
#include "display.h"
#include <time.h>
#include "Parser.h"

int main(int argc, char *argv[])
{
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
        scanf(" %s", a);

        if (a[0] == 'q')
        {
            break;
        }
        // else
        // {
        //     scroller(a, arr, &currx, &curry, C, R);
        // }
        clock_t start = clock(); // Start timing

        int status = 1;

        
        if (a[0] == 'w' || a[0] == 'a' || a[0] == 's' || a[0] == 'd')
        {
            scroller(a, arr, &currx, &curry, C, R); 
        }
        else
        {
            status = parser(a, C, R, arr); 
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
