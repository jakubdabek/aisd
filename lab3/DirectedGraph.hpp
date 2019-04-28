#pragma once

#include <map>
#include <vector>
#include <deque>

class DirectedGraph
{
public:
    using weight_t = int;
    using vertex_t = int;
    using distance_t = weight_t;

    explicit DirectedGraph(vertex_t size) : size(size) {}
    void insert_edge(vertex_t source, vertex_t sink, weight_t weight = 0);

    auto dijkstra(vertex_t source, bool verbose = false) -> std::vector<std::pair<weight_t, std::deque<vertex_t>>>;
private:
    vertex_t size;
    std::multimap<vertex_t, std::pair<vertex_t, weight_t>> edges;
};
