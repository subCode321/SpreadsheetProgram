#ifndef PARSER_H
#define PARSER_H
#include "Graph.h"
#include "Functions.h"

int cell_parser(char *a, int C, int R, int start, int end, Graph *graph);
int valuefunc(char *a, int C, int R, int pos_equalto, int pos_end, int *arr, Graph *graph,Formula *formulaArray);
int arth_op(char *a, int C, int R, int pos_equalto, int pos_end, int *arr, Graph *graph,Formula *formulaArray);
int funct(char *a, int C, int R, int pos_equalto, int pos_end, int *arr, Graph *graph,Formula *formulaArray);
int parser(char *a, int C, int R, int *arr, Graph *graph,Formula *formulaArray);

extern int old_value;
extern int old_op_type;
extern int old_op_info1;
extern int old_op_info2;
extern int hasCycle;

#endif
