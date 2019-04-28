#pragma once

#include <map>
#include <vector>
#include <deque>

class Graph
{
public:
    using weight_t = int;
    using vertex_t = int;
    using distance_t = weight_t;

    explicit Graph(size_t size) : size(size) {}
    void insert_edge(vertex_t source, vertex_t sink, weight_t weight = 0);
    template<class Visitor>
    void dfs(vertex_t source, const Visitor& v);
    template<class Visitor>
    void bfs(vertex_t source, const Visitor& v);

    auto dijkstra(vertex_t source) -> std::vector<std::pair<weight_t, std::deque<vertex_t>>>;
private:
    size_t size;
    std::multimap<vertex_t, std::pair<vertex_t, weight_t>> edges;
};
