#include"structs_for_graph.h"
#include<iostream>


double get_weighted_cost(const Edge& edge, const std::vector<bool>& consider_params){
    double cost = 0.0;
    for (size_t i = 0; i < edge.params.size(); i++) {
        if (consider_params[i]) {
            cost += edge.params[i] * edge.weights[i];
        }
    }
    return cost;
}