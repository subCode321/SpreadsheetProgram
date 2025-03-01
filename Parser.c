#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Graph.h"
#include "Functions.h"
#include <limits.h>
#include <ctype.h>

int old_value;
int old_op_type;
int old_op_info1;
int old_op_info2;
int hasCycle;

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
    int dfound = 0;

    for (int i = start; i <= end; i++)
    {
        if (isAlpha(a[i]))
        {
            if (!dfound)
            {
                cell_col = 26 * cell_col + (a[i] - 'A' + 1);
            }
            else
            {
                return -1; // Reject if letters appear after digits
            }
        }
        else if (isDigit(a[i]))
        {
            cell_row = 10 * cell_row + (a[i] - '0');
            dfound = 1;
        }
        else
            return -1; // Reject invalid characters
    }

    cell_col -= 1; // Convert to 0-based index
    cell_row -= 1; // Convert to 0-based index

    // Reject if column or row is negative or out of bounds
    if (cell_col < 0 || cell_row < 0 || cell_col >= C || cell_row >= R)
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
int valuefunc(char *a, int C, int R, int pos_equalto, int pos_end, int *arr, Graph *graph,Formula *formulaArray)
{
    int first_cell;
    first_cell = cell_parser(a, C, R, 0, pos_equalto - 1, graph);
    if (first_cell == -1)
    {
        // printf("Invalid cell\n");
        return -1;
    }
    old_value = arr[first_cell];
    old_op_type = formulaArray[first_cell].op_type;
    old_op_info1 = formulaArray[first_cell].op_info1;
    old_op_info2 = formulaArray[first_cell].op_info2;

    // Delete existing dependencies if the cell already has a formula
    if (formulaArray[first_cell].op_type > 0)
    {
        Deleteedge(graph, first_cell, C, formulaArray);
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
        
        return -1;
    }

    if (is_negative && !is_cell) // If it's a negative constant
    {
        second_cell = -second_cell;
    }

    // Handle constant values
    if (is_cell == 0)
    {
        arr[first_cell] = second_cell;
        AddFormula(graph, first_cell, second_cell, 0, 0,formulaArray);
        Recalc(graph, C, arr, first_cell,formulaArray);
    }
    else // Handle cell references
    {
        if(is_negative==1){
            int tmp = -1*arr[second_cell];
            arr[first_cell] = tmp;
            graph->adjLists_head[second_cell] = Addedge(first_cell, graph->adjLists_head[second_cell]);
            AddFormula(graph, first_cell, second_cell, -1, 3,formulaArray);
            Recalc(graph,C,arr,first_cell,formulaArray);
        }
        else{
            int tmp = arr[second_cell];
            arr[first_cell] = tmp;
            graph->adjLists_head[second_cell] = Addedge(first_cell, graph->adjLists_head[second_cell]);
            AddFormula(graph, first_cell, second_cell, 0, 1,formulaArray);
            Recalc(graph, C, arr, first_cell,formulaArray);
        }
    }
    if (hasCycle) {
        arr[first_cell] = old_value;
        Deleteedge(graph, first_cell, C, formulaArray);

        formulaArray[first_cell].op_type = old_op_type;
        formulaArray[first_cell].op_info1 = old_op_info1;
        formulaArray[first_cell].op_info2 = old_op_info2;

        Addedge_formula(graph, first_cell, C, formulaArray);
        return -1;
    }
    // printf("%d %d\n", first_cell, second_cell);
    return 1;
}

// void arth_op(char *a, int C, int R, int pos_equalto, int pos_end,int *arr, Graph *graph)
// {
//     int first_cell;
//     int second_cell;
//     int third_cell;
//     int res = 0;
//     int second_cell_check = 0;
//     int third_cell_check = 0;

//     // first_cell = cell_parser(a, C, R, 0, pos_equalto - 1,graph);
//     // int op = -1;

//     // if (formulaArray[first_cell].op_type > 0)
//     // {
//     //     Deleteedge(graph, first_cell, C);
//     // }
//     int foundadigit=0;
//     int digitpos1 = -1;
//     int digitpos2 = -1;

//     int cellpos1 = -1;
//     int cellpos2 = -1;
//     int foundcell=0;
//     int notvalid=0;
//     char operation;
//     int opindex = -1;
//     for (int i = pos_equalto + 1; i < pos_end; i++)
//     {
//         if(isDigit(a[i])){
//             foundadigit = 1;

//         }
//         if(isAlpha(a[i])){
//             foundcell = 1;
//         }
//         if(a[i] == '+' || a[i] == '-' || a[i] == '*' || a[i] == '/' ){
//             if(foundadigit && !foundcell){
//                 operation = a[i];
//                 opindex = i;
//             }
//             else if(foundadigit && foundcell){
//                 operation = a[i];
//                 opindex =i;
//             }
//             else if(!foundadigit && foundcell){
//                 notvalid =1;
//             }
//         }

//         // if (a[i] == '+' || a[i] == '-' || a[i] == '*' || a[i] == '/')
//         // {
//         //     op = i;
//         //     break;
//         // }
//     }
//     if(notvalid){
//         printf("Invalid command");
//         return;
//     }
//     //i have the op now, now i need to get operands
//     int is1cell = 0;
//     int is1no =0;
//     int sign = 1;
//     char s[100];
//     char no[100];
//     int l = 0;

//     for(int i = pos_equalto + 1; i < opindex; i++){
//         if(a[i]=='+'){
//             ;
//         }
//         else if(a[i]=='-'){
//             sign*=-1;
//         }
//         else if(!isDigit(a[i] && !isAlpha(a[i]))){
//             notvalid =1;
//             break;
//         }
//         else if(is1no && isAlpha(a[i])){
//             notvalid =1;
//             break;
//         }
//         else if(is1cell){
//             s[l] = a[i];
//             l++;
//         }
//         else if(is1no){
//             no[l] = a[i];
//             l++;
//         }
//         else if(isAlpha(a[i])){
//             is1cell = 1;
//             s[l] = a[i];
//             if(l>5){
//                 notvalid =1;
//                 break;
//             }
//             l++;

//         }
//         else if(isDigit(a[i])){
//             is1no = 1;
//             no[l] = a[i];
//             if(l>NUM_CELLS){
//                 notvalid =1;
//                 break;
//             }
//             l++;
//         }
//         else{
//             ;
//         }
//     }
//     if(notvalid){
//         printf("Invalid command");
//         return;
//     }
//     if(is1cell){
//         //call cell parser here
//     }

//     //do same for 2nd operand

//     for (int i = pos_equalto + 1; i < pos_end; i++)
//     {
//         if(isDigit(a[i])){

//         }
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
int arth_op(char *a, int C, int R, int pos_equalto, int pos_end, int *arr, Graph *graph,Formula *formulaArray)
{
    int first_cell, second_cell, third_cell;
    int res = 0;
    int notvalid = 0;
    char operation = '\0';
    int opindex = -1;

    // Find the actual operator (not sign)
    for (int i = pos_equalto + 1; i < pos_end; i++)
    {
        if (a[i] == '+' || a[i] == '-' || a[i] == '*' || a[i] == '/')
        {
            // Check if this is a sign or operator
            if (i == pos_equalto + 1 || // Sign at start
                (i > pos_equalto + 1 && (a[i - 1] == '+' || a[i - 1] == '-' || a[i - 1] == '*' || a[i - 1] == '/')))
            {
                continue; // This is a sign, not our operator
            }
            operation = a[i];
            opindex = i;
            break;
        }
    }

    if (opindex == -1)
    {
        return -1;
    }

    // Parse first operand
    int is1cell = 0, is1num = 0;
    int sign1 = 1;
    char cell1[100] = {0};
    char num1[100] = {0};
    int l1 = 0;

    // Handle sign for first operand
    int start_pos = pos_equalto + 1;
    if (a[start_pos] == '-')
    {
        sign1 = -1;
        start_pos++;
    }
    else if (a[start_pos] == '+')
    {
        start_pos++;
    }

    // Parse first operand content
    for (int i = start_pos; i < opindex; i++)
    {
        if (!isDigit(a[i]) && !isAlpha(a[i]))
        {
            notvalid = 1;
            break;
        }
        else if (is1num && isAlpha(a[i]))
        {
            notvalid = 1;
            break;
        }
        else if (is1cell)
        {
            cell1[l1++] = a[i];
        }
        else if (is1num)
        {
            num1[l1++] = a[i];
        }
        else if (isAlpha(a[i]))
        {
            is1cell = 1;
            cell1[l1++] = a[i];
        }
        else if (isDigit(a[i]))
        {
            is1num = 1;
            num1[l1++] = a[i];
        }
    }
    // printf("%s\n", cell1);
    if(is1cell){
        cell1[l1]='\0';
        int ii=0;
        int dfound = 0;
        while(cell1[ii]!='\0'){
            if(isdigit(cell1[ii]) && !dfound){
                dfound = 1;
            }
            else if(!isdigit(cell1[ii]) && dfound){
                notvalid = 1;

            }
            ii++;
        }
        


    }

    // Parse second operand
    int is2cell = 0, is2num = 0;
    int sign2 = 1;
    char cell2[100] = {0};
    char num2[100] = {0};
    int l2 = 0;

    // Handle sign for second operand
    int second_start = opindex + 1;
    if (a[second_start] == '-')
    {
        sign2 = -1;
        second_start++;
    }
    else if (a[second_start] == '+')
    {
        second_start++;
    }

    // Parse second operand content
    for (int i = second_start; i < pos_end; i++)
    {
        if (!isDigit(a[i]) && !isAlpha(a[i]))
        {
            notvalid = 1;
            break;
        }
        else if (is2num && isAlpha(a[i]))
        {
            notvalid = 1;
            break;
        }
        else if (is2cell)
        {
            cell2[l2++] = a[i];
        }
        else if (is2num)
        {
            num2[l2++] = a[i];
        }
        else if (isAlpha(a[i]))
        {
            is2cell = 1;
            cell2[l2++] = a[i];
        }
        else if (isDigit(a[i]))
        {
            is2num = 1;
            num2[l2++] = a[i];
        }
    }

    if(is2cell){
        cell2[l2]='\0';
        int jj=0;
        int ddfound = 0;
        while(cell2[jj]!='\0'){
            if(isdigit(cell2[jj]) && !ddfound){
                ddfound = 1;
            }
            else if(!isdigit(cell2[jj]) && ddfound){
                notvalid = 1;

            }
            jj++;

        }

    }

    if (notvalid || (!is1cell && !is1num) || (!is2cell && !is2num))
    {
        return -1;
    }

    // Convert first operand
    if (is1cell)
    {
        second_cell = cell_parser(cell1, C, R, 0, l1 - 1, graph);
        if (second_cell == -1)
        {
    
            return -1;
        }
    }
    else
    {
        num1[l1] = '\0';
        second_cell = atoi(num1) * sign1;
    }

    // Convert second operand
    if (is2cell)
    {
        third_cell = cell_parser(cell2, C, R, 0, l2 - 1, graph);
        if (third_cell == -1)
        {
            // printf("%s", cell2);
            return -1;
        }
    }
    else
    {
        num2[l2] = '\0';
        third_cell = atoi(num2) * sign2;
    }

    // Parse destination cell
    first_cell = cell_parser(a, C, R, 0, pos_equalto - 1, graph);
    if (first_cell == -1)
    {
        return -1;
    }

    old_value = arr[first_cell];
    old_op_type = formulaArray[first_cell].op_type;
    old_op_info1 = formulaArray[first_cell].op_info1;
    old_op_info2 = formulaArray[first_cell].op_info2;

    // Clean up existing formula if any
    if (formulaArray[first_cell].op_type > 0)
    {
        Deleteedge(graph, first_cell, C,formulaArray);
    }

    // Handle all four cases
    if (!is1cell && !is2cell)
    { // constant op constant
        res = arithmetic_eval(second_cell, third_cell, operation);
        arr[first_cell] = res;
        AddFormula(graph, first_cell, res, 0, 0,formulaArray);
    }
    else if (is1cell && !is2cell)
    { // cell op constant
        res = arithmetic_eval(arr[second_cell], third_cell, operation);
        arr[first_cell] = res;
        graph->adjLists_head[second_cell] = Addedge(first_cell, graph->adjLists_head[second_cell]);
        AddFormula(graph, first_cell, second_cell, third_cell, return_optype(operation),formulaArray);
    }
    else if (!is1cell && is2cell)
    { // constant op cell
        res = arithmetic_eval(second_cell, arr[third_cell], operation);
        arr[first_cell] = res;
        graph->adjLists_head[third_cell] = Addedge(first_cell, graph->adjLists_head[third_cell]);
        AddFormula(graph, first_cell, third_cell, second_cell, return_optype(operation),formulaArray);
    }
    else
    { // cell op cell
        res = arithmetic_eval(arr[second_cell], arr[third_cell], operation);
        arr[first_cell] = res;
        graph->adjLists_head[second_cell] = Addedge(first_cell, graph->adjLists_head[second_cell]);
        graph->adjLists_head[third_cell] = Addedge(first_cell, graph->adjLists_head[third_cell]);
        AddFormula(graph, first_cell, second_cell, third_cell, return_optype(operation) + 4,formulaArray);
    }

    Recalc(graph, C, arr, first_cell,formulaArray);
    if (hasCycle)
    {
        arr[first_cell] = old_value;
        Deleteedge(graph, first_cell, C, formulaArray);

        formulaArray[first_cell].op_type = old_op_type;
        formulaArray[first_cell].op_info1 = old_op_info1;
        formulaArray[first_cell].op_info2 = old_op_info2;

        Addedge_formula(graph, first_cell, C, formulaArray);
        return -1;
    }
    return 1;
}
// void arth_op(char *a, int C, int R, int pos_equalto, int pos_end, int *arr, Graph *graph) {
//     // Parse destination cell (e.g., "A1")
//     int dest = cell_parser(a, C, R, 0, pos_equalto - 1, graph);
//     if (dest == -1) {
//         printf("Invalid command: Invalid destination cell\n");
//         return;
//     }

//     if (formulaArray[dest].op_type > 0) {
//         Deleteedge(graph, dest, C);
//     }

//     int i = pos_equalto + 1;
//     if ((a[i] == '+' || a[i] == '-') && isDigit(a[i + 1])) {
//         i++;
//     }
//     int op_index = -1;
//     for (; i < pos_end; i++) {
//         if (a[i] == '+' || a[i] == '-' || a[i] == '*' || a[i] == '/') {
//             op_index = i;
//             break;
//         }
//     }
//     if (op_index == -1) {
//         printf("Invalid command: Missing operator\n");
//         return;
//     }

//     int len1 = op_index - (pos_equalto + 1);
//     char operand1_str[50];
//     strncpy(operand1_str, a + pos_equalto + 1, len1);
//     operand1_str[len1] = '\0';

//     int operand1_value;
//     int operand1_is_cell = 0;
//     if (isDigit(operand1_str[0]) || operand1_str[0] == '+' || operand1_str[0] == '-') {
//         operand1_value = atoi(operand1_str);
//     } else {
//         operand1_value = cell_parser(a, C, R, pos_equalto + 1, op_index - 1, graph);
//         operand1_is_cell = 1;
//     }

//     int len2 = pos_end - (op_index + 1);
//     char operand2_str[50];
//     strncpy(operand2_str, a + op_index , len2);
//     operand2_str[len2] = '\0';

//     int operand2_value;
//     int operand2_is_cell = 0;
//     if (isDigit(operand2_str[0]) || operand2_str[0] == '+' || operand2_str[0] == '-') {
//         operand2_value = atoi(operand2_str);
//     } else {
//         operand2_value = cell_parser(a, C, R, op_index + 1, pos_end - 1, graph);
//         operand2_is_cell = 1;
//     }

//     if ((operand1_is_cell && operand1_value == -1) || (operand2_is_cell && operand2_value == -1)) {
//         printf("Invalid command: Invalid operands\n");
//         return;
//     }

//     char op = a[op_index];
//     int res = 0;
//     int optype = return_optype(op);

//     if (!operand1_is_cell && !operand2_is_cell) {
//         res = arithmetic_eval(operand1_value, operand2_value, op);

//         AddFormula(graph, Addcell(dest), res, 0, 0);
//     } else if (operand1_is_cell && !operand2_is_cell) {
//         res = arithmetic_eval(arr[operand1_value], operand2_value, op);
//         graph->adjLists_head[operand1_value] = Addedge(dest, graph->adjLists_head[operand1_value]);
//         AddFormula(graph, Addcell(dest), operand1_value, operand2_value, optype);
//     } else if (!operand1_is_cell && operand2_is_cell) {
//         res = arithmetic_eval(operand1_value, arr[operand2_value], op);
//         graph->adjLists_head[operand2_value] = Addedge(dest, graph->adjLists_head[operand2_value]);
//         AddFormula(graph, Addcell(dest), operand2_value, operand1_value, optype + 4);
//     } else {
//         res = arithmetic_eval(arr[operand1_value], arr[operand2_value], op);
//         graph->adjLists_head[operand1_value] = Addedge(dest, graph->adjLists_head[operand1_value]);
//         graph->adjLists_head[operand2_value] = Addedge(dest, graph->adjLists_head[operand2_value]);
//         AddFormula(graph, Addcell(dest), operand1_value, operand2_value, optype + 4);
//     }
//     Recalc(graph, C, arr, dest);
//     arr[dest] = res;

//     printf("first %d\nsecond %d\n%c\n%d\n", operand1_value, operand2_value, op, res);
// }

int funct(char *a, int C, int R, int pos_equalto, int pos_end, int *arr, Graph *graph,Formula *formulaArray)
{
    int first_cell;
    first_cell = cell_parser(a, C, R, 0, pos_equalto - 1, graph);

    if (first_cell == -1)
    {
        // printf("Invalid cell");
        return -1;
    }
    old_value = arr[first_cell];
    old_op_type = formulaArray[first_cell].op_type;
    old_op_info1 = formulaArray[first_cell].op_info1;
    old_op_info2 = formulaArray[first_cell].op_info2;

    if (formulaArray[first_cell].op_type > 0)
    {
        Deleteedge(graph, first_cell, C,formulaArray);
    }

    char *open_paren1 = strchr(a + pos_equalto, '(');
    char *close_paren1 = strchr(a + pos_equalto, ')');

    if (!open_paren1 || !close_paren1 || close_paren1 <= open_paren1 + 1)
    {
        // printf("Invalid range: Missing or misplaced parentheses\n");
        return -1;
    }

    int idx_open = open_paren1 - a;
    //int idx_close = close_paren1 - a;

    if (idx_open - pos_equalto >= 3)
    {
        if (idx_open - pos_equalto - 1 == 5)
        {
            if (a[pos_equalto + 1] == 'S' && a[pos_equalto + 2] == 'T' && a[pos_equalto + 3] == 'D' && a[pos_equalto + 4] == 'E' && a[pos_equalto + 5] == 'V')
            {
                stdev_func(a, C, R, pos_equalto, pos_end, arr, graph,formulaArray);
                Recalc(graph, C, arr, first_cell,formulaArray);
            }
            else if (a[pos_equalto + 1] == 'S' && a[pos_equalto + 2] == 'L' && a[pos_equalto + 3] == 'E' && a[pos_equalto + 4] == 'E' && a[pos_equalto + 5] == 'P')
            {
                sleep_func(a, C, R, pos_equalto, pos_end, arr, graph,formulaArray);
                Recalc(graph, C, arr, first_cell,formulaArray);
            }
        }
        else if (idx_open - pos_equalto - 1 == 3)
        {
            if (a[pos_equalto + 1] == 'M' && a[pos_equalto + 2] == 'I' && a[pos_equalto + 3] == 'N')
            {
                min_func(a, C, R, pos_equalto, pos_end, arr, graph,formulaArray);
                Recalc(graph, C, arr, first_cell,formulaArray);
            }
            else if (a[pos_equalto + 1] == 'M' && a[pos_equalto + 2] == 'A' && a[pos_equalto + 3] == 'X')
            {
                maxfunc(a, C, R, pos_equalto, pos_end, arr, graph,formulaArray);
                Recalc(graph, C, arr, first_cell,formulaArray);
            }
            else if (a[pos_equalto + 1] == 'A' && a[pos_equalto + 2] == 'V' && a[pos_equalto + 3] == 'G')
            {
                avg_func(a, C, R, pos_equalto, pos_end, arr, graph,formulaArray);
                Recalc(graph, C, arr, first_cell,formulaArray);
            }
            else if (a[pos_equalto + 1] == 'S' && a[pos_equalto + 2] == 'U' && a[pos_equalto + 3] == 'M')
            {
                sum_func(a, C, R, pos_equalto, pos_end, arr, graph,formulaArray);
                Recalc(graph, C, arr, first_cell,formulaArray);
            }
        }
        else
        {
            // printf("Invalid function\n");
            return -1;
        }
    }
    else
    {
        // printf("Invalid function\n");
        return -1;
    }
    if (hasCycle)
    {
        arr[first_cell] = old_value;
        Deleteedge(graph, first_cell, C, formulaArray);

        formulaArray[first_cell].op_type = old_op_type;
        formulaArray[first_cell].op_info1 = old_op_info1;
        formulaArray[first_cell].op_info2 = old_op_info2;

        Addedge_formula(graph, first_cell, C, formulaArray);
        return -1;
    }
}

int parser(char *a, int C, int R, int *arr, Graph *graph,Formula *formulaArray)
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
        return -1;
    }

    int value = 0;
    int arth_exp = 0;
    int func = 0;
    int foundadigit = 0;

    for (int i = pos_equalto + 1; i < pos_end; i++)
    {
        if (a[i] == '(')
        {
            func = 1;
            break;
        }
        if (isdigit(a[i]))
        {
            foundadigit = 1;
        }
        // if(isalplha(a[i])){
        //     foundcell =1;
        // }
        if ((a[i] == '+' || a[i] == '-' || a[i] == '*' || a[i] == '/') && (foundadigit == 1))
        {
            arth_exp = 1;
            break;
        }
    }
    if (func == 1 && arth_exp == 1)
    {
        printf("Invalid input");
        return -1;
    }
    if (func == 0 && arth_exp == 0)
    {
        value = 1;
    }

    if (value == 1)
    {
        int checker = valuefunc(a, C, R, pos_equalto, pos_end, arr, graph,formulaArray);
        if(checker == -1){
            return -1;
        }
        // printf("Hello");
    }
    else if (arth_exp == 1)
    {
        int checker = arth_op(a, C, R, pos_equalto, pos_end, arr, graph,formulaArray);
        if(checker == -1){
            return -1;
        }
    }
    else if (func == 1)
    {

        int checker = funct(a, C, R, pos_equalto, pos_end, arr, graph,formulaArray);
        if(checker == -1){
            return -1;
        }
    }

    if (value == 1 || func == 1 || arth_exp == 1)
    {
        return 1;
    }
    else
    {
        return -1;
    }
}

// int parser(char *a, int C, int R, int *arr, Graph *graph)
// {
//     int pos_equalto = -1;
//     int pos_end = -1;

//     // Find position of '=' and end of input
//     for (int i = 0; a[i] != '\0'; i++)
//     {
//         if (a[i] == '=' && pos_equalto == -1)
//             pos_equalto = i;
//         pos_end = i;
//     }
//     pos_end++;

//     if (pos_equalto == -1)
//     {
//         printf("Invalid command: Missing '='\n");
//         return -1;
//     }

//     int value = 0;
//     int arth_exp = 0;
//     int func = 0;

//     // Detect type of input
//     for (int i = pos_equalto + 1; i < pos_end; i++)
//     {
//         if (a[i] == '(')
//         {
//             func = 1;
//             break;
//         }
//         if (a[i] == '+' || a[i] == '-' || a[i] == '*' || a[i] == '/')
//         {
//             arth_exp = 1;
//             break;
//         }
//     }

//     // Debugging output
//     printf("Input: %s\n", a);
//     printf("Detected type: ");
//     if (value) printf("Value assignment\n");
//     if (arth_exp) printf("Arithmetic expression\n");
//     if (func) printf("Function\n");

//     if (func == 1 && arth_exp == 1)
//     {
//         printf("Invalid input: Cannot be both function and arithmetic expression\n");
//         return -1;
//     }

//     if (func == 0 && arth_exp == 0)
//     {
//         value = 1;
//     }

//     if (value == 1)
//     {
//         valuefunc(a, C, R, pos_equalto, pos_end, arr, graph);
//     }
//     else if (arth_exp == 1)
//     {
//         arth_op(a, C, R, pos_equalto, pos_end, arr, graph);
//     }
//     else if (func == 1)
//     {
//         funct(a, C, R, pos_equalto, pos_end, arr, graph);
//     }

//     return 1;
// }
