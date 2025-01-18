#include <stdio.h>
#include <stdlib.h>

int cell_parser(char *a, int C, int R, int start, int end)
{
    int cell_col = 0;
    int cell_row = 0;

    for (int i = start; i <= end; i++)
    {
        if (isalpha(a[i]))
            cell_col = 26 * cell_col + (a[i] - 'A' + 1);
        else if (isdigit(a[i]))
            cell_row = 10 * cell_row + (a[i] - '0');
        else
            return -1;
    }
    cell_col -= 1;
    cell_col -= 1;

    if (cell_col == -1 || cell_col == -1)
    {
        return -1;
    }
    return C * cell_col + cell_row;
}
void parser(char *a, int C, int R)
{

    if (a[0] == 'w' || a[0] == 'd' || a[0] == 'a' || a[0] == 's')
    {
        ;
    }
    int pos_equalto = -1;
    int pos_end = -1;

    for (int i = 0; a[i] != '\0'; i++)
    {
        if (a[i] == '=' && pos_equalto == -1)
            pos_equalto = i;
        pos_end = i;
    }
    pos_end++;

    if (pos_equalto == -1)
    {
        printf("Invalid input");
        return;
    }
    if (cell_parser(a, C, R, 0, pos_equalto - 1) == -1)
    {
        printf("Invalid cell");
        return;
    }
    if (pos_end - pos_equalto >= 3)
    {
        if (pos_end - pos_equalto >= 5)
        {
            if (a[pos_equalto + 1] == 'S' && a[pos_equalto + 2] == 'T' && a[pos_equalto + 3] == 'D' && a[pos_equalto + 4] == 'E' && a[pos_equalto + 5] == 'V')
            {
            }
            else if (a[pos_equalto + 1] == 'S' && a[pos_equalto + 2] == 'L' && a[pos_equalto + 3] == 'E' && a[pos_equalto + 4] == 'E' && a[pos_equalto + 5] == 'P')
            {
            }
        }
        else
        {
            if (a[pos_equalto + 1] == 'M' && a[pos_equalto + 2] == 'I' && a[pos_equalto + 3] == 'N')
            {
            }
            else if (a[pos_equalto + 1] == 'M' && a[pos_equalto + 2] == 'A' && a[pos_equalto + 3] == 'X')
            {
            }
            else if (a[pos_equalto + 1] == 'A' && a[pos_equalto + 2] == 'V' && a[pos_equalto + 3] == 'G')
            {
            }
            else if (a[pos_equalto + 1] == 'S' && a[pos_equalto + 2] == 'U' && a[pos_equalto + 3] == 'M')
            {
            }
        }
    }
}

int main()
{
}