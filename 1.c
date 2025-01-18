#include <stdio.h>
#include <stdlib.h>
#include "display.h"

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

    while (1)
    {
        scanf(" %s", a);

        if (a[0] == 'q')
        {
            break;
        }
        else
        {
            scroller(a, arr, &currx, &curry, C, R);
        }

        free(a);
        free(arr);

    }

    return 0;
}
