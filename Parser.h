#ifndef PARSER_H
#define PARSER_H
#include "Graph.h"

int cell_parser(char *a, int C, int R, int start, int end, Graph *graph);
void valuefunc(char *a, int C, int R, int pos_equalto, int pos_end, int *arr, Graph *graph,Formula *formulaArray);
void arth_op(char *a, int C, int R, int pos_equalto, int pos_end, int *arr, Graph *graph,Formula *formulaArray);
void funct(char *a, int C, int R, int pos_equalto, int pos_end, int *arr, Graph *graph,Formula *formulaArray);
int parser(char *a, int C, int R, int *arr, Graph *graph,Formula *formulaArray);

#endif
