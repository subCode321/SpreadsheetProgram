#include <stdio.h>
#include <stdlib.h>

int main() {
    int R;
    int C;
    scanf("%d", &R);
    scanf("%d", &C);

    int *arr = (int *)calloc(R * C, sizeof(int));
    if (arr == NULL) {
        printf("Memory allocation failed\n");
        return 1;
    }

    char a = 'n';

    while (1) {
        if (a != 'n') {
            printf("current input is %c\n", a);
            a = 'n';
        }

        else {
            char b;
            scanf(" %c", &b);
            if (b != '\0') {
                a = b;
            }
        }
    }

    free(arr);

    return 0;
}