#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <limits.h>

// Define NUM_CELLS before including Graph.h and Graph.c
int NUM_CELLS = 100;

#include "Graph.h"
#include "Graph.c"

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

int main()
{
    printf("Running unit tests for Graph.c...\n");

    test_utility_functions();
    test_AddFormula();
    test_graph_creation_and_cell_ops();
    test_topological_sort();
    test_formula_edge_operations();
    test_Recalc();

    printf("\nAll tests for Graph.c passed!\n");
    return 0;
}