#pragma once

#include <vector>
#include <unordered_map>
#include <string>
#include <iostream>
#include <queue>
#include <functional>
#include "../structs_for_graph/structs_for_graph.h"
#include "../errors/errors.h"

class Graph {
private:
    std::unordered_map<std::string, Vertex> vertices; 
    std::unordered_map<std::string, Edge> edges;
    
    std::unordered_map<std::string, std::string> name_to_id;
    int vertex_counter = 0;

    std::string generate_vertex_id();
    std::string get_vertex_id_by_name(const std::string& name) const;

public:
    Graph();
    Graph(const std::vector<std::string>& vert_names);
    ~Graph();
    
    void add_edge(const std::string& from_name, const std::string& to_name, const std::vector<int>& data = {});
    void remove_vertex(const std::string& vertex_name);
    void remove_edge(const std::string& from_name, const std::string& to_name);
    
    bool has_vertex(const std::string& vertex_name) const;
    bool has_edge(const std::string& from_name, const std::string& to_name) const;
    
    Vertex& get_vertex(const std::string& vertex_name) const;
    Edge& get_edge(const std::string& from_name, const std::string& to_name) const;
    
    void add_vertex(const std::string& vertex_name, int x = -1, int y = -1);
    void rename_vertex(const std::string& old_name, const std::string& new_name);
    void set_vertex_position(const std::string& vertex_name, int x, int y);
    std::pair<int, int> get_vertex_position(const std::string& vertex_name) const;
    
    std::vector<std::string> get_to_vertices(const std::string& vertex_name) const;  
    std::vector<std::string> get_from_vertices(const std::string& vertex_name) const; 
    std::vector<std::string> get_neighbors(const std::string& vertex_name) const;
    
    size_t vertex_count() const;
    size_t edge_count() const;
    bool empty() const;
    const std::unordered_map<std::string, Vertex>& get_all_vertices() const;
    const std::unordered_map<std::string, Edge>& get_all_edges() const;
    
    std::string get_vertex_name_by_id(const std::string& vertex_id) const;
    std::vector<std::string> get_all_vertex_names() const;
    
    void generate_graph(int vert_count, int percent, int params_count);
    void generate_graph(const std::vector<std::string>& verts_names, int percent, int params_count);
    void generate_edges_for_graph(int percent, int params_count);
    
    void clear();
    void print() const;
};