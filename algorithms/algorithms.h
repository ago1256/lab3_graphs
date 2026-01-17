#pragma once

#include "../structs_for_graph/structs_for_graph.h"
#include "../graph/graph.h"
#include <queue>
#include <unordered_map>
#include <vector>
#include <functional>

struct Path {
    std::vector<std::string> verts; 
    std::vector<int> cost;
    double weighted_sum = 0.0;   

    Path() = default;
    Path(const std::string& start_vertex_id, size_t param_count) 
        : verts({start_vertex_id}), 
          cost(std::vector<int>(param_count, 0)),
          weighted_sum(0.0) {}

    bool operator<(const Path& other) const {
        return weighted_sum < other.weighted_sum;
    }
};

Path add_edge_to_path(const Graph& graph, const Path& path, const std::string& from_name, const std::string& to_name,
                                const std::vector<bool>& consider_params, const std::vector<double>& weights);
void print_path(const Path& path);

void relax_edges(const Graph& graph,  const Path& curr_path,  const std::vector<bool>& consider_params,
                    const std::vector<double>& weights, std::unordered_map<std::string, double>& best_sums,  
                    std::priority_queue<std::pair<double, Path>,  std::vector<std::pair<double, Path>>,  
                        std::greater<std::pair<double, Path>>>& pq);
    
Path dijkstra_alg(const Graph& graph, const std::string& start, const std::string& finish,
           const std::vector<bool>& consider_params, const std::vector<double>& weights);
    
std::vector<Path> dijkstra_all_optimal_paths(const Graph& graph, const std::string& start, 
                                            const std::string& finish, const std::vector<bool>& consider_params,
                                            const std::vector<double>& weights);

std::vector<std::vector<std::string>> find_scc(const Graph& graph);