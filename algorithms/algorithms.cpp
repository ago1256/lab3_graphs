#include "algorithms.h"
#include <stack>
#include <stdio.h>
#include <iostream>
#include <algorithm>

void relax_edges(const Graph& graph, const Path& curr_path, const std::vector<bool>& consider_params,
                const std::vector<double>& weights, std::unordered_map<std::string, double>& best_sums,   
                std::priority_queue<std::pair<double, Path>, std::vector<std::pair<double, Path>>, 
                                    std::greater<std::pair<double, Path>>>& pq) {
    
    std::string curr_vertex_name = curr_path.verts.back();
    std::vector<std::string> neighbors = graph.get_to_vertices(curr_vertex_name);
    
    for (const auto& neighbor_name : neighbors) {
        Path new_path = add_edge_to_path(graph, curr_path, curr_vertex_name, neighbor_name, consider_params, weights);
        
        if (best_sums.find(neighbor_name) == best_sums.end() || new_path.weighted_sum < best_sums[neighbor_name]) {
            best_sums[neighbor_name] = new_path.weighted_sum;
            pq.push({new_path.weighted_sum, new_path});
        }
    }
}

Path dijkstra_alg(const Graph& graph, const std::string& start, const std::string& finish,
     const std::vector<bool>& consider_params, const std::vector<double>& weights) {
    
    if (!graph.has_vertex(start) || !graph.has_vertex(finish)) {
        return Path();
    }
    
    std::priority_queue<std::pair<double, Path>, std::vector<std::pair<double, Path>>, 
                        std::greater<std::pair<double, Path>>> pq; 
    std::unordered_map<std::string, double> best_sums;
    
    Path start_path;
    start_path.verts = {start};
    start_path.cost = std::vector<int>(consider_params.size(), 0);
    start_path.weighted_sum = 0.0;
    
    pq.push({0.0, start_path});
    best_sums[start] = 0.0;
    
    while (!pq.empty()) {
        auto [curr_sum, curr_path] = pq.top();
        pq.pop();
        
        std::string curr_vertex = curr_path.verts.back();
        
        if (curr_vertex == finish) {
            return curr_path;
        }
        
        if (curr_sum > best_sums[curr_vertex]) {
            continue;
        }
        
        relax_edges(graph, curr_path, consider_params, weights, best_sums, pq);
    }
    return Path();
}

std::vector<Path> dijkstra_all_optimal_paths(const Graph& graph, const std::string& start, 
                                            const std::string& finish, const std::vector<bool>& consider_params,
                                            const std::vector<double>& weights) {
    
    if (!graph.has_vertex(start) || !graph.has_vertex(finish)) {
        return {};
    }
    
    std::priority_queue<std::pair<double, Path>,  std::vector<std::pair<double, Path>>, 
                        std::greater<std::pair<double, Path>>> pq;

    std::unordered_map<std::string, double> best_sums;
    std::unordered_map<std::string, std::vector<Path>> best_paths;
    std::vector<Path> result_paths;
    
    Path start_path;
    start_path.verts = {start};
    start_path.cost = std::vector<int>(consider_params.size(), 0);
    start_path.weighted_sum = 0.0;
    
    pq.push({0.0, start_path});
    best_sums[start] = 0.0;
    best_paths[start] = {start_path};
    
    while (!pq.empty()) {
        auto [curr_sum, curr_path] = pq.top();
        pq.pop();
        
        std::string curr_vertex = curr_path.verts.back();
        
        if (curr_sum > best_sums[curr_vertex]) {
            continue;
        }

        if (curr_vertex == finish) {
            if (result_paths.empty() || abs(curr_sum - result_paths[0].weighted_sum) < 1e-10) {
                result_paths.push_back(curr_path);
            }
            continue;
        }
        
        std::vector<std::string> neighbors = graph.get_to_vertices(curr_vertex);
        
        for (const auto& neighbor_name : neighbors) {
            Path new_path = add_edge_to_path(graph, curr_path, curr_vertex, neighbor_name, consider_params, weights);
            
            if (best_sums.find(neighbor_name) == best_sums.end() || new_path.weighted_sum < best_sums[neighbor_name]) {
                best_sums[neighbor_name] = new_path.weighted_sum;
                best_paths[neighbor_name] = {new_path};
                pq.push({new_path.weighted_sum, new_path});
                
            } 
            else if (abs(new_path.weighted_sum - best_sums[neighbor_name]) < 1e-10) {
                best_paths[neighbor_name].push_back(new_path);
                pq.push({new_path.weighted_sum, new_path});
            }
        }
    }
    
    return result_paths;
}

std::vector<std::vector<std::string>> find_scc(const Graph& graph) {
    
    const auto& vertices = graph.get_all_vertices();
    const auto& edges = graph.get_all_edges();
    
    std::vector<std::vector<std::string>> components;
    std::unordered_map<std::string, bool> visited;
    std::vector<std::string> order;
    
    std::unordered_map<std::string, std::string> id_to_name;
    for (const auto& [id, vertex] : vertices) {
        id_to_name[id] = vertex.name;
    }
    
    std::unordered_map<std::string, std::string> name_to_id;
    for (const auto& [id, vertex] : vertices) {
        name_to_id[vertex.name] = id;
    }
    
    for (const auto& [id, vertex] : vertices) {
        if (visited[id]) continue;
        
        std::stack<std::string> stack;
        stack.push(id);
        visited[id] = true;
        
        while (!stack.empty()) {
            std::string v_id = stack.top();
            const Vertex& v = vertices.at(v_id);
            bool pushed = false;
            
            for (const auto& edge_id : v.out_edges) {
                auto edge_it = edges.find(edge_id);
                if (edge_it == edges.end()) continue;
                
                std::string u_id = edge_it->second.finish;
                if (!visited[u_id]) {
                    visited[u_id] = true;
                    stack.push(u_id);
                    pushed = true;
                    break;
                }
            }
            
            if (!pushed) {
                stack.pop();
                order.push_back(v_id);
            }
        }
    }
    
    std::unordered_map<std::string, bool> visited_rev;
    std::reverse(order.begin(), order.end());
    
    for (const std::string& v_id : order) {
        if (visited_rev[v_id]) continue;
        
        std::vector<std::string> comp;
        std::stack<std::string> stack;
        stack.push(v_id);
        visited_rev[v_id] = true;
        
        while (!stack.empty()) {
            std::string node_id = stack.top();
            stack.pop();
            comp.push_back(vertices.at(node_id).name);
            
            const Vertex& node = vertices.at(node_id);
            
            for (const auto& edge_id : node.in_edges) {
                auto edge_it = edges.find(edge_id);
                if (edge_it == edges.end()) continue;
                
                std::string u_id = edge_it->second.start;
                if (!visited_rev[u_id]) {
                    visited_rev[u_id] = true;
                    stack.push(u_id);
                }
            }
        }
        
        components.push_back(std::move(comp));
    }
    
    return components;
}


Path add_edge_to_path(const Graph& graph, const Path& path, const std::string& from_name, const std::string& to_name,
                                const std::vector<bool>& consider_params, const std::vector<double>& weights) {
    Path new_path = path;
    
    Edge edge = graph.get_edge(from_name, to_name);

    new_path.verts.push_back(to_name);
    
    for (size_t i = 0; i < edge.params.size(); i++) {
        new_path.cost[i] += edge.params[i];
    }
    
    double weighted_cost = 0.0;
    for (size_t i = 0; i < edge.params.size() && i < weights.size(); i++) {
        if (consider_params[i]) {
            weighted_cost += edge.params[i] * weights[i];
        }
    }
    new_path.weighted_sum += weighted_cost;
    
    return new_path;
}


void print_path(const Path& path){
    for(auto& vert : path.verts){
        std::cout << vert << " ";
    }
    
    std::cout << "[ ";
    for(auto& param : path.cost){
        std::cout << param << " ";
    }
    
    std::cout << "]\n";
}

