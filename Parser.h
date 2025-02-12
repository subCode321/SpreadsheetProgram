#ifndef PARSER_H
#define PARSER_H
#include "Graph.h"

int cell_parser(char *a, int C, int R, int start, int end, Graph *graph);
void valuefunc(char *a, int C, int R, int pos_equalto, int pos_end, int *arr, Graph *graph);
void arth_op(char *a, int C, int R, int pos_equalto, int pos_end, int *arr, Graph *graph);
void funct(char *a, int C, int R, int pos_equalto, int pos_end, int *arr, Graph *graph);
int parser(char *a, int C, int R, int *arr, Graph *graph);

#endif
