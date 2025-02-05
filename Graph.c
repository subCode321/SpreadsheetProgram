#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define NUM_CELLS 18259722
int min2(int a, int b)
{
    if (a < b)
        return a;
    return b;
}
int max2(int a, int b)
{
    if (a > b)
        return a;
    return b;
}

/*
Op                    op_type   op_info
(+)                      1        NULL
(-)                      2        NULL
(* with Cell)            3        NULL
(* with const)           4        NULL
(/ denom with const)     5        NULL
(/ num with const)       6        NULL
(/ denom with cell)      7        NULL
(/ num with cell)        8        NULL
(MIN)                    9        NULL
(MAX)                    10       NULL
(AVG)                    11      |Range|
(SUM)                    12       NULL
(STDEV)                  13      |Range|
(SLEEP)                  14       NULL
*/

typedef struct Stack
{
    int data;
    struct Stack *next_in_stack; //I'm doing so to avoid confusions b/w the two nexts
}Stack;

typedef struct Cell
{
    int cell;
    struct Cell *left;
    struct Cell *right;
    int op_type;
    int op_info;
    int height;

} Cell;

typedef struct Graph
{
    struct Cell **adjLists_head;
} Graph;


int height(Cell *c) {
    if (c == NULL) return -1;

    return c->height;
}

int balance(Cell *c) {
    if (c == NULL) return 0;

    return height(c->left) - height(c->right);
}

Cell* RR(Cell *c) {
    Cell *x = c->left;
    Cell *T2 = x->right;

    x->right = c;
    c->left = T2;

    c->height = max(height(c->left), height(c->right)) + 1;
    x->height = max(height(x->left), height(x->right)) + 1;

    return x;
}

Cell *LL(Cell *c){
    Cell *x = c->right;
    Cell *T2 = x->left;

    x->left = c;
    c->right = T2;

    c->height = max(height(c->left), height(c->right)) + 1;
    x->height = max(height(x->left), height(x->right)) + 1;

    return x;
}
Stack *createStackNode(int data)
{
    struct Stack *newNode = (struct Stack *)malloc(sizeof(struct Stack));
    newNode->data = data;
    newNode->next_in_stack = NULL;
    return newNode;
}

Cell *Addcell(int cell, int op_type, int op_info)
{
    Cell *new_cell = (Cell *)malloc(sizeof(Cell));
    if (!new_cell)
    {
        printf("Cannot allocate memory for %d\n", cell);
        return NULL;
    }

    new_cell->cell = cell;
    new_cell->left = NULL;
    new_cell->right = NULL;
    new_cell->op_type = op_type;
    new_cell->op_info = op_info;
    new_cell->height = 0;
    return new_cell;
}

Graph *CreateGraph()
{
    Graph *graph = (Graph *)malloc(sizeof(Graph));
    if (!graph)
    {
        printf("Cannot allocate memory for graph\n");
        return NULL;
    }

    graph->adjLists_head = (Cell **)malloc(NUM_CELLS * sizeof(Cell *));

    if (!graph->adjLists_head)
    {
        printf("Cannot allocate memory for adjacency lists\n");
        free(graph->adjLists_head);
        free(graph);
        return NULL;
    }

    return graph;
}

Cell* Addedge(int cell1, Cell *x, int op_type, int op_info)
{
    if (x == NULL) {
        Cell *c = Addcell(cell1, op_type, op_info);
        x = c;
        return c;
    }

    if (cell1 < x->cell) {
        x->left = Addedge(cell1, x->left, op_type, op_info);
    }
    else {
        x->right = Addedge(cell1, x->right, op_type, op_info);
    }

    int bal = balance(x);

    if (bal > 1 && cell1 < x->left->cell) return RR(x);

    if (bal < -1 && cell1 > x->right->cell) return LL(x);

    if (bal > 1 && cell1 > x->left->cell) {
        x->left = LL(x->left);
        return RR(x);
    }
    if (bal < -1 && cell1 < x->right->cell) {
        x->right = RR(x->right);
        return LL(x);
    }

    return x;
}


void Toposort(Graph *graph, int v, int visited[], Stack** stack){
    visited[v]=1;
    Cell *headcell = graph->adjLists_head[v]; //what I want to do here is to get the current head cell of the adj list associated with v
    while(headcell!=NULL){
        int child = headcell->cell;
        if(visited[child]!=1){
            Toposort(graph,child,visited,stack);
        }
        headcell = headcell->next;
    }
    Stack *newChild = (Stack *)malloc(sizeof(Stack));
    newChild->data = v;
    newChild->next_in_stack = *stack; // the prev top of the stack is now the next node of this newChild
    *stack = newChild; // newChild is the New top

}

// void Recalc(int cell, int new_value, Graph *graph, int *cell_values)
// {
//     int old_value = cell_values[cell];
//     cell_values[cell] = new_value;

   
//     Cell *x = graph->adjLists_head[cell];
//     if (x == NULL) {
//         // printf("Returning");
//         return;
//     }
//     // printf("%d\n", x->cell);
//     // printf("%d\n", new_value);
//     // printf("%d\n", old_value);
//     // printf("%d\n", cell_values[x->cell]);
//     while (x != NULL)
//     {
//         int dependent_new_value = cell_values[x->cell]; // Default value

//         if (x->op_type ==0){
//             dependent_new_value = new_value;
//         }

//         if (x->op_type == 1)
//         { // (+)
//             dependent_new_value = cell_values[x->cell] + new_value - old_value;
//         }
//         else if (x->op_type == 2)
//         { // (-)
//             dependent_new_value = cell_values[x->cell] - new_value + old_value;
//         }
//         else if (x->op_type == 3)
//         { // (*)
//             if (old_value != 0)
//             {
//                 dependent_new_value = (cell_values[x->cell] / old_value) * new_value;
//                 printf("%d\n", dependent_new_value);
//             }
//         }
//         else if (x->op_type == 4)
//         { // (/)
//             dependent_new_value = (cell_values[x->cell] * old_value) / new_value;
//         }
//         else if (x->op_type == 5)
//         {
//             dependent_new_value = cell_values[x->cell] / old_value * new_value;
//         }
//         else if (x->op_type == 6)
//         {
//             dependent_new_value = cell_values[x->cell] * old_value / new_value;
//         }
//         else if (x->op_type == 7)
//         {
//             dependent_new_value = cell_values[x->cell] / old_value * new_value;
//         }
//         else if(x->op_type == 8)
//         {
//             dependent_new_value = cell_values[x->cell] * old_value / new_value;
//         }
//         else if(x->op_type == 9)
//         {
//             dependent_new_value = min2(new_value, cell_values[x->cell]);

//         }
//         else if (x->op_type == 10)
//         {
//             dependent_new_value = max2(new_value, cell_values[x->cell]);
//         }
//         else if (x->op_type == 11)
//         {
//             dependent_new_value = (x->op_info * cell_values[x->cell] - old_value + new_value) / x->op_info;
//         }
//         else if (x->op_type == 12)
//         {
//             dependent_new_value = cell_values[x->cell] - old_value + new_value;
//         }
    

            

//         // Update dependent cell and recursively recalculate
//         Recalc(x->cell, dependent_new_value, graph, cell_values);
//         cell_values[x->cell] = dependent_new_value;

//         x = x->next;
//     }
// }

void Recalc(Stack *stack, Graph *graph, int *cell_values, int new_value, int givencell)
{
    int cells_which_have_changed[1000] = {0}; // Stores updated cells
    int idx = 0;
    cells_which_have_changed[idx] = givencell;

    int ogvalue = cell_values[givencell];
    cell_values[givencell] = new_value;

    while (stack != NULL)
    {
        int curr_cell = stack->data;
        stack = stack->next_in_stack; // Move to the next node

        for (int j = 0; j <= idx; j++)
        {
            int dpcell = cells_which_have_changed[j];
            Cell *check_cell = graph->adjLists_head[dpcell];

            int check = 0, op = -1;
            while (check_cell != NULL)
            {
                if (check_cell->cell == curr_cell)
                {
                    check = 1;
                    op = check_cell->op_type;
                    break;
                }
                check_cell = check_cell->next; // Move to next node in adjacency list
            }

            if (check == 1)
            {
                int tmp = cell_values[curr_cell];

                switch (op)
                {
                case 0: // Addition
                case 1: // Same as Addition
                    cell_values[curr_cell] -= ogvalue;
                    cell_values[curr_cell] += new_value;
                    break;

                case 2: // Subtraction
                    cell_values[curr_cell] += ogvalue;
                    cell_values[curr_cell] -= new_value;
                    break;

                case 3: // Division (Avoid Zero Division)
                    if (ogvalue != 0)
                    {
                        cell_values[curr_cell] /= ogvalue;
                    }
                    cell_values[curr_cell] *= new_value;
                    break;

                case 4: // Multiplication
                    if (new_value != 0)
                    {
                        cell_values[curr_cell] *= ogvalue;
                        cell_values[curr_cell] /= new_value;
                    }
                    break;
                }

                ogvalue = tmp;
                new_value = cell_values[curr_cell];
                cells_which_have_changed[++idx] = curr_cell;
            }
        }
    }
}
