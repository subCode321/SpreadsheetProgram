#ifndef FUNCTIONS_H
#define FUNCTIONS_H
#include "Graph.h"

int arithmetic_eval(int v1, int v2, char op);
void min_func(char *a, int C, int R, int pos_equalto, int pos_end, int *arr, Graph *graph);
void maxfunc(char *a, int C, int R, int pos_equalto, int pos_end, int *arr, Graph *graph);
void sum_func(char *a, int C, int R, int pos_equalto, int pos_end, int *arr, Graph *graph);
void avg_func(char *a, int C, int R, int pos_equalto, int pos_end, int *arr, Graph *graph);
void stdev_func(char *a, int C, int R, int pos_equalto, int pos_end, int *arr, Graph *graph);
void sleep_func(char *a, int C, int R, int pos_equalto, int pos_end, int *arr, Graph *graph);
int return_optype(char op);

#endif