#ifndef GRAPH_H
#define GRAPH_H
extern int NUM_CELLS;

// Cell structure for adjacency list
typedef struct Cell
{
    int cell;
    struct Cell *next;
} Cell;

// Range structure to store range endpoints
typedef struct Range
{
    int startCell;
    int endCell;
    int dependentCell;
    struct Range *next;
} Range;

// Graph structure
typedef struct
{
    Cell **adjLists_head;
    Range *ranges_head; // New field to store ranges
} Graph;

// Formula structure
typedef struct
{
    int op_type;
    int op_info1;
    int op_info2;
} Formula;

// Graph operations
Graph *CreateGraph();
void FreeGraph(Graph *graph);
Cell *Addedge_formula(Graph *graph, int cell, int COLS, Formula *formulaArray);
Cell *Deleteedge(Graph *graph, int cell, int COLS, Formula *formulaArray);
void Recalc(Graph *graph, int C, int *arr, int startCell, Formula *formulaArray);
void AddFormula(Graph *graph, int cell, int c1, int c2, int op_type, Formula *formulaArray);
Cell *Addedge(int cell1, Cell *head);
void AddRangeToGraph(Graph *graph, int startCell, int endCell, int dependentCell);
void DeleteRangeFromGraph(Graph *graph, int dependentCell);
#endif // GRAPH_H