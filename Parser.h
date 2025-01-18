#ifndef PARSER_H
#define PARSER_H

int cell_parser(char *a, int C, int R, int start, int end);
void value(char *a, int C, int R, int pos_equalto, int pos_end);
void arth_op(char *a, int C, int R, int pos_equalto, int pos_end);
void func(char *a, int C, int R, int pos_equalto, int pos_end, int *arr);
int parser(char *a, int C, int R, int *arr);

#endif
