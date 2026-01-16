#pragma once

#include <vector>
#include <string>

struct Vertex {
    std::string id;   
    std::vector<std::string> in_edges;    
    std::vector<std::string> out_edges;
    std::string name;  
    int x = 0;    
    int y = 0;        
    
    Vertex() = default;  
    Vertex(const std::string& vert_id, const std::string& vert_name) 
        : id(vert_id), name(vert_name) {}
    Vertex(const std::string& vert_id, const std::string& vert_name, int pos_x, int pos_y) 
        : id(vert_id), name(vert_name), x(pos_x), y(pos_y) {}
};

struct Edge {
    std::string id;     
    std::string start;   
    std::string finish; 
    std::vector<int> params;   
    std::vector<double> weights; 
    
    Edge() = default;  
    Edge(const std::string& start_id, const std::string& finish_id, 
         const std::vector<int>& d = {}, const std::vector<double>& w = {}) 
        : start(start_id), 
          finish(finish_id), 
          id(start_id + "_" + finish_id),
          params(d),
          weights(w) 
    {
        if (weights.empty()) {
            weights.resize(params.size(), 1.0);
        }
    }
};


double get_weighted_cost(const Edge& edge, const std::vector<bool>& consider_params); 
