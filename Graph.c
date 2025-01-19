typedef struct Cell
{
    int cell_num;      
    struct Cell *next;
} Cell;

typedef struct List
{
    Cell *head;
} List;

