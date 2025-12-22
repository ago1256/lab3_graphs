#include "errors.h"
#include <iostream>

void errors_detection(Error error) {
    switch (error) {
        case Error::VERTEX_NOT_FOUND:
            std::cerr << "Error: VERTEX_NOT_FOUND" << std::endl;
            break;
        case Error::EDGE_NOT_FOUND:
            std::cerr << "Error: EDGE_NOT_FOUND" << std::endl;
            break;
        case Error::GRAPH_EMPTY:
            std::cerr << "Error: GRAPH_EMPTY" << std::endl;
            break;
        case Error::VERTEX_ALREADY_EXISTS:
            std::cerr << "Error: VERTEX_ALREADY_EXISTS" << std::endl;
            break;
        case Error::EDGE_ALREADY_EXISTS:
            std::cerr << "Error: EDGE_ALREADY_EXISTS" << std::endl;
            break;
    }
}

