#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <string>
#include <iostream>
#include <random>
#include <algorithm>  
#include <tuple>    
#include <stack>     
#include <ctime>
#include "graph.h"

Graph::Graph() : vertex_counter(0) {}

Graph::Graph(const std::vector<std::string>& vert_names) : vertex_counter(0) {
    
    for (const auto& name : vert_names) {
        add_vertex(name);
    }
}

Graph::~Graph() = default;

std::string Graph::generate_vertex_id() {
    std::string id;
    do {
        id = std::to_string(vertex_counter++);
    } while (vertices.find(id) != vertices.end());
    return id;
}

std::string Graph::get_vertex_id_by_name(const std::string& name) const {
    auto it = name_to_id.find(name);
    if (it != name_to_id.end()) {
        return it->second;
    }
    throw Error::VERTEX_NOT_FOUND;
}

void Graph::add_vertex(const std::string& vertex_name, int x, int y) {
    if (has_vertex(vertex_name)) {
        errors_detection(Error::VERTEX_ALREADY_EXISTS);
        throw Error::VERTEX_ALREADY_EXISTS;
    }
    
    std::string vertex_id = generate_vertex_id();
    
    Vertex new_vertex(vertex_id, vertex_name);
    if (x != -1 && y != -1) {
        new_vertex.x = x;
        new_vertex.y = y;
    }
    
    vertices[vertex_id] = new_vertex;
    name_to_id[vertex_name] = vertex_id;
}

void Graph::add_edge(const std::string& from_name, const std::string& to_name, const std::vector<int>& data) {
    if (to_name == from_name) {
        throw Error::EDGE_NOT_FOUND;
    }
    if (!has_vertex(from_name) || !has_vertex(to_name)) {
        errors_detection(Error::VERTEX_NOT_FOUND);
        throw Error::VERTEX_NOT_FOUND;
    }
    
    std::string from_id = get_vertex_id_by_name(from_name);
    std::string to_id = get_vertex_id_by_name(to_name);
    std::string edge_id = from_id + "_" + to_id;
    
    if (has_edge(from_name, to_name)) {
        edges[edge_id].params = data;
        return;
    }
    
    Edge new_edge(from_id, to_id, data);
    edges[edge_id] = new_edge;
    
    vertices[from_id].out_edges.push_back(edge_id);
    vertices[to_id].in_edges.push_back(edge_id);
}


void Graph::remove_vertex(const std::string& vertex_name) {
    if (!has_vertex(vertex_name)) {
        throw Error::VERTEX_NOT_FOUND;
    }
    
    std::string vertex_id = get_vertex_id_by_name(vertex_name);
    const Vertex& vertex = vertices.at(vertex_id);
    
    std::unordered_set<std::string> edges_to_remove;
    edges_to_remove.insert(vertex.out_edges.begin(), vertex.out_edges.end());
    edges_to_remove.insert(vertex.in_edges.begin(), vertex.in_edges.end());
    
    for (const auto& edge_id : edges_to_remove) {
        const Edge& e = edges.at(edge_id);
        if (e.start != vertex_id && vertices.find(e.start) != vertices.end()) {
            auto& out_list = vertices.at(e.start).out_edges;
            out_list.erase(std::remove(out_list.begin(), out_list.end(), edge_id), out_list.end());
        }
        if (e.finish != vertex_id && vertices.find(e.finish) != vertices.end()) {
            auto& in_list = vertices.at(e.finish).in_edges;
            in_list.erase(std::remove(in_list.begin(), in_list.end(), edge_id), in_list.end());
        }
        edges.erase(edge_id);
    }

    vertices.erase(vertex_id);
    name_to_id.erase(vertex_name);
}

void Graph::remove_edge(const std::string& from_name, const std::string& to_name) {
    if (!has_vertex(from_name) || !has_vertex(to_name)) {
        errors_detection(Error::VERTEX_NOT_FOUND);
        throw Error::VERTEX_NOT_FOUND;
    }
    
    if (!has_edge(from_name, to_name)) {
        errors_detection(Error::EDGE_NOT_FOUND);
        throw Error::EDGE_NOT_FOUND;
    }
    
    std::string from_id = get_vertex_id_by_name(from_name);
    std::string to_id = get_vertex_id_by_name(to_name);
    std::string edge_id = from_id + "_" + to_id;

    auto& out_vector = vertices[from_id].out_edges;
    out_vector.erase(std::remove(out_vector.begin(), out_vector.end(), edge_id), out_vector.end());
    
    auto& in_vector = vertices[to_id].in_edges;
    in_vector.erase(std::remove(in_vector.begin(), in_vector.end(), edge_id), in_vector.end());
    edges.erase(edge_id);
    
}

bool Graph::has_vertex(const std::string& vertex_name) const {
    return name_to_id.find(vertex_name) != name_to_id.end();
}

bool Graph::has_edge(const std::string& from_name, const std::string& to_name) const {
    if (!has_vertex(from_name) || !has_vertex(to_name)) {
        return false;
    }

    std::string from_id = get_vertex_id_by_name(from_name);
    std::string to_id = get_vertex_id_by_name(to_name);
    std::string edge_id = from_id + "_" + to_id;
    return edges.find(edge_id) != edges.end();

}

Vertex& Graph::get_vertex(const std::string& vertex_name) const {
    if (!has_vertex(vertex_name)) {
        errors_detection(Error::VERTEX_NOT_FOUND);
        throw Error::VERTEX_NOT_FOUND;
    }
    std::string vertex_id = get_vertex_id_by_name(vertex_name);
    return const_cast<Vertex&>(vertices.at(vertex_id));
}

Edge& Graph::get_edge(const std::string& from_name, const std::string& to_name) const {
    if (!has_edge(from_name, to_name)) {
        errors_detection(Error::EDGE_NOT_FOUND);
        throw Error::EDGE_NOT_FOUND;
    }
    
    std::string from_id = get_vertex_id_by_name(from_name);
    std::string to_id = get_vertex_id_by_name(to_name);
    std::string edge_id = from_id + "_" + to_id;
    
    return const_cast<Edge&>(edges.at(edge_id));
}

void Graph::rename_vertex(const std::string& old_name, const std::string& new_name) {
    if (!has_vertex(old_name)) {
        throw Error::VERTEX_NOT_FOUND;
    }
    
    if (has_vertex(new_name)) {
        throw Error::VERTEX_ALREADY_EXISTS;
    }
    
    std::string vertex_id = get_vertex_id_by_name(old_name);
    vertices[vertex_id].name = new_name;
    
    name_to_id.erase(old_name);
    name_to_id[new_name] = vertex_id;
}

void Graph::set_vertex_position(const std::string& vertex_name, int x, int y) {
    if (!has_vertex(vertex_name)) {
        throw Error::VERTEX_NOT_FOUND;
    }
    
    std::string vertex_id = get_vertex_id_by_name(vertex_name);
    vertices[vertex_id].x = x;
    vertices[vertex_id].y = y;
}

std::pair<int, int> Graph::get_vertex_position(const std::string& vertex_name) const {
    if (!has_vertex(vertex_name)) {
        throw Error::VERTEX_NOT_FOUND;
    }
    
    std::string vertex_id = get_vertex_id_by_name(vertex_name);
    return {vertices.at(vertex_id).x, vertices.at(vertex_id).y};
}

std::vector<std::string> Graph::get_to_vertices(const std::string& vertex_name) const {
    std::vector<std::string> result;
    
    if (!has_vertex(vertex_name)) {
        return result;
    }
    
    std::string vertex_id = get_vertex_id_by_name(vertex_name);
    const Vertex& vertex = vertices.at(vertex_id);
    
    for (const auto& edge_id : vertex.out_edges) {
        auto edge = edges.find(edge_id);
        if (edge != edges.end()) {
            std::string to_id = edge->second.finish;
            if (vertices.find(to_id) != vertices.end()) {
                result.push_back(vertices.at(to_id).name);
            }
        }
    }
    
    return result;
}

std::vector<std::string> Graph::get_from_vertices(const std::string& vertex_name) const {
    std::vector<std::string> result;
    
    if (!has_vertex(vertex_name)) {
        return result;
    }
    
    std::string vertex_id = get_vertex_id_by_name(vertex_name);
    const Vertex& vertex = vertices.at(vertex_id);
    
    for (const auto& edge_id : vertex.in_edges) {
        auto edge = edges.find(edge_id);
        if (edge != edges.end()) {
            std::string from_id = edge->second.start;
            if (vertices.find(from_id) != vertices.end()) {
                result.push_back(vertices.at(from_id).name);
            }
        }
    }
    
    return result;
}

std::vector<std::string> Graph::get_neighbors(const std::string& vertex_name) const {

    if (!has_vertex(vertex_name)) {
        return {};
    }
    
    std::string vertex_id = get_vertex_id_by_name(vertex_name);
    std::unordered_set<std::string> neighbours;
    
    for (const auto& edge_id : vertices.at(vertex_id).out_edges) {
        const auto& edge = edges.at(edge_id);
        neighbours.insert(vertices.at(edge.finish).name);
    }
    
    for (const auto& edge_id : vertices.at(vertex_id).in_edges) {
        const auto& edge = edges.at(edge_id);
        neighbours.insert(vertices.at(edge.start).name);
    }
    
    return {neighbours.begin(), neighbours.end()};
}

size_t Graph::vertex_count() const {
    return vertices.size();
}

size_t Graph::edge_count() const {
    return edges.size();
}

bool Graph::empty() const {
    return vertices.empty();
}

const std::unordered_map<std::string, Vertex>& Graph::get_all_vertices() const {
    return vertices;
}

const std::unordered_map<std::string, Edge>& Graph::get_all_edges() const {
    return edges;
}

std::string Graph::get_vertex_name_by_id(const std::string& vertex_id) const {
    auto it = vertices.find(vertex_id);
    if (it != vertices.end()) {
        return it->second.name;
    }
    return "";
}

std::vector<std::string> Graph::get_all_vertex_names() const {
    std::vector<std::string> result;
    result.reserve(name_to_id.size());
    
    for (const auto& [name, id] : name_to_id) {
        result.push_back(name);
    }
    
    return result;
}

void Graph::clear() {
    vertices.clear();
    edges.clear();
    name_to_id.clear();
    vertex_counter = 0;
}

void Graph::print() const {
    std::cout << "vertex count: " << vertex_count() << "\nedges count: " << edge_count() << "\n";
    
    for (const auto& [id, edge] : edges) {
        std::cout << get_vertex_name_by_id(edge.start) << " -> " 
                  << get_vertex_name_by_id(edge.finish) << " [ ";
        for(int i = 0; i < edge.params.size(); i++) {
            std::cout << edge.params[i] << " ";
        }
        std::cout << "]\n";
    }
}

void Graph::generate_graph(int vert_count, int percent, int params_count) {
    for(int i = 0; i < vert_count; i++) {
        this->add_vertex(std::to_string(i));
    }
    this->generate_edges_for_graph(percent, params_count);
}

void Graph::generate_graph(const std::vector<std::string>& verts_names, int percent, int params_count) {
    for(const auto& name : verts_names) {
        this->add_vertex(name);
    }
    this->generate_edges_for_graph(percent, params_count);
}

void Graph::generate_edges_for_graph(int percent, int params_count) {
    std::vector<std::string> vertex_names = get_all_vertex_names();
    
    for (const auto& vert_name1 : vertex_names) {
        for (const auto& vert_name2 : vertex_names) {
            if (vert_name1 != vert_name2) {
                if ((std::rand() % 100 + 1) <= percent) {
                    std::vector<int> params;
                    
                    for (int i = 0; i < params_count; i++) {
                        params.push_back(std::rand() % 100);
                    }
                    
                    try {
                        this->add_edge(vert_name1, vert_name2, params);
                    } catch (const Error& e) {

                    }
                }
            }
        }
    }
}