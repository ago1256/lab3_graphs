#pragma once    

#include "../graph/graph.h"
#include "../errors/errors.h"
#include "../structs_for_graph/structs_for_graph.h"
#include "../algorithms/algorithms.h"
    
void test_add_vertex();
void test_add_edge();
void test_remove_edge();
void test_remove_vertex();
void test_get_neighbors();
void test_vertex_edge_count();
void test_constructor_with_vertices();
void test_error_cases();
void test_remove_all_vertices();
void test_generate_graph_1();
void test_generate_graph_2();

void test_dijkstra_one_param();
void test_dijkstra_both_params();
void test_dijkstra_no_path();
void test_dijkstra_complex_graph();
void test_dijkstra_after_changes();
void test_scc();
void test_scc_complex();