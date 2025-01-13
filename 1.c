#include <stdio.h>
#include <stdlib.h>

void printer(int currx, int curry, int *arr, int C) {
    for (int i = 0; i < 10; i++) {
        for (int j = 0; j < 10; j++) {
            printf("%d ", arr[C * (currx + i) + (curry + j)]);
        }
        printf("\n");
    }
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        printf("Usage: %s <rows> <columns>\n", argv[0]);
        return 1;
    }

    int R = atoi(argv[1]);
    int C = atoi(argv[2]);

    int *arr = (int *)calloc(R * C, sizeof(int));
    if (arr == NULL) {
        printf("Memory allocation failed\n");
        return 1;
    }

    int currx = 0;
    int curry = 0;

    printer(currx, curry, arr, C);
    char a = 'n';

    int flag = 0;
    while (1) {
        if (a != 'n') {
            if (a == 'w') {
                if (curry < 10) {
                    flag = 1;
                } else {
                    curry -= 10;
                    printer(currx, curry, arr, C);
                }
            } else if (a == 'd') {
                if (currx > C - 10) {
                    flag = 1;
                } else {
                    currx += 10;
                    printer(currx, curry, arr, C);
                }
            } else if (a == 'a') {
                if (currx < 10) {
                    flag = 1;
                } else {
                    currx -= 10;
                    printer(currx, curry, arr, C);
                }
            } else {
                if (curry > R - 10) {
                    flag = 1;
                } else {
                    curry += 10;
                    printer(currx, curry, arr, C);
                }
            }
            if (flag) {
                printf("Invalid");
                flag = 0;
            } else {
                printf("Ok");

            }
            a = 'n';
        } else {
            char b;
            scanf(" %c", &b);
            if (b != '\0') {
                if (b == 'w' || b == 'd' || b == 'a' || b == 's') {
                    a = b;
                }
            }
        }
    }

    free(arr);

    return 0;
}