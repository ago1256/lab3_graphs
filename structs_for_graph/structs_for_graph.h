#pragma once

#include <vector>
#include <string>


struct Vertex {
    std::vector<std::string> in_edges;    
    std::vector<std::string> out_edges;
    std::string name;    
    
    Vertex() = default;  
    Vertex(const std::string& vert_name) : name(vert_name) {}
};

struct Edge {
    std::string name;    
    std::string start;    
    std::string finish;   
    std::vector<int> params;   
    std::vector<double> weights; 
    
    Edge() = default;  
    Edge(const std::string& start_name, const std::string& finish_name, 
         const std::vector<int>& d = {}, const std::vector<double>& w = {}) 
        : start(start_name), 
          finish(finish_name), 
          name(start_name + "_" + finish_name),
          params(d),
          weights(w) 
    {
        if (weights.empty()) {
            weights.resize(params.size(), 1.0);
        }
    }
};

struct Path {
    std::vector<std::string> verts;
    std::vector<int> cost;
    double weighted_sum = 0.0;   

    Path() = default;
    Path(const std::string& start_vertex, size_t param_count) 
        : verts({start_vertex}), 
          cost(std::vector<int>(param_count, 0)),
          weighted_sum(0.0) {}

    bool operator<(const Path& other) const {
        return weighted_sum < other.weighted_sum;
    }
};

Path operator+(const Path& path, const Edge& edge);
Path add_edge_to_path(const Path& path, const Edge& edge, const std::vector<bool>& consider_params);
double get_weighted_cost(const Edge& edge, const std::vector<bool>& consider_params); 
void print_path(const Path& path);