#include"structs_for_graph.h"
#include<iostream>

Path operator+(const Path& path, const Edge& edge) {
    Path new_path = path;
    new_path.verts.push_back(edge.finish);
    
    for (size_t i = 0; i < edge.params.size(); i++) {
        new_path.cost[i] += edge.params[i];
    }
    
    for (size_t i = 0; i < edge.params.size() && i < edge.weights.size(); i++) {
        new_path.weighted_sum += edge.params[i] * edge.weights[i];
    }
    
    return new_path;
}

Path add_edge_to_path(const Path& path, const Edge& edge, const std::vector<bool>& consider_params) {
    Path new_path = path;
    new_path.verts.push_back(edge.finish);
    
    for (size_t i = 0; i < edge.params.size(); i++) {
        new_path.cost[i] += edge.params[i];
    }
    
    new_path.weighted_sum += get_weighted_cost(edge, consider_params);
    
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

double get_weighted_cost(const Edge& edge, const std::vector<bool>& consider_params){

    double cost = 0.0;
    for (size_t i = 0; i < edge.params.size(); i++) {
        if (consider_params[i]) {
            cost += edge.params[i] * edge.weights[i];
        }
    }
    return cost;
}