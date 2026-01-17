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
    std::string start;
    std::string finish;
    std::vector<int> params;
    
    Edge() = default; 
    
    Edge(const std::string& s, const std::string& f, 
         const std::vector<int>& p = {}) 
        : start(s), finish(f), params(p) {}
};

