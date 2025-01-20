#ifndef FUNCTIONS_H
#define FUNCTIONS_H

int arithmetic_eval(int v1, int v2, char op);
void min_func(char *a, int C, int R, int pos_equalto, int pos_end, int *arr);
void maxfunc(char *a, int C, int R, int pos_equalto, int pos_end, int *arr);
void sum_func(char *a, int C, int R, int pos_equalto, int pos_end, int *arr);
void avg_func(char *a, int C, int R, int pos_equalto, int pos_end, int *arr);
void stdev_func(char *a, int C, int R, int pos_equalto, int pos_end, int *arr);
void sleep_func(char *a, int C, int R, int pos_equalto, int pos_end, int *arr);

#endif