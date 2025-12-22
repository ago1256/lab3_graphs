#include <vector>
#include <unordered_map>
#include <string>
#include <iostream>
#include <random>
#include <algorithm>  // для std::sort, std::unique
#include <tuple>      // для std::tuple
#include "graph.h"

Graph::Graph() = default;

Graph::Graph(const std::vector<std::string>& vert_names) {
    for (const auto& name : vert_names) {
        add_vertex(name);
    }
}

Graph::~Graph() = default;

void Graph::add_vertex(const std::string& vertex_name) {
    if (has_vertex(vertex_name)) {
        errors_detection(Error::VERTEX_ALREADY_EXISTS);
        throw Error::VERTEX_ALREADY_EXISTS;
    }
    vertices[vertex_name] = Vertex(vertex_name);
}

void Graph::add_edge(const std::string& from_name, const std::string& to_name,  const std::vector<int>& data,
                     const std::vector<double>& weights) {
    if(to_name == from_name){
        throw Error::EDGE_NOT_FOUND;
    }
    if (!has_vertex(from_name) || !has_vertex(to_name)) {
        errors_detection(Error::VERTEX_NOT_FOUND);
        throw Error::VERTEX_NOT_FOUND;
    }
    
    std::string edge_name = from_name + "_" + to_name;
    
    if (has_edge(from_name, to_name)) {
        edges[edge_name].params = data;
        if (!weights.empty()) {
            edges[edge_name].weights = weights;
        }
        return; 
    }
    
    Edge new_edge(from_name, to_name, data, weights);
    edges[new_edge.name] = new_edge;
    
    vertices[from_name].out_edges.push_back(new_edge.name);
    vertices[to_name].in_edges.push_back(new_edge.name);
}

void Graph::remove_vertex(const std::string& vertex_name) {
    if (!has_vertex(vertex_name)) {
        throw Error::VERTEX_NOT_FOUND;
    }
    
    // Собираем имена рёбер, которые нужно удалить
    std::vector<std::string> edges_to_remove;
    
    // Все исходящие рёбра
    for (const auto& edge_name : vertices.at(vertex_name).out_edges) {
        edges_to_remove.push_back(edge_name);
    }
    // Все входящие рёбра
    for (const auto& edge_name : vertices.at(vertex_name).in_edges) {
        if (std::find(edges_to_remove.begin(), edges_to_remove.end(), edge_name) == edges_to_remove.end()) {
            edges_to_remove.push_back(edge_name);
        }
    }

    // Удаляем ссылки на эти рёбра из других вершин
    for (const auto& edge_name : edges_to_remove) {
        const Edge& e = edges.at(edge_name);
        if (e.start != vertex_name) {
            auto& out_list = vertices.at(e.start).out_edges;
            out_list.erase(std::remove(out_list.begin(), out_list.end(), edge_name), out_list.end());
        }
        if (e.finish != vertex_name) {
            auto& in_list = vertices.at(e.finish).in_edges;
            in_list.erase(std::remove(in_list.begin(), in_list.end(), edge_name), in_list.end());
        }
    }

    // Удаляем саму вершину и рёбра
    for (const auto& edge_name : edges_to_remove) {
        edges.erase(edge_name);
    }
    vertices.erase(vertex_name);
}

void Graph::remove_edge(const std::string& from_name, const std::string& to_name) {
    if (!has_vertex(from_name) || !has_vertex(to_name)) {
        errors_detection(Error::VERTEX_NOT_FOUND);
        throw Error::VERTEX_NOT_FOUND;
    }
    
    std::string edge_name = from_name + "_" + to_name;
    
    if (!has_edge(from_name, to_name)) {
        errors_detection(Error::EDGE_NOT_FOUND);
        throw Error::EDGE_NOT_FOUND;
    }
    
    // Удаляем ребро из edges
    edges.erase(edge_name);
    
    // Удаляем ссылку из out_edges исходной вершины
    auto& out_vector = vertices[from_name].out_edges;
    for (size_t i = 0; i < out_vector.size(); ++i) {
        if (out_vector[i] == edge_name) {
            out_vector.erase(out_vector.begin() + i);
            break;
        }
    }
    
    // Удаляем ссылку из in_edges целевой вершины
    auto& in_vector = vertices[to_name].in_edges;
    for (size_t i = 0; i < in_vector.size(); ++i) {
        if (in_vector[i] == edge_name) {
            in_vector.erase(in_vector.begin() + i);
            break;
        }
    }
}

bool Graph::has_vertex(const std::string& vertex_name) const {
    return vertices.find(vertex_name) != vertices.end();
}

bool Graph::has_edge(const std::string& from_name, const std::string& to_name) const {
    std::string edge_name = from_name + "_" + to_name;
    return edges.find(edge_name) != edges.end();
}

Vertex& Graph::get_vertex(const std::string& vertex_name) const {
    if (!has_vertex(vertex_name)) {
        errors_detection(Error::VERTEX_NOT_FOUND);
        throw Error::VERTEX_NOT_FOUND;
    }
    auto vert = vertices.find(vertex_name);
    return const_cast<Vertex&>(vert->second);
}

Edge& Graph::get_edge(const std::string& from_name, const std::string& to_name) const {
    if (!has_edge(from_name, to_name)) {
        errors_detection(Error::EDGE_NOT_FOUND);
        throw Error::EDGE_NOT_FOUND;
    }
    std::string edge_name = from_name + "_" + to_name;
    auto edge = edges.find(edge_name);
    return const_cast<Edge&>(edge->second);
}

void Graph::change_vertex(const std::string& old_name, const std::string& new_name) {
    if (old_name == new_name) return;
    
    if (!has_vertex(old_name)) {
        throw Error::VERTEX_NOT_FOUND;
    }
    
    if (has_vertex(new_name)) {
        throw Error::VERTEX_ALREADY_EXISTS;
    }
    
    // 1. Создаём новую вершину
    add_vertex(new_name);
    
    // 2. Копируем ВСЕ ребра, связанные со старой вершиной
    // НЕ используем итераторы по edges, так как они могут измениться!
    std::vector<std::pair<std::string, std::string>> edges_to_copy;
    std::vector<std::vector<int>> params_to_copy;
    std::vector<std::vector<double>> weights_to_copy;
    
    // Сначала собираем информацию обо ВСЕХ ребрах графа
    for (const auto& edge_pair : edges) {
        const Edge& edge = edge_pair.second;
        
        // Если ребро связано со старой вершиной
        if (edge.start == old_name || edge.finish == old_name) {
            // Определяем новые имена для начала и конца ребра
            std::string new_start = (edge.start == old_name) ? new_name : edge.start;
            std::string new_finish = (edge.finish == old_name) ? new_name : edge.finish;
            
            // Пропускаем петли
            if (new_start == new_finish) {
                continue;
            }
            
            edges_to_copy.push_back({new_start, new_finish});
            params_to_copy.push_back(edge.params);
            weights_to_copy.push_back(edge.weights);
        }
    }
    
    // 3. Удаляем старую вершину
    remove_vertex(old_name);
    
    // 4. Добавляем новые ребра с обновлёнными именами
    for (size_t i = 0; i < edges_to_copy.size(); ++i) {
        const auto& [from, to] = edges_to_copy[i];
        
        // Проверяем, что обе вершины существуют (новая вершина точно существует)
        if (has_vertex(from) && has_vertex(to)) {
            // Проверяем, не существует ли уже такое ребро
            if (!has_edge(from, to)) {
                add_edge(from, to, params_to_copy[i], weights_to_copy[i]);
            }
        }
    }
}

std::vector<std::string> Graph::get_to_vertices(const std::string& vertex_name) const {
    std::vector<std::string> result;
    
    if (!has_vertex(vertex_name)) {
        return result;
    }
    
    const Vertex& vertex = vertices.at(vertex_name);
    
    for (const auto& edge_name : vertex.out_edges) {
        auto edge = edges.find(edge_name);
        if (edge != edges.end()) {
            result.push_back(edge->second.finish);
        }
    }
    
    return result;
}


std::vector<std::string> Graph::get_from_vertices(const std::string& vertex_name) const {
    std::vector<std::string> result;
    
    if (!has_vertex(vertex_name)) {
        return result;
    }
    
    const Vertex& vertex = vertices.at(vertex_name);
    
    for (const auto& edge_name : vertex.in_edges) {
        auto edge = edges.find(edge_name);
        if (edge != edges.end()) {

            result.push_back(edge->second.start);
        }
    }
    
    return result;
}

std::vector<std::string> Graph::get_neighbors(const std::string& vertex_name) const {
    std::vector<std::string> result;
    
    if (!has_vertex(vertex_name)) {
        return result;
    }
    
    auto to_vertices = get_to_vertices(vertex_name);
    result = to_vertices;  
    
    auto from_vertices = get_from_vertices(vertex_name);
    for (const auto& v : from_vertices) {
        bool flag = false;
        for (const auto& exist_path : result) {
            if (exist_path == v) {
                flag = true;
                break;
            }
        }
        if (!flag) {
            result.push_back(v);
        }
    }
    
    return result;
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

void Graph::clear() {
    vertices.clear();
    edges.clear();
}

void Graph::print() const {
    std::cout << "vertex count: " << vertex_count() << "\nedges count: " << edge_count() << "\n";
    
    for (const auto& [name, edge] : edges) {
        std::cout << edge.start << " _ " << edge.finish << " [ ";
        for(int i = 0; i< edge.params.size(); i++){
            std::cout << edge.params[i] << " ";
        }
        std::cout << "]\n";
    }
}


void Graph::generate_graph(int vert_count, int percent, int params_count){
    for(int i = 0; i < vert_count; i++){
        this->add_vertex(std::to_string(i));
    }

    this->generate_edges_for_graph(percent, params_count);
}

void Graph::generate_graph(std::vector<std::string> verts_name, int percent, int params_count){
    for(auto vert_name: verts_name){
        this->add_vertex(vert_name);
    }
    this->generate_edges_for_graph(percent, params_count);

}

void Graph::generate_edges_for_graph(int percent, int params_count){

    for(auto vert_1: vertices){
    for(auto vert_2: vertices){
        if(vert_1.first != vert_2.first){
            if((rand()%100 + 1) <= percent){
                std::vector<int> params;
                std::vector<double> weights;

                for(int i = 0; i <params_count; i++){
                    params.push_back(rand() % 100);
                    double weight = (rand()% 100) / 100;
                    weights.push_back(weight);
                }
                this->add_edge(vert_1.first, vert_2.first, params);
            }
        }
    }
}
}

void Graph::relax_edges(const Path& curr_path, const std::vector<bool>& consider_params,
                       std::unordered_map<std::string, double>& best_sums, std::priority_queue<std::pair<double, Path>, 
                                          std::vector<std::pair<double, Path>>, std::greater<std::pair<double, Path>>>& pq) const {
    
    std::string curr_vertex = curr_path.verts.back();
    auto neighbors = get_to_vertices(curr_vertex);

    for (const auto& neighbor : neighbors) {
        Edge edge = get_edge(curr_vertex, neighbor);
        
        Path new_path = add_edge_to_path(curr_path, edge, consider_params);
        
        if (best_sums.find(neighbor) == best_sums.end() || 
            new_path.weighted_sum < best_sums[neighbor]) {
            best_sums[neighbor] = new_path.weighted_sum;
            pq.push({new_path.weighted_sum, new_path});
        }
    }
}

Path Graph::dijkstra_alg(const std::string& start, const std::string& finish,
                        const std::vector<bool>& consider_params) {
    
    if (!has_vertex(start) || !has_vertex(finish)) {
        errors_detection(Error::VERTEX_NOT_FOUND);
        throw Error::VERTEX_NOT_FOUND;
    }
    
    std::priority_queue<std::pair<double, Path>, std::vector<std::pair<double, Path>>, std::greater<std::pair<double, Path>>> pq;
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
        
        relax_edges(curr_path, consider_params, best_sums, pq);
    }
    return Path();
}