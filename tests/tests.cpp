#include "tests.h"
#include <cassert>
#include <iostream>
#include <ctime>
#include <cstdlib>

void test_add_vertex() {
    std::cout << "Test add_vertex: ";
    Graph g;
    
    g.add_vertex("A");
    assert(g.has_vertex("A"));
    assert(g.vertex_count() == 1);
    assert(!g.has_vertex("C"));

    g.add_vertex("B");
    g.add_vertex("C");
    assert(g.vertex_count() == 3);
    
    std::cout << "pass\n";
}

void test_add_edge() {
    std::cout << "Test add_edge: ";
    Graph g;
    
    g.add_vertex("A");
    g.add_vertex("B");
    g.add_vertex("C");
    
    std::vector<int> data = {5, 10};
    g.add_edge("A", "B", data);
    
    assert(g.has_edge("A", "B"));
    assert(g.edge_count() == 1);
    assert(!g.has_edge("B", "A"));
    
    Edge& edge = g.get_edge("A", "B");
    assert(edge.params[0] == 5);
    assert(edge.params[1] == 10);
    
    g.add_edge("B", "C");
    g.add_edge("B", "A");
    g.add_edge("C", "A");
    assert(g.edge_count() == 4);
    
    std::cout << "pass\n";
}

void test_remove_edge() {
    std::cout << "Test remove_edge: ";
    Graph g;
    
    g.add_vertex("A");
    g.add_vertex("B");
    g.add_vertex("C");
    
    g.add_edge("A", "B");
    g.add_edge("B", "C");
    g.add_edge("C", "A");
    
    assert(g.edge_count() == 3);
    assert(g.has_edge("A", "B"));

    g.remove_edge("A", "B");
    assert(g.edge_count() == 2);
    assert(!g.has_edge("A", "B"));
    assert(g.has_edge("B", "C"));
    assert(g.has_edge("C", "A"));
    
    std::cout << "pass\n";
}

void test_remove_vertex() {
    std::cout << "Test remove_vertex: ";
    Graph g;
    
    g.add_vertex("A");
    g.add_vertex("B");
    g.add_vertex("C");
    g.add_vertex("D");
    
    g.add_edge("A", "B");
    g.add_edge("B", "C");
    g.add_edge("C", "A");
    g.add_edge("A", "D"); 
    
    assert(g.vertex_count() == 4);
    assert(g.edge_count() == 4);
    
    g.remove_vertex("A");
    
    assert(g.vertex_count() == 3);
    assert(g.edge_count() == 1);  
    assert(g.has_edge("B", "C"));
    assert(!g.has_vertex("A"));
    assert(!g.has_edge("A", "B"));
    assert(!g.has_edge("C", "A"));
    assert(!g.has_edge("A", "D"));

    g.remove_vertex("B");
    g.remove_vertex("C");
    g.remove_vertex("D");
    assert(g.empty() == true);
    g.print();
    
    std::cout << "pass\n";
}

void test_get_neighbors() {
    std::cout << "Test get_neighbors: ";
    Graph g;
    
    g.add_vertex("A");
    g.add_vertex("B");
    g.add_vertex("C");
    g.add_vertex("D");
    g.add_vertex("E");
    
    g.add_edge("A", "B");
    g.add_edge("A", "C");
    g.add_edge("B", "C");
    g.add_edge("C", "A");
    g.add_edge("D", "A");
    
    auto from_A_to = g.get_to_vertices("A");
    assert(from_A_to.size() == 2);
    
    bool found_B = false;
    bool found_C = false;
    for (const auto& vertex : from_A_to) {
        if (vertex == "B") found_B = true;
        if (vertex == "C") found_C = true;
    }
    assert(found_B);
    assert(found_C);
    
    auto from_to_A = g.get_from_vertices("A");
    assert(from_to_A.size() == 2);
    
    found_C = false;
    bool found_D = false;
    for (const auto& vertex : from_to_A) {
        if (vertex == "C") found_C = true;
        if (vertex == "D") found_D = true;
    }
    assert(found_C);
    assert(found_D);
    
    auto neighbors_A = g.get_neighbors("A");
    g.print();
    assert(neighbors_A.size() == 3);
    
    bool has_B = false;
    bool has_C = false;
    bool has_D = false;
    for (const auto& n : neighbors_A) {
        if (n == "B") has_B = true;
        if (n == "C") has_C = true;
        if (n == "D") has_D = true;
    }
    assert(has_B && has_C && has_D);
    
    assert(g.get_to_vertices("E").empty());
    assert(g.get_from_vertices("E").empty());
    assert(g.get_neighbors("E").empty());
    
    std::cout << "pass\n";
}

void test_vertex_edge_count() {
    std::cout << "Test vertex edge count: ";
    Graph g;
    
    assert(g.empty());
    assert(g.vertex_count() == 0);
    assert(g.edge_count() == 0);
    
    g.add_vertex("A");
    assert(!g.empty());
    assert(g.vertex_count() == 1);
    
    g.add_vertex("B");
    g.add_vertex("C");
    assert(g.vertex_count() == 3);
    
    g.add_edge("A", "B");
    g.add_edge("B", "C");
    assert(g.edge_count() == 2);
    
    g.clear();
    assert(g.empty());
    assert(g.vertex_count() == 0);
    assert(g.edge_count() == 0);
    
    std::cout << "pass\n";
}

void test_remove_all_vertices() {
    std::cout << "Test remove all vertices than add: ";
    Graph g;
    
    g.add_vertex("A");
    g.add_vertex("B");
    g.add_vertex("C");
    assert(g.vertex_count() == 3);
    
    g.add_edge("A", "B");
    g.add_edge("B", "C");
    assert(g.edge_count() == 2);
    
    g.remove_vertex("A");
    g.remove_vertex("B");
    g.remove_vertex("C");
    assert(g.empty());

    g.add_vertex("A");
    g.add_vertex("V");
    g.add_edge("V", "A");
    assert(g.vertex_count() == 2);
    assert(g.edge_count() == 1);
    
    std::cout << "pass\n";
}

void test_constructor_with_vertices() {
    std::cout << "Test constructor with vertices: ";
    
    std::vector<std::string> cities = {"A", "B", "C", "D"};
    Graph g(cities);
    
    assert(g.vertex_count() == 4);
    assert(g.has_vertex("D"));
    assert(g.has_vertex("A"));
    assert(g.has_vertex("B"));
    assert(g.has_vertex("C"));
    assert(!g.has_vertex("F"));
    
    std::cout << "pass\n";
}

void test_error_cases() {
    std::cout << "Test error cases: ";
    Graph g;
    
    try {
        g.get_vertex("A");
        assert(false);  
    } catch (const Error& e) {
        assert(e == Error::VERTEX_NOT_FOUND);
    }
    
    g.add_vertex("A");
    g.add_vertex("C");
    
    try {
        g.add_vertex("A");
        assert(false);  
    } catch (const Error& e) {
        assert(e == Error::VERTEX_ALREADY_EXISTS);
    }

    try {
        g.get_edge("A", "C");
        assert(false); 
    } catch (const Error& e) {
        assert(e == Error::EDGE_NOT_FOUND);
    }
    
    g.add_edge("A", "C", {2, 3, 3});
    g.print();
    
    std::vector<int> exp_params = {2, 3, 4};
    g.add_edge("A", "C", exp_params);
    std::vector<int> new_params = g.get_edge("A", "C").params;
    assert(new_params == exp_params);
    g.print();
    
    std::cout << "pass\n";
}

void test_dijkstra_one_param() {
    std::cout << "Test dijkstra one parameter: ";
    Graph g;
    
    g.add_vertex("A");
    g.add_vertex("B");
    g.add_vertex("C");
    g.add_vertex("D");
    
    g.add_edge("A", "B", {5, 10});
    g.add_edge("A", "D", {10, 5});
    g.add_edge("B", "C", {3, 8});    
    g.add_edge("D", "C", {2, 12});  
    
    std::vector<bool> first_param_only = {true, false};
    Path path1 = dijkstra_alg(g, "A", "C", first_param_only, {1, 1, 1});
    
    g.print();
    print_path(path1);
    
    assert(!path1.verts.empty()); 
    assert(path1.verts[0] == "A");
    assert(path1.verts.back() == "C");
    
    assert(path1.cost[0] == 8);   
    assert(path1.cost[1] == 18);  
    assert(path1.weighted_sum == 8.0);
    
    std::vector<bool> second_param_only = {false, true};
    Path path2 = dijkstra_alg(g, "A", "C", second_param_only,  {1, 1, 1});
    
    std::cout << "Path considering only second parameter: ";
    print_path(path2);
    
    assert(!path2.verts.empty());
    assert(path2.cost[0] == 12); 
    assert(path2.cost[1] == 17);
    assert(path2.weighted_sum == 17.0); 
    
    std::cout << "pass\n";
}

void test_dijkstra_both_params() {
    std::cout << "Test dijkstra both params: ";
    Graph g;

    g.add_vertex("A");
    g.add_vertex("B");
    g.add_vertex("C");
    g.add_vertex("D");
    
    g.add_edge("A", "B", {5, 10});
    g.add_edge("A", "D", {10, 5});
    g.add_edge("B", "C", {3, 8});
    g.add_edge("D", "C", {2, 12});
    
    std::vector<bool> both_params = {true, true};
    Path best_path = dijkstra_alg(g, "A", "C", both_params, {1, 1, 1});
    
    std::cout << "\nBest path considering both parameters: ";
    print_path(best_path);
    
    assert(best_path.verts.size() == 3);
    assert(best_path.verts[0] == "A");
    assert(best_path.verts[1] == "B");
    assert(best_path.verts[2] == "C");
    assert(best_path.cost[0] == 8);
    assert(best_path.cost[1] == 18); 
    assert(best_path.weighted_sum == 26.0);
    
    std::cout << "pass\n";
}

void test_dijkstra_with_weights() {
    std::cout << "Test dijkstra with weights: ";
    Graph g;
    
    g.add_vertex("A");
    g.add_vertex("B");
    g.add_vertex("C");
    
    g.add_edge("A", "B", {10, 100});
    g.add_edge("A", "C", {15, 80});   
    g.add_edge("B", "C", {20, 50}); 
    
    std::vector<bool> both_params = {true, true};
    Path path = dijkstra_alg(g, "A", "C", both_params, {0.7, 0.3});
    
    print_path(path);
    
    assert(!path.verts.empty());
    assert(path.verts[0] == "A");
    assert(path.verts[1] == "C");
    assert(path.cost[0] == 15);
    assert(path.cost[1] == 80);
    assert(path.weighted_sum == 34.5);
    
    std::cout << "pass\n";
}

void test_dijkstra_no_path() {
    std::cout << "Test dijkstra no path: ";
    Graph g;
    
    g.add_vertex("A");
    g.add_vertex("B");
    g.add_vertex("C");
    
    g.add_edge("A", "B", {5, 10});
    
    std::vector<bool> params = {true, true};
    Path path = dijkstra_alg(g, "A", "C", params,  {1, 1, 1});
    
    assert(path.verts.empty()); 
    std::cout << "pass\n";
}

void test_dijkstra_after_changes() {
    std::cout << "Test dijkstra after changes: ";
    Graph g;
    
    g.add_vertex("A");
    g.add_vertex("B");
    g.add_vertex("C");
    g.add_vertex("D");
    g.add_vertex("E");

    g.add_edge("A", "B", {1, 4});
    g.add_edge("A", "C", {4, 1});
    g.add_edge("B", "D", {2, 2});
    g.add_edge("D", "C", {1, 2}); 
    g.add_edge("D", "E", {1, 1});
    g.add_edge("B", "E", {5, 5});
    g.add_edge("C", "E", {5, 5});
    
    std::vector<bool> first_param_only = {true, false};
    Path path1 = dijkstra_alg(g, "A", "C", first_param_only,  {1, 1, 1});
    
    print_path(path1);
    
    assert(!path1.verts.empty());
    assert(path1.cost[0] == 4);  
    
    g.remove_edge("A", "B");
    g.add_edge("A", "C", {3, 5}); 
    
    g.print();
    
    Path path2 = dijkstra_alg(g, "A", "C", first_param_only, {1, 1, 1});
    print_path(path2);
    
    assert(!path2.verts.empty());
    assert(path2.verts.size() == 2);  
    assert(path2.verts[0] == "A");
    assert(path2.verts[1] == "C");
    assert(path2.cost[0] == 3);
    assert(path2.cost[1] == 5);  
    
    std::cout << "pass\n";
}

void test_dijkstra_complex_graph() {
    std::cout << "Test dijkstra complex graph: ";
    Graph g;
    
    g.add_vertex("A");
    g.add_vertex("B");
    g.add_vertex("C");
    g.add_vertex("D");
    g.add_vertex("E");

    g.add_edge("A", "B", {8, 7});
    g.add_edge("A", "C", {4, 1});
    g.add_edge("B", "D", {2, 2});
    g.add_edge("C", "D", {2, 2});
    g.add_edge("D", "E", {1, 1});
    g.add_edge("B", "E", {5, 5});
    g.add_edge("C", "E", {5, 5});
    
    std::vector<bool> params = {true, true};
    Path path = dijkstra_alg(g, "A", "E", params,  {1, 1, 1});
    
    std::cout << "\nComplex graph: ";
    print_path(path);
    
    assert(!path.verts.empty());
    
    if (path.verts.size() == 4) {
        assert(path.verts[0] == "A");
        assert(path.verts[1] == "C");
        assert(path.verts[2] == "D");
        assert(path.verts[3] == "E");
        assert(path.cost[0] == 7);
        assert(path.cost[1] == 4); 
    }
    
    std::cout << "pass\n";
}

void test_generate_graph_1(){
    std::cout << "Test generate graph 1: ";
    Graph g;

    g.generate_graph(5, 35, 2);
    assert(g.vertex_count() == 5);
    g.print();
    g.add_vertex("A");
    assert(g.vertex_count() == 6);
    g.print();
    std::cout << "pass\n";
}

void test_generate_graph_2(){
    std::cout << "Test generate graph 2: ";
    Graph g;
    std::vector<std::string> vert_names= {"A", "B", "C", "D", "E", "F", "G"};
    g.generate_graph(vert_names, 50, 2);
    assert(g.vertex_count() == 7);
    g.print();
 
    std::cout << "pass\n";
}

void test_scc(){
    std::cout << "Test  scc: ";
    Graph g({"A", "B", "C", "D"});
    g.add_edge("A", "B");
    g.add_edge("B", "C");
    g.add_edge("C", "A");
    g.add_edge("B", "D");

    auto sccs = find_scc(g);

    for (size_t i = 0; i < sccs.size(); ++i) {
        std::cout << "{ ";
        for (const auto& v : sccs[i]) {
            std::cout << v << " ";
        }
        std::cout << "} ";
    }
    std::cout << "\n";
    
    assert(sccs.size() == 2);
    for(auto& scc: sccs){
        if(scc.size() == 1){
            assert(scc[0] == "D");
        }
    }
    std::cout << "pass \n";
}

void test_scc_complex() {
    std::cout << "Test SCC complex: ";

    Graph g({"A", "B", "C", "D", "E", "F", "G", "H", "I", "J"});

    g.add_edge("A", "B");
    g.add_edge("B", "A");
    g.add_edge("B", "C");
    g.add_edge("C", "B");
    g.add_edge("D", "E");
    g.add_edge("E", "D");
    g.add_edge("G", "H");
    g.add_edge("I", "A");
    g.add_edge("C", "J");

    auto sccs = find_scc(g);

    for (size_t i = 0; i < sccs.size(); ++i) {
        std::cout << "{ ";
        for (const auto& v : sccs[i]) {
            std::cout << v << " ";
        }
        std::cout << "} ";
    }
    std::cout << "\n";

    assert(sccs.size() == 7);

    std::cout << "pass " << std::endl;
}

void run_dijkstra_tests() {
    std::cout << "\nDijkstra Algorithm Tests\n\n";
    
    test_dijkstra_one_param();
    test_dijkstra_both_params();
    test_dijkstra_with_weights();
    test_dijkstra_no_path();
    test_dijkstra_after_changes();
    test_dijkstra_complex_graph();
    
    std::cout << "\nAll Dijkstra Tests Passed\n\n";
}

void run_all_tests() {
    std::cout << "Graph Tests\n\n";
    
    test_add_vertex();
    test_add_edge();
    test_remove_edge();
    test_remove_vertex();
    test_get_neighbors();
    test_vertex_edge_count();
    test_constructor_with_vertices();
    test_error_cases();
    test_remove_all_vertices();
    test_generate_graph_1();
    test_generate_graph_2();
    test_scc();
    test_scc_complex();
    
    std::cout << "\nAll tests passed\n";
}

int main() {

    run_all_tests();
    run_dijkstra_tests();
    return 0;
}