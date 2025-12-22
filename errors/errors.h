#pragma once

enum class Error {
    VERTEX_NOT_FOUND,
    VERTEX_ALREADY_EXISTS,
    EDGE_NOT_FOUND,
    GRAPH_EMPTY,
    EDGE_ALREADY_EXISTS
};

void errors_detection(Error error);