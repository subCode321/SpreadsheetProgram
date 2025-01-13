#include <stdio.h>
#include <stdlib.h>

int main() {

        int R;
        int C;
        scanf("%d", &R);
        scanf("%d", &C);

        int *arr = (int *)calloc(R * C, sizeof(int));

        return 0;
}
