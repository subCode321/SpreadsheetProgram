#include <stdio.h>

void min_func(char *a, int C, int R, int pos_equalto, int pos_end, int *arr)
{
    char colon = ':';
    char *colon_pos = strchr(a, colon);

    if (colon_pos == NULL)
    {
        printf("Invalid range: Missing ':'\n");
        return;
    }

    int range_start = cell_parser(a, C, R, pos_equalto + 5, colon_pos - a - 1);
    int range_end = cell_parser(a, C, R, colon_pos - a + 1, pos_end - 1);

    if (range_start == -1 || range_end == -1)
    {
        printf("Invalid range\n");
        return;
    }

    int min_value = arr[range_start];
    for (int idx = range_start + 1; idx <= range_end; idx++)
    {
        if (arr[idx] < min_value)
        {
            min_value = arr[idx];
        }
    }

    int dest_cell = cell_parser(a, C, R, 0, pos_equalto - 1);
    if (dest_cell == -1)
    {
        printf("Invalid destination cell\n");
        return;
    }

    arr[dest_cell] = min_value;

}

int main() {

}