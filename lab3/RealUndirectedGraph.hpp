#pragma once

#include <map>
#include <vector>
#include <deque>

class RealUndirectedGraph
{
public:
    using weight_t = double;
    using vertex_t = int;
    using distance_t = weight_t;

    explicit RealUndirectedGraph(vertex_t size) : size(size) {}
    void insert_edge(vertex_t source, vertex_t sink, weight_t weight = 0);

    // O(E log V)
    auto kruskal() const -> std::pair<weight_t, std::vector<std::pair<vertex_t, vertex_t>>>;
    // O(E log E) or O(E log V)
    auto prim(bool verbose = false) const -> std::pair<weight_t, std::vector<std::pair<vertex_t, vertex_t>>>;

private:
    vertex_t size;
    std::multimap<vertex_t, std::pair<vertex_t, weight_t>> edges;
};
