#include <stdio.h>
#include <stdlib.h>

int min(int x, int y)
{
    if (x > y)
        return y;
    else
        return x;
}

void printer(int currx, int curry, int *arr, int C, int R)
{
    printf("       ");

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

        for (int j = idx - 1; j >= 0; j--)
        {
            printf("%c", s[j]);
        }
        printf("      ");
    }
    printf("\n");

    for (int j = 0; j < min(10, R - curry); j++)
    {   
        printf("%d      ", curry + j + 1);
        for (int i = 0; i < min(10, C - currx); i++)
        {
            printf("%d      ", arr[(currx + i) + C * (curry + j)]);
        }
        printf("\n");
    }
}

void scroller(char *a, int *arr, int *currx, int *curry, int C, int R)
{
    int flag = 0;

    if (a[0] == 'w')
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
    else if (a[0] == 'd')
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
    else if (a[0] == 'a')
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
    else if (a[0] == 's')
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
        printf("Ok\n");
    }
}

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
