#ifndef PARSER_H
#define PARSER_H

int cell_parser(char *a, int C, int R, int start, int end);
void valuefunc(char *a, int C, int R, int pos_equalto, int pos_end,int *arr);
void arth_op(char *a, int C, int R, int pos_equalto, int pos_end,int *arr);
void funct(char *a, int C, int R, int pos_equalto, int pos_end, int *arr);
int parser(char *a, int C, int R, int *arr);

#endif
