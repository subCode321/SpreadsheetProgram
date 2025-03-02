#ifndef FUNCTIONS_H
#define FUNCTIONS_H
#include "Graph.h"

int std(int *arr, int n);
int arithmetic_eval(int v1, int v2, char op);
int min_func(char *a, int C, int R, int pos_equalto, int pos_end, int *arr, Graph *graph,Formula *formulaArray);
int maxfunc(char *a, int C, int R, int pos_equalto, int pos_end, int *arr, Graph *graph, Formula *formulaArray);
int sum_func(char *a, int C, int R, int pos_equalto, int pos_end, int *arr, Graph *graph, Formula *formulaArray);
int avg_func(char *a, int C, int R, int pos_equalto, int pos_end, int *arr, Graph *graph, Formula *formulaArray);
int stdev_func(char *a, int C, int R, int pos_equalto, int pos_end, int *arr, Graph *graph, Formula *formulaArray);
int sleep_func(char *a, int C, int R, int pos_equalto, int pos_end, int *arr, Graph *graph, Formula *formulaArray);
int return_optype(char op);

#endif