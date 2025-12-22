#pragma once

#include <vector>
#include <unordered_map>
#include <string>
#include <iostream>
#include <queue>
#include <functional>
#include"../structs_for_graph/structs_for_graph.h"
#include"../errors/errors.h"


class Graph {
private:

    std::unordered_map<std::string, Vertex> vertices;  
    std::unordered_map<std::string, Edge> edges;     
    
    void relax_edges(const Path& curr_path, const std::vector<bool>& consider_params,
                    std::unordered_map<std::string, double>& best_sums, std::priority_queue<std::pair<double, Path>, 
                                       std::vector<std::pair<double, Path>>,  std::greater<std::pair<double, Path>>>& pq) const;

    void generate_edges_for_graph(int percent, int params_count);

public:

    Graph();
    Graph(const std::vector<std::string>& vert_names);
    ~Graph();
    
    void add_vertex(const std::string& vertex_name);
    void add_edge(const std::string& from_name, const std::string& to_name, const  std::vector<int>& data = {}, 
        const std::vector<double>& weights = {});
    void remove_vertex(const std::string& vertex_name);
    void remove_edge(const std::string& from_name, const std::string& to_name);
    
    bool has_vertex(const std::string& vertex_name) const;
    bool has_edge(const std::string& from_name, const std::string& to_name) const;
    
    Vertex& get_vertex(const std::string& vertex_name) const;
    Edge& get_edge(const std::string& from_name, const std::string& to_name) const;

    void change_vertex(const std::string& old_name, const std::string& new_name);
    
    std::vector<std::string> get_to_vertices(const std::string& vertex_name) const;  
    std::vector<std::string> get_from_vertices(const std::string& vertex_name) const; 
    std::vector<std::string> get_neighbors(const std::string& vertex_name) const;
    
    size_t vertex_count() const;
    size_t edge_count() const;
    bool empty() const;
    const std::unordered_map<std::string, Vertex>& get_all_vertices() const;
    const std::unordered_map<std::string, Edge>& get_all_edges() const;
    
    void generate_graph(int vert_count, int percent, int params_count);
    void generate_graph(std::vector<std::string> verts_name, int percent, int params_count);
    void clear();
    void print() const;

    Path dijkstra_alg(const std::string& start, const std::string& finish, const std::vector<bool>& consider_params);
};

