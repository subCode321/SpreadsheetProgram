#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Graph.h"
#include "Functions.h"
#include <limits.h>

int isAlpha(char c)
{
    if (c >= 'A' && c <= 'Z')
        return 1;
    return 0;
    
}

int isDigit(char c)
{
    if (c >= '0' && c <= '9')
        return 1;
    
    return 0;
}

int cell_parser(char *a, int C, int R, int start, int end, Graph *graph)
{
    int cell_col = 0;
    int cell_row = 0;

    for (int i = start; i <= end; i++)
    {
        if (isAlpha(a[i]))
            cell_col = 26 * cell_col + (a[i] - 'A' + 1);
        else if (isDigit(a[i]))
            cell_row = 10 * cell_row + (a[i] - '0');
        else
            return -1;
    }
    cell_col -= 1;
    cell_row -= 1;

    if (cell_col == -1 || cell_row == -1)
    {
        return -1;
    }
    return C * cell_row + cell_col;
}

// void valuefunc(char *a, int C, int R, int pos_equalto, int pos_end, int *arr,Graph *graph)   
// {
//     int first_cell;
//     first_cell = cell_parser(a, C, R, 0, pos_equalto - 1, graph);
//     if (first_cell == -1)
//     {
//         printf("Invalid cell");
//         return;
//     }
//     if (formulaArray[first_cell].op_type > 0)
//     {
//         Deleteedge(graph, first_cell, C);
    
//     }
//     int second_cell = -1;
//     int is_cell = 0;
//     if (isDigit(a[pos_equalto + 1]))
//     {
//         char *tmp = (char *)malloc(20 * sizeof(char));
//         for(int i = pos_equalto + 1; i < pos_end; i++)
//         {
//             tmp[i - pos_equalto - 1] = a[i];
//         }
//         tmp[pos_end - pos_equalto - 1] = '\0';
//         second_cell = atoi(tmp);
//         free(tmp);
//     }
//     else
//     {
//         second_cell = cell_parser(a, C, R, pos_equalto + 1, pos_end - 1,graph);
//         is_cell = 1;
//     }

//     if (second_cell == -1)
//     {
//         printf("Invalid\n");
//         return;
//     }
//     if (is_cell == 0)
//     {

//         arr[first_cell] = second_cell;
//         AddFormula(graph, Addcell(first_cell), second_cell, 0, 0);

//             Recalc(graph, C, arr, first_cell);

//         // printf("%d_Hello1 \n", first_cell);
//     }
//     else
//     {
//         int tmp = arr[second_cell];
//         // printf("%d_Hello2 \n", first_cell);
        
//         arr[first_cell] = tmp;
//         graph->adjLists_head[second_cell] = Addedge(first_cell, graph->adjLists_head[second_cell]);
//         AddFormula(graph, Addcell(first_cell), second_cell, 0, 1);
//         Recalc(graph, C, arr, first_cell);
//     }
    
// }
void valuefunc(char *a, int C, int R, int pos_equalto, int pos_end, int *arr, Graph *graph)
{
    int first_cell;
    first_cell = cell_parser(a, C, R, 0, pos_equalto - 1, graph);
    if (first_cell == -1)
    {
        printf("Invalid cell\n");
        return;
    }

    // Delete existing dependencies if the cell already has a formula
    if (formulaArray[first_cell].op_type > 0)
    {
        Deleteedge(graph, first_cell, C);
    }

    int second_cell = -1;
    int is_cell = 0;
    int is_negative = 0; // Flag to track if the value is negative

    // Check if the value starts with a '+' or '-' sign
    if (a[pos_equalto + 1] == '-' || a[pos_equalto + 1] == '+')
    {
        if (a[pos_equalto + 1] == '-')
            is_negative = 1;
        pos_equalto++; // Move past the sign
    }

    // Determine if the value is a constant or a cell reference
    if (isDigit(a[pos_equalto + 1]))
    {
        char *tmp = (char *)malloc(20 * sizeof(char));
        for (int i = pos_equalto + 1; i < pos_end; i++)
        {
            tmp[i - pos_equalto - 1] = a[i];
        }
        tmp[pos_end - pos_equalto - 1] = '\0';
        second_cell = atoi(tmp);
        free(tmp);
    }
    else
    {
        second_cell = cell_parser(a, C, R, pos_equalto + 1, pos_end - 1, graph);
        is_cell = 1;
    }

    if (second_cell == -1)
    {
        printf("Invalid command\n");
        return;
    }

    if (is_negative && !is_cell) // If it's a negative constant
    {
        second_cell = -second_cell;
    }

    // Handle constant values
    if (is_cell == 0)
    {
        arr[first_cell] = second_cell;
        AddFormula(graph, Addcell(first_cell), second_cell, 0, 0);
        Recalc(graph, C, arr, first_cell);
    }
    else // Handle cell references
    {
        int tmp = arr[second_cell];
        arr[first_cell] = tmp;
        graph->adjLists_head[second_cell] = Addedge(first_cell, graph->adjLists_head[second_cell]);
        AddFormula(graph, Addcell(first_cell), second_cell, 0, 1);
        Recalc(graph, C, arr, first_cell);
    }
}



// void arth_op(char *a, int C, int R, int pos_equalto, int pos_end,int *arr, Graph *graph)
// {
//     int first_cell;
//     int second_cell;
//     int third_cell;
//     int res = 0;
//     int second_cell_check = 0;
//     int third_cell_check = 0;

//     first_cell = cell_parser(a, C, R, 0, pos_equalto - 1,graph);
//     int op = -1;

//     if (formulaArray[first_cell].op_type > 0)
//     {
//         Deleteedge(graph, first_cell, C);
//     }

//     for (int i = pos_equalto + 1; i < pos_end; i++)
//     {
//         if (a[i] == '+' || a[i] == '-' || a[i] == '*' || a[i] == '/')
//         {
//             op = i;
//             break;
//         }
//     }

//     if (isDigit(a[pos_equalto + 1]))
//     {
//         char *tmp2 = (char *)malloc(20 * sizeof(char));
//         for (int i = pos_equalto + 1; i < op; i++)
//         {
//             tmp2[i - pos_equalto - 1] = a[i];
//         }
//         tmp2[op - pos_equalto - 1] = '\0';
//         second_cell = atoi(tmp2);
//         free(tmp2);
//     }
//     else
//     {
//         second_cell = cell_parser(a, C, R, pos_equalto + 1, op - 1,graph);
//         second_cell_check = 1;
//     }
//     if(isDigit(a[op + 1]))
//     {
//         char *tmp3 = (char *)malloc(20 * sizeof(char));
//         for (int i = op + 1; i < pos_end; i++)
//         {
//             tmp3[i - op - 1] = a[i];
//         }
//         tmp3[pos_end - op - 1] = '\0';

//         third_cell = atoi(tmp3);
//         free(tmp3);
//     }
//     else
//     {
//     third_cell = cell_parser(a, C, R, op + 1, pos_end - 1,graph);
//     third_cell_check = 1;
//     }

//     if (first_cell == -1 || second_cell == -1 || third_cell == -1)
//     {
//         printf("Invalid command");
//         return;
//     }

//     if (second_cell_check==0 && third_cell_check==0){
//         res = arithmetic_eval(second_cell, third_cell, a[op]);
//         arr[first_cell] = res;
//         AddFormula(graph, Addcell(first_cell), res, 0, 0);
//         Recalc(graph, C, arr, first_cell);
//     }

//     else if (second_cell_check==1 && third_cell_check==0){
//         res = arithmetic_eval(arr[second_cell], third_cell, a[op]);

//         int optype= return_optype(a[op]);
//         arr[first_cell] = res;
//         graph->adjLists_head[second_cell] = Addedge(first_cell, graph->adjLists_head[second_cell]);
//         AddFormula(graph, Addcell(first_cell), second_cell, third_cell, optype);
//         Recalc(graph, C, arr, first_cell);
//     }
//     else if (second_cell_check==0 && third_cell_check==1){
//         res = arithmetic_eval(second_cell, arr[third_cell], a[op]);

//         int optype = return_optype(a[op]);
//         arr[first_cell] = res;
//         graph->adjLists_head[third_cell] = Addedge(first_cell, graph->adjLists_head[third_cell]);
//         AddFormula(graph, Addcell(first_cell), third_cell, second_cell, optype+4);
//         Recalc(graph, C, arr, first_cell);
//     }
//     else if (second_cell_check==1 && third_cell_check==1){
//         res = arithmetic_eval(arr[second_cell], arr[third_cell], a[op]);

//         int optype = return_optype(a[op]);
//         arr[first_cell] = res;
//         graph->adjLists_head[second_cell] = Addedge(first_cell, graph->adjLists_head[second_cell]);
//         graph->adjLists_head[third_cell] = Addedge(first_cell, graph->adjLists_head[third_cell]);

//         AddFormula(graph, Addcell(first_cell), second_cell, third_cell, optype+4);
//         Recalc(graph, C, arr, first_cell);
//     }
// }

void arth_op(char *a, int C, int R, int pos_equalto, int pos_end, int *arr, Graph *graph) {
    // Parse destination cell (e.g., "A1")
    int dest = cell_parser(a, C, R, 0, pos_equalto - 1, graph);
    if (dest == -1) {
        printf("Invalid command: Invalid destination cell\n");
        return;
    }

    
    if (formulaArray[dest].op_type > 0) {
        Deleteedge(graph, dest, C);
    }

    
    int i = pos_equalto + 1;
    if ((a[i] == '+' || a[i] == '-') && isDigit(a[i + 1])) {
        i++;
    }
    int op_index = -1;
    for (; i < pos_end; i++) {
        if (a[i] == '+' || a[i] == '-' || a[i] == '*' || a[i] == '/') {
            op_index = i;
            break;
        }
    }
    if (op_index == -1) {
        printf("Invalid command: Missing operator\n");
        return;
    }

    
    int len1 = op_index - (pos_equalto + 1);
    char operand1_str[50];
    strncpy(operand1_str, a + pos_equalto + 1, len1);
    operand1_str[len1] = '\0';

    int operand1_value;
    int operand1_is_cell = 0;
    if (isDigit(operand1_str[0]) || operand1_str[0] == '+' || operand1_str[0] == '-') {
        operand1_value = atoi(operand1_str);
    } else {
        operand1_value = cell_parser(a, C, R, pos_equalto + 1, op_index - 1, graph);
        operand1_is_cell = 1;
    }

    
    int len2 = pos_end - (op_index + 1);
    char operand2_str[50];
    strncpy(operand2_str, a + op_index + 1, len2);
    operand2_str[len2] = '\0';

    int operand2_value;
    int operand2_is_cell = 0;
    if (isDigit(operand2_str[0]) || operand2_str[0] == '+' || operand2_str[0] == '-') {
        operand2_value = atoi(operand2_str);
    } else {
        operand2_value = cell_parser(a, C, R, op_index + 1, pos_end - 1, graph);
        operand2_is_cell = 1;
    }

    
    if ((operand1_is_cell && operand1_value == -1) || (operand2_is_cell && operand2_value == -1)) {
        printf("Invalid command: Invalid operands\n");
        return;
    }

    char op = a[op_index];
    int res = 0;
    int optype = return_optype(op);

    
    if (!operand1_is_cell && !operand2_is_cell) {
        res = arithmetic_eval(operand1_value, operand2_value, op);
        
        AddFormula(graph, Addcell(dest), res, 0, 0);
    } else if (operand1_is_cell && !operand2_is_cell) {
        res = arithmetic_eval(arr[operand1_value], operand2_value, op);
        graph->adjLists_head[operand1_value] = Addedge(dest, graph->adjLists_head[operand1_value]);
        AddFormula(graph, Addcell(dest), operand1_value, operand2_value, optype);
    } else if (!operand1_is_cell && operand2_is_cell) {
        res = arithmetic_eval(operand1_value, arr[operand2_value], op);
        graph->adjLists_head[operand2_value] = Addedge(dest, graph->adjLists_head[operand2_value]);
        AddFormula(graph, Addcell(dest), operand2_value, operand1_value, optype + 4);
    } else {  
        res = arithmetic_eval(arr[operand1_value], arr[operand2_value], op);
        graph->adjLists_head[operand1_value] = Addedge(dest, graph->adjLists_head[operand1_value]);
        graph->adjLists_head[operand2_value] = Addedge(dest, graph->adjLists_head[operand2_value]);
        AddFormula(graph, Addcell(dest), operand1_value, operand2_value, optype + 4);
    }
    arr[dest] = res;
    Recalc(graph, C, arr, dest);
    printf("first %d\nsecond %d\n%c\n%d\n", operand1_value, operand2_value, op, res);
}




void funct(char *a, int C, int R, int pos_equalto, int pos_end, int *arr,Graph *graph)
{
    int first_cell;
    first_cell = cell_parser(a,C,R,0,pos_equalto -1,graph);


    if (first_cell  == -1)
    {
        printf("Invalid cell");
        return;
    }

    if (formulaArray[first_cell].op_type > 0)
    {
        Deleteedge(graph, first_cell, C);
    }

    char *open_paren1 = strchr(a + pos_equalto, '(');
    char *close_paren1 = strchr(a + pos_equalto, ')');

    if (!open_paren1 || !close_paren1 || close_paren1 <= open_paren1 + 1)
    {
        printf("Invalid range: Missing or misplaced parentheses\n");
        return;
    }

    int idx_open = open_paren1 - a;
    int idx_close = close_paren1 - a;


    if (idx_open - pos_equalto >= 3)
    {
        if (idx_open - pos_equalto-1 == 5)
        {
            if (a[pos_equalto + 1] == 'S' && a[pos_equalto + 2] == 'T' && a[pos_equalto + 3] == 'D' && a[pos_equalto + 4] == 'E' && a[pos_equalto + 5] == 'V')
            {
                stdev_func(a,C,R,pos_equalto,pos_end,arr,graph);
                Recalc(graph,C,arr,first_cell);
            }
            else if (a[pos_equalto + 1] == 'S' && a[pos_equalto + 2] == 'L' && a[pos_equalto + 3] == 'E' && a[pos_equalto + 4] == 'E' && a[pos_equalto + 5] == 'P')
            {
                sleep_func(a,C,R,pos_equalto,pos_end,arr,graph);
                Recalc(graph, C, arr, first_cell);
            }
        }
        else if (idx_open - pos_equalto-1 == 3)
        {
            if (a[pos_equalto + 1] == 'M' && a[pos_equalto + 2] == 'I' && a[pos_equalto + 3] == 'N')
            {
                min_func(a, C, R, pos_equalto, pos_end, arr,graph);
                Recalc(graph, C, arr, first_cell);
            }
            else if (a[pos_equalto + 1] == 'M' && a[pos_equalto + 2] == 'A' && a[pos_equalto + 3] == 'X')
            {
                maxfunc(a, C, R, pos_equalto, pos_end,arr,graph);
                Recalc(graph, C, arr, first_cell);
            }
            else if (a[pos_equalto + 1] == 'A' && a[pos_equalto + 2] == 'V' && a[pos_equalto + 3] == 'G')
            {
                avg_func(a, C, R, pos_equalto, pos_end,arr,graph);
                Recalc(graph, C, arr, first_cell);
            }
            else if (a[pos_equalto + 1] == 'S' && a[pos_equalto + 2] == 'U' && a[pos_equalto + 3] == 'M')
            {
                sum_func(a, C, R, pos_equalto, pos_end, arr, graph);
                Recalc(graph, C, arr, first_cell);
            }
        }
        else
        {
            printf("Invalid function\n");
        }
    }
    else
    {
        printf("Invalid function\n");
    }
}

// int parser(char *a, int C, int R,int *arr, Graph *graph)
// {

//     if (a[0] == 'w' || a[0] == 'd' || a[0] == 'a' || a[0] == 's')
//     {
//         ;
//     }
//     int pos_equalto = -1;
//     int pos_end = -1;

//     for (int i = 0; a[i] != '\0'; i++)
//     {
//         if (a[i] == '=' && pos_equalto == -1)
//             pos_equalto = i;
//         pos_end = i;
//     }
//     pos_end++;

//     if (pos_equalto == -1)
//     {
//         return -1;
//     }

//     int value = 0;
//     int arth_exp = 0;
//     int func = 0;

//     for (int i = pos_equalto + 1; i < pos_end; i++) {
//         if (a[i] == '(') {
//             func = 1;
//             break;
//         }
//         if (a[i] == '+' || a[i] == '-' || a[i] == '*' || a[i] == '/') {
//             arth_exp = 1;
//             break;
//         }
//     }
//     if (func == 1 && arth_exp == 1) {
//         printf("Invalid input");
//         return -1;
//     }
//     if (func == 0 && arth_exp == 0) {
//         value = 1;
//     }

//     if (value == 1) {
//         valuefunc(a, C, R, pos_equalto, pos_end, arr, graph);
//         // printf("Hello");
//     }
//     else if (arth_exp == 1) {
//         arth_op(a, C, R, pos_equalto, pos_end,arr,graph);
//     }
//     else if (func == 1) {
        
//         funct(a, C, R, pos_equalto, pos_end,arr,graph);
//     }

//     if (value == 1 || func == 1 || arth_exp == 1){
//         return 1;
//     }
//     else{
//         return 0;
//     }

// }

int parser(char *a, int C, int R, int *arr, Graph *graph)
{
    int pos_equalto = -1;
    int pos_end = -1;

    // Find position of '=' and end of input
    for (int i = 0; a[i] != '\0'; i++)
    {
        if (a[i] == '=' && pos_equalto == -1)
            pos_equalto = i;
        pos_end = i;
    }
    pos_end++;

    if (pos_equalto == -1)
    {
        printf("Invalid command: Missing '='\n");
        return -1;
    }

    int value = 0;
    int arth_exp = 0;
    int func = 0;

    // Detect type of input
    for (int i = pos_equalto + 1; i < pos_end; i++)
    {
        if (a[i] == '(')
        {
            func = 1;
            break;
        }
        if (a[i] == '+' || a[i] == '-' || a[i] == '*' || a[i] == '/')
        {
            arth_exp = 1;
            break;
        }
    }

    // Debugging output
    printf("Input: %s\n", a);
    printf("Detected type: ");
    if (value) printf("Value assignment\n");
    if (arth_exp) printf("Arithmetic expression\n");
    if (func) printf("Function\n");

    if (func == 1 && arth_exp == 1)
    {
        printf("Invalid input: Cannot be both function and arithmetic expression\n");
        return -1;
    }

    if (func == 0 && arth_exp == 0)
    {
        value = 1;
    }

    if (value == 1)
    {
        valuefunc(a, C, R, pos_equalto, pos_end, arr, graph);
    }
    else if (arth_exp == 1)
    {
        arth_op(a, C, R, pos_equalto, pos_end, arr, graph);
    }
    else if (func == 1)
    {
        funct(a, C, R, pos_equalto, pos_end, arr, graph);
    }

    return 1;
}
