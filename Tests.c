#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <limits.h>
#include "Graph.h"
#include "Graph.c"
#include "Parser.c"
#include "Functions.c"
#include "display.c"
// Define NUM_CELLS before including Graph.h and Graph.c
int NUM_CELLS = 100;

// Mock variable for the external hasCycle
int hasCycle = 0;

// Test helper function to print a linked list
void printList(Cell *head)
{
    Cell *current = head;
    printf("[");
    while (current != NULL)
    {
        printf("%d", current->cell);
        current = current->next;
        if (current != NULL)
            printf(", ");
    }
    printf("]");
}

// Test helper to compare two linked lists
int listsEqual(Cell *list1, Cell *list2)
{
    while (list1 != NULL && list2 != NULL)
    {
        if (list1->cell != list2->cell)
            return 0;
        list1 = list1->next;
        list2 = list2->next;
    }
    return (list1 == NULL && list2 == NULL);
}

// Test helper to create a list with specified values
Cell *createTestList(int values[], int size)
{
    Cell *head = NULL;
    for (int i = size - 1; i >= 0; i--)
    {
        Cell *newCell = Addcell(values[i]);
        newCell->next = head;
        head = newCell;
    }
    return head;
}

// Test utility functions
void test_utility_functions()
{
    printf("Testing utility functions...\n");

    // Test min2 function
    assert(min2(5, 10) == 5);
    assert(min2(10, 5) == 5);
    assert(min2(-5, 5) == -5);

    // Test max2 function
    assert(max2(5, 10) == 10);
    assert(max2(10, 5) == 10);
    assert(max2(-5, 5) == 5);

    // Test arithmetic_eval2 function
    assert(arithmetic_eval2(5, 3, '+') == 8);
    assert(arithmetic_eval2(5, 3, '-') == 2);
    assert(arithmetic_eval2(5, 3, '*') == 15);
    assert(arithmetic_eval2(6, 3, '/') == 2);

    printf("Utility functions tests passed!\n");
}

// Test AddFormula function
void test_AddFormula()
{
    printf("Testing AddFormula function...\n");

    Graph *graph = CreateGraph();
    Formula formulaArray[NUM_CELLS];

    // Test constant formula
    AddFormula(graph, 0, 42, 0, 0, formulaArray);
    assert(formulaArray[0].op_type == 0);
    assert(formulaArray[0].op_info1 == 42);
    assert(formulaArray[0].op_info2 == -1);

    // Test cell + constant formula
    AddFormula(graph, 1, 0, 10, 1, formulaArray);
    assert(formulaArray[1].op_type == 1);
    assert(formulaArray[1].op_info1 == 0);
    assert(formulaArray[1].op_info2 == 10);

    // Test cell + cell formula
    AddFormula(graph, 2, 0, 1, 5, formulaArray);
    assert(formulaArray[2].op_type == 5);
    assert(formulaArray[2].op_info1 == 0);
    assert(formulaArray[2].op_info2 == 1);

    printf("AddFormula tests passed!\n");

    // Clean up
    for (int i = 0; i < NUM_CELLS; i++)
    {
        Cell *current = graph->adjLists_head[i];
        while (current != NULL)
        {
            Cell *temp = current;
            current = current->next;
            free(temp);
        }
    }
    free(graph->adjLists_head);
    free(graph);
}

// Test graph creation and cell operations
void test_graph_creation_and_cell_ops()
{
    printf("Testing graph creation and cell operations...\n");

    // Test CreateGraph
    Graph *graph = CreateGraph();
    assert(graph != NULL);
    assert(graph->adjLists_head != NULL);

    // Test Addcell
    Cell *cell1 = Addcell(1);
    assert(cell1 != NULL);
    assert(cell1->cell == 1);
    assert(cell1->next == NULL);

    // Test Addedge
    Cell *head = NULL;
    head = Addedge(1, head);
    assert(head != NULL);
    assert(head->cell == 1);

    // Add another edge
    head = Addedge(2, head);
    assert(head != NULL);
    assert(head->cell == 2);
    assert(head->next->cell == 1);

    // Try adding duplicate - should not change list
    Cell *before = head;
    head = Addedge(2, head);
    assert(head == before); // Should be the same pointer

    // Test Deletecell
    head = Deletecell(2, head);
    assert(head != NULL);
    assert(head->cell == 1);
    assert(head->next == NULL);

    // Delete the remaining cell
    head = Deletecell(1, head);
    assert(head == NULL);

    // Test deleting from empty list
    head = Deletecell(1, head);
    assert(head == NULL);

    free(cell1);
    free(graph->adjLists_head);
    free(graph);

    printf("Graph creation and cell operations tests passed!\n");
}

// Test topological sort and cycle detection
void test_topological_sort()
{
    printf("Testing topological sort and cycle detection...\n");

    Graph *graph = CreateGraph();

    // Create a simple DAG: 0 -> 1 -> 2
    graph->adjLists_head[0] = Addedge(1, graph->adjLists_head[0]);
    graph->adjLists_head[1] = Addedge(2, graph->adjLists_head[1]);

    int size;
    hasCycle = 0;
    int *result = topoSortFromCell(graph, 0, &size, &hasCycle);

    assert(result != NULL);
    assert(hasCycle == 0);
    assert(size == 3);
    assert(result[0] == 0);
    assert(result[1] == 1);
    assert(result[2] == 2);

    free(result);

    // Create a cycle: 0 -> 1 -> 2 -> 0
    graph->adjLists_head[2] = Addedge(0, graph->adjLists_head[2]);

    hasCycle = 0;
    result = topoSortFromCell(graph, 0, &size, &hasCycle);

    assert(result == NULL);
    assert(hasCycle == 1);

    // Clean up the adjacency lists
    for (int i = 0; i < NUM_CELLS; i++)
    {
        Cell *current = graph->adjLists_head[i];
        while (current != NULL)
        {
            Cell *temp = current;
            current = current->next;
            free(temp);
        }
        graph->adjLists_head[i] = NULL;
    }

    printf("Topological sort and cycle detection tests passed!\n");

    free(graph->adjLists_head);
    free(graph);
}

// Test formula edge operations
void test_formula_edge_operations()
{
    printf("Testing formula edge operations...\n");

    Graph *graph = CreateGraph();
    Formula formulaArray[NUM_CELLS];
    int COLS = 3; // Assuming a 3x3 grid

    // Test cell + constant formula (op_type 1)
    formulaArray[4].op_type = 1;
    formulaArray[4].op_info1 = 1;
    formulaArray[4].op_info2 = 10;

    Addedge_formula(graph, 4, COLS, formulaArray);

    // Cell 1 should have an edge to cell 4
    assert(graph->adjLists_head[1] != NULL);
    assert(graph->adjLists_head[1]->cell == 4);

    // Test cell + cell formula (op_type 5)
    formulaArray[5].op_type = 5;
    formulaArray[5].op_info1 = 1;
    formulaArray[5].op_info2 = 2;

    Addedge_formula(graph, 5, COLS, formulaArray);

    // Cell 1 should have edges to cells 4 and 5
    assert(graph->adjLists_head[1] != NULL);
    Cell *current = graph->adjLists_head[1];
    int found4 = 0, found5 = 0;
    while (current)
    {
        if (current->cell == 4)
            found4 = 1;
        if (current->cell == 5)
            found5 = 1;
        current = current->next;
    }
    assert(found4 && found5);

    // Cell 2 should have an edge to cell 5
    assert(graph->adjLists_head[2] != NULL);
    assert(graph->adjLists_head[2]->cell == 5);

    // Test range formula (op_type 9 - MIN)
    // Range covering cells 0, 1, 3, 4 (2x2 grid starting at 0)
    formulaArray[6].op_type = 9;
    formulaArray[6].op_info1 = 0; // Start cell
    formulaArray[6].op_info2 = 4; // End cell

    Addedge_formula(graph, 6, COLS, formulaArray);

    // Cells 0, 1, 3, 4 should all have edges to cell 6
    for (int cell = 0; cell <= 4; cell++)
    {
        if (cell == 2)
            continue; // Skip cell 2 as it's not in the range

        current = graph->adjLists_head[cell];
        int found6 = 0;
        while (current)
        {
            if (current->cell == 6)
            {
                found6 = 1;
                break;
            }
            current = current->next;
        }
        assert(found6);
    }

    // Test Deleteedge
    Deleteedge(graph, 4, COLS, formulaArray);

    // Cell 1 should no longer have an edge to cell 4
    current = graph->adjLists_head[1];
    found4 = 0;
    while (current)
    {
        if (current->cell == 4)
            found4 = 1;
        current = current->next;
    }
    assert(!found4);

    printf("Formula edge operations tests passed!\n");

    // Clean up
    for (int i = 0; i < NUM_CELLS; i++)
    {
        Cell *current = graph->adjLists_head[i];
        while (current != NULL)
        {
            Cell *temp = current;
            current = current->next;
            free(temp);
        }
    }
    free(graph->adjLists_head);
    free(graph);
}

// Test Recalc function
void test_Recalc()
{
    printf("Testing Recalc function...\n");

    Graph *graph = CreateGraph();
    Formula formulaArray[NUM_CELLS];
    // Fix: Initialize values array properly
    int *values = (int *)calloc(NUM_CELLS, sizeof(int));
    int COLS = 3;

    // Set up a simple spreadsheet:
    // Cell 0 = 5 (constant)
    // Cell 1 = 10 (constant)
    // Cell 2 = Cell 0 + Cell 1 (5 + 10 = 15)
    // Cell 3 = Cell 2 * 2 (15  *2 = 30)

    // Set up formulas
    AddFormula(graph, 0, 5, 0, 0, formulaArray);  // Cell 0 = 5
    AddFormula(graph, 1, 10, 0, 0, formulaArray); // Cell 1 = 10
    AddFormula(graph, 2, 0, 1, 5, formulaArray);  // Cell 2 = Cell 0 + Cell 1
    AddFormula(graph, 3, 2, 2, 3, formulaArray);  // Cell 3 = Cell 2 * 2

    // Add edges to the graph
    Addedge_formula(graph, 2, COLS, formulaArray);
    Addedge_formula(graph, 3, COLS, formulaArray);
    values[1] = 10;
    Recalc(graph, COLS, values, 0, formulaArray);

    assert(values[0] == 5);
    assert(values[1] == 10);
    assert(values[2] == 15);
    assert(values[3] == 30);

    // Modify a value and recalculate
    AddFormula(graph, 0, 7, 0, 0, formulaArray); // Change Cell 0 from 5 to 7
    Recalc(graph, COLS, values, 0, formulaArray);

    // Check the updated results
    assert(values[0] == 7);
    assert(values[1] == 10);
    assert(values[2] == 17); // Now 7 + 10
    assert(values[3] == 34); // Now 17 * 2

    // Test error propagation - division by zero
    AddFormula(graph, 4, 3, 0, 8, formulaArray); // Cell 4 = Cell 3 / Cell 0
    Addedge_formula(graph, 4, COLS, formulaArray);

    values[0] = 0; // Set Cell 0 to 0 to create division by zero in Cell 4
    Recalc(graph, COLS, values, 0, formulaArray);

    printf("Recalc function tests passed!\n");

    // Clean up
    free(values);
    for (int i = 0; i < NUM_CELLS; i++)
    {
        Cell *current = graph->adjLists_head[i];
        while (current != NULL)
        {
            Cell *temp = current;
            current = current->next;
            free(temp);
        }
    }
    free(graph->adjLists_head);
    free(graph);
}

// Test cell_parser function
void test_cell_parser()
{
    printf("Testing cell_parser function...\n");

    Graph *graph = CreateGraph();
    int C = 26;  // Columns A-Z
    int R = 100; // Rows 1-100

    // Test valid cell references
    assert(cell_parser("A1", C, R, 0, 1, graph) == 0);
    assert(cell_parser("Z100", C, R, 0, 3, graph) == 2599);

    // Test invalid cell references
    assert(cell_parser("AA1", C, R, 0, 2, graph) == -1);
    assert(cell_parser("A101", C, R, 0, 3, graph) == -1);
    assert(cell_parser("1A", C, R, 0, 1, graph) == -1);

    printf("cell_parser tests passed!\n");
    free(graph->adjLists_head);
    free(graph);
}

// Test valuefunc function
void test_valuefunc()
{
    printf("Testing valuefunc function...\n");

    Graph *graph = CreateGraph();
    Formula formulaArray[NUM_CELLS];
    int *arr = (int *)calloc(NUM_CELLS, sizeof(int));
    int C = 26;  // Columns A-Z
    int R = 100; // Rows 1-100

    // Test constant assignment
    char input1[] = "A1=42";
    valuefunc(input1, C, R, 2, 5, arr, graph, formulaArray);
    assert(arr[0] == 42);

    // Test cell reference
    char input2[] = "B1=A1";
    valuefunc(input2, C, R, 2, 5, arr, graph, formulaArray);
    assert(arr[1] == 42);

    // Test negative value
    char input3[] = "C1=-10";
    valuefunc(input3, C, R, 2, 6, arr, graph, formulaArray);
    assert(arr[2] == -10);

    printf("valuefunc tests passed!\n");
    free(arr);
    free(graph->adjLists_head);
    free(graph);
}

// Test arth_op function
void test_arth_op()
{
    printf("Testing arth_op function...\n");

    Graph *graph = CreateGraph();
    Formula formulaArray[NUM_CELLS];
    int *arr = (int *)calloc(NUM_CELLS, sizeof(int));
    int C = 26;  // Columns A-Z
    int R = 100; // Rows 1-100

    // Set up initial values
    arr[0] = 10; // A1 = 10
    arr[1] = 5;  // B1 = 5

    // Test addition
    char input1[] = "C1=A1+B1";
    arth_op(input1, C, R, 2, 8, arr, graph, formulaArray);
    assert(arr[2] == 15);

    // Test subtraction
    char input2[] = "D1=A1-B1";
    arth_op(input2, C, R, 2, 8, arr, graph, formulaArray);
    assert(arr[3] == 5);

    // Test multiplication
    char input3[] = "E1=A1*B1";
    arth_op(input3, C, R, 2, 8, arr, graph, formulaArray);
    assert(arr[4] == 50);

    // Test division
    char input4[] = "F1=A1/B1";
    arth_op(input4, C, R, 2, 8, arr, graph, formulaArray);
    assert(arr[5] == 2);

    printf("arth_op tests passed!\n");
    free(arr);
    free(graph->adjLists_head);
    free(graph);
}

// Test min_func function
void test_min_func()
{
    printf("Testing min_func function...\n");

    Graph *graph = CreateGraph();
    int *arr = (int *)calloc(NUM_CELLS, sizeof(int));
    Formula formulaArray[NUM_CELLS];

    // Initialize test data
    arr[0] = 5; // A1
    arr[1] = 2; // B1
    arr[2] = 8; // C1
    arr[3] = 1; // A2
    arr[4] = 9; // B2
    arr[5] = 3; // C2
    arr[6] = 7; // A3
    arr[7] = 4; // B3
    arr[8] = 6; // C3

    char input[] = "C3=MIN(A1:B2)"; // Avoids self-cycle
    int C = 3, R = 3;
    int pos_equalto = 2;
    int pos_end = strlen(input);

    min_func(input, C, R, pos_equalto, pos_end, arr, graph, formulaArray);

    assert(arr[8] == 1); // C3 should have the minimum value from range A1:B2

    printf("min_func tests passed!\n");

    // Clean up
    free(arr);
    free(graph->adjLists_head);
    free(graph);
}

// Test maxfunc function
void test_max_func()
{
    printf("Testing max_func function...\n");

    Graph *graph = CreateGraph();
    int *arr = (int *)calloc(NUM_CELLS, sizeof(int));
    Formula formulaArray[NUM_CELLS];

    // Initialize test data
    arr[0] = 5; // A1
    arr[1] = 2; // B1
    arr[2] = 8; // C1
    arr[3] = 1; // A2
    arr[4] = 9; // B2
    arr[5] = 3; // C2
    arr[6] = 7; // A3
    arr[7] = 4; // B3
    arr[8] = 6; // C3

    char input[] = "C3=MAX(A1:B2)"; // Avoids self-cycle
    int C = 3, R = 3;
    int pos_equalto = 2;
    int pos_end = strlen(input);

    maxfunc(input, C, R, pos_equalto, pos_end, arr, graph, formulaArray);

    assert(arr[8] == 9); // C3 should have the maximum value from range A1:B2

    printf("max_func tests passed!\n");

    // Clean up
    free(arr);
    free(graph->adjLists_head);
    free(graph);
}

// Test avg_func function
void test_avg_func()
{
    printf("Testing avg_func function...\n");

    Graph *graph = CreateGraph();
    int *arr = (int *)calloc(NUM_CELLS, sizeof(int));
    Formula formulaArray[NUM_CELLS];

    // Initialize test data
    arr[0] = 1; // A1
    arr[1] = 2; // B1
    arr[2] = 3; // C1
    arr[3] = 4; // A2
    arr[4] = 5; // B2
    arr[5] = 6; // C2
    arr[6] = 7; // A3
    arr[7] = 8; // B3
    arr[8] = 9; // C3

    char input[] = "C3=AVG(A1:B2)"; // Avoids self-cycle
    int C = 3, R = 3;
    int pos_equalto = 2;
    int pos_end = strlen(input);

    avg_func(input, C, R, pos_equalto, pos_end, arr, graph, formulaArray);

    assert(arr[8] == 3); // C3 should have the average value from range A1:B2

    printf("avg_func tests passed!\n");

    // Clean up
    free(arr);
    free(graph->adjLists_head);
    free(graph);
}

// Test sum_func function
void test_sum_func()
{
    printf("Testing sum_func function...\n");

    Graph *graph = CreateGraph();
    int *arr = (int *)calloc(NUM_CELLS, sizeof(int));
    Formula formulaArray[NUM_CELLS];

    // Initialize test data
    arr[0] = 1; // A1
    arr[1] = 2; // B1
    arr[2] = 3; // C1
    arr[3] = 4; // A2
    arr[4] = 5; // B2
    arr[5] = 6; // C2
    arr[6] = 7; // A3
    arr[7] = 8; // B3
    arr[8] = 9; // C3

    char input[] = "C3=SUM(A1:B2)"; // Avoids self-cycle
    int C = 3, R = 3;
    int pos_equalto = 2;
    int pos_end = strlen(input);

    sum_func(input, C, R, pos_equalto, pos_end, arr, graph, formulaArray);

    assert(arr[8] == 12); // C3 should have the sum of values from range A1:B2

    printf("sum_func tests passed!\n");

    // Clean up
    free(arr);
    free(graph->adjLists_head);
    free(graph);
}

// Test stdev_func function
void test_stdev_func()
{
    printf("Testing stdev_func function...\n");

    Graph *graph = CreateGraph();
    int *arr = (int *)calloc(NUM_CELLS, sizeof(int));
    Formula formulaArray[NUM_CELLS];

    // Initialize test data
    arr[0] = 2; // A1
    arr[1] = 4; // B1
    arr[2] = 4; // C1
    arr[3] = 4; // A2
    arr[4] = 5; // B2
    arr[5] = 5; // C2
    arr[6] = 7; // A3
    arr[7] = 9; // B3
    arr[8] = 6; // C3

    char input[] = "C3=STDEV(A1:B2)"; // Avoids self-cycle
    int C = 3, R = 3;
    int pos_equalto = 2;
    int pos_end = strlen(input);

    stdev_func(input, C, R, pos_equalto, pos_end, arr, graph, formulaArray);

    assert(arr[8] == 1); // C3 should have approx std deviation of [2,4,4,5] rounded

    printf("stdev_func tests passed!\n");

    // Clean up
    free(arr);
    free(graph->adjLists_head);
    free(graph);
}

// Test sleep_func function
void test_sleep_func()
{
    printf("Testing sleep_func function...\n");

    Graph *graph = CreateGraph();
    int *arr = (int *)calloc(NUM_CELLS, sizeof(int));
    Formula formulaArray[NUM_CELLS];

    char input[] = "C3=SLEEP(1)"; // No self-cycle issue here
    int C = 3, R = 3;
    int pos_equalto = 2;
    int pos_end = strlen(input);

    sleep_func(input, C, R, pos_equalto, pos_end, arr, graph, formulaArray);

    assert(arr[8] == 1); // C3 should have the value 1

    printf("sleep_func tests passed!\n");

    // Clean up
    free(arr);
    free(graph->adjLists_head);
    free(graph);
}
// Test for scroller() function with 'w' command (scroll up)
void test_scroller_w_command()
{
    printf("Testing scroller with 'w' command...\n");

    Graph *graph = CreateGraph();
    int *arr = (int *)calloc(100, sizeof(int)); // 10x10 grid for simplicity
    int currx = 0, curry = 5;
    int C = 10, R = 10;

    // Test scrolling up when possible
    char cmd1[] = "w";
    scroller(cmd1, arr, &currx, &curry, C, R, graph);
    assert(curry == 0); // Should scroll to top since curry < 10

    // Test scrolling up when already at top
    char cmd2[] = "w";
    scroller(cmd2, arr, &currx, &curry, C, R, graph);
    assert(curry == 0); // Should remain at top

    // Test scrolling up from position > 10
    curry = 15;
    char cmd3[] = "w";
    scroller(cmd3, arr, &currx, &curry, C, R, graph);
    assert(curry == 5); // Should scroll up by 10

    free(arr);
    free(graph->adjLists_head);
    free(graph);

    printf("scroller 'w' command tests passed!\n");
}

// Test for scroller() function with 's' command (scroll down)
void test_scroller_s_command()
{
    printf("Testing scroller with 's' command...\n");

    Graph *graph = CreateGraph();
    int *arr = (int *)calloc(400, sizeof(int)); // 20x20 grid
    int currx = 0, curry = 0;
    int C = 20, R = 20;

    // Test scrolling down when possible
    char cmd1[] = "s";
    scroller(cmd1, arr, &currx, &curry, C, R, graph);
    assert(curry == 10); // Should scroll down by 10
    // Test scrolling down near bottom
    curry = 5;
    char cmd2[] = "s";
    scroller(cmd2, arr, &currx, &curry, C, R, graph);
    assert(curry == 10); // Should scroll down by 5

    // Test scrolling down when at bottom
    curry = 10;
    char cmd3[] = "s";
    scroller(cmd3, arr, &currx, &curry, C, R, graph);
    assert(curry == 10); // Should remain at 10 since scrolling would go out of bounds

    free(arr);
    free(graph->adjLists_head);
    free(graph);

    printf("scroller 's' command tests passed!\n");
}

// Test for scroller() function with 'a' command (scroll left)
void test_scroller_a_command()
{
    printf("Testing scroller with 'a' command...\n");

    Graph *graph = CreateGraph();
    int *arr = (int *)calloc(400, sizeof(int)); // 20x20 grid
    int currx = 5, curry = 0;
    int C = 20, R = 20;

    // Test scrolling left when possible
    char cmd1[] = "a";
    scroller(cmd1, arr, &currx, &curry, C, R, graph);
    assert(currx == 0); // Should scroll to leftmost since currx < 10

    // Test scrolling left when already at leftmost
    char cmd2[] = "a";
    scroller(cmd2, arr, &currx, &curry, C, R, graph);
    assert(currx == 0); // Should remain at leftmost

    // Test scrolling left from position > 10
    currx = 15;
    char cmd3[] = "a";
    scroller(cmd3, arr, &currx, &curry, C, R, graph);
    assert(currx == 5); // Should scroll left by 10

    free(arr);
    free(graph->adjLists_head);
    free(graph);

    printf("scroller 'a' command tests passed!\n");
}

// Test for scroller() function with 'd' command (scroll right)
void test_scroller_d_command()
{
    printf("Testing scroller with 'd' command...\n");

    Graph *graph = CreateGraph();
    int *arr = (int *)calloc(400, sizeof(int)); // 20x20 grid
    int currx = 0, curry = 0;
    int C = 20, R = 20;

    // Test scrolling right when possible
    char cmd1[] = "d";
    scroller(cmd1, arr, &currx, &curry, C, R, graph);
    assert(currx == 10); // Should scroll right by 10

    // Test scrolling right with less than 10 columns remaining
    char cmd2[] = "d";
    scroller(cmd2, arr, &currx, &curry, C, R, graph);
    assert(currx == 10); // Should remain at 10 since 10+10 = 20 (grid width)

    // Test in a wider grid
    C = 30;
    char cmd3[] = "d";
    scroller(cmd3, arr, &currx, &curry, C, R, graph);
    assert(currx == 20); // Should scroll right by 10

    free(arr);
    free(graph->adjLists_head);
    free(graph);

    printf("scroller 'd' command tests passed!\n");
}

// Test for scroller() function with 'scroll_to' command
void test_scroller_scroll_to_command()
{
    printf("Testing scroller with 'scroll_to' command...\n");

    Graph *graph = CreateGraph();
    int *arr = (int *)calloc(400, sizeof(int)); // 20x20 grid
    int currx = 0, curry = 0;
    int C = 20, R = 20;

    // Mock the cell_parser function using a function pointer if necessary
    // For now, we'll assume it works correctly and test valid scroll_to commands

    // Test valid scroll_to command
    char cmd1[] = "scroll_to A1";
    scroller(cmd1, arr, &currx, &curry, C, R, graph);
    // We can't assert the exact behavior here without mocking cell_parser
    // But we can run the function to check for crashes

    // Test invalid scroll target (out of bounds)
    char cmd2[] = "scroll_to Z99"; // Assuming this is out of bounds
    int old_x = currx;
    int old_y = curry;
    scroller(cmd2, arr, &currx, &curry, C, R, graph);
    // Coordinates should not change on invalid scroll

    // Test unrecognized command
    char cmd3[] = "invalid";
    scroller(cmd3, arr, &currx, &curry, C, R, graph);

    free(arr);
    free(graph->adjLists_head);
    free(graph);

    printf("scroller 'scroll_to' command tests passed!\n");
}

// Test for printer() function
void test_printer_function()
{
    printf("Testing printer function...\n");

    int C = 20, R = 20;
    int *arr = (int *)calloc(C * R, sizeof(int));

    // Initialize some test values
    for (int i = 0; i < C * R; i++)
    {
        arr[i] = i;
    }

    // Set a few cells to INT_MIN to test error display
    arr[5] = INT_MIN;
    arr[25] = INT_MIN;

    // Test printing from different starting positions
    // We can't automatically verify the output, but we can verify it doesn't crash

    // Print from top-left
    printf("Printing from top-left (0,0):\n");
    printer(0, 0, arr, C, R);

    // Print from middle
    printf("\nPrinting from middle (5,5):\n");
    printer(5, 5, arr, C, R);

    // Print near edge
    printf("\nPrinting near edge (15,15):\n");
    printer(15, 15, arr, C, R);

    free(arr);

    printf("printer function tests passed!\n");
}

// Main function to run all tests
int main()
{
    printf("Running unit tests for Graph.c, Parser.c, Functions.c, and display.c...\n\n");

    // Tests for utility functions
    test_utility_functions();

    // Tests for Graph.c
    test_AddFormula();
    test_graph_creation_and_cell_ops();
    test_topological_sort();
    test_formula_edge_operations();
    test_Recalc();

    // Tests for Parser.c
    test_cell_parser();
    test_valuefunc();
    test_arth_op();

    // Tests for Functions.c
    test_min_func();
    test_max_func();
    test_avg_func();
    test_sum_func();
    test_stdev_func();
    test_sleep_func();

    // Tests for display.c
    test_scroller_w_command();
    test_scroller_s_command();
    test_scroller_a_command();
    test_scroller_d_command();
    test_scroller_scroll_to_command();
    test_printer_function();

    printf("\nAll tests for Graph.c, Parser.c, Functions.c, and display.c passed!\n");
    return 0;
}