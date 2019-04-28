#include "Graph.hpp"

#include "priority_queue.tpp"

#include <map>
#include <optional>

void Graph::insert_edge(vertex_t source, vertex_t sink, weight_t weight)
{
    edges.emplace(source, std::make_pair(sink, weight));
}

struct dijkstra_vertex_info
{
    bool visited;
    std::optional<Graph::distance_t> distance;
    Graph::vertex_t previous;

    friend std::ostream& operator<<(std::ostream& os, const dijkstra_vertex_info& info)
    {
        return os << "{ " 
                  << std::boolalpha << info.visited << ", " 
                  << info.distance.value_or(-1) << ", "
                  << info.previous
                  << " }";
    }
};

auto Graph::dijkstra(vertex_t source) -> std::vector<std::pair<weight_t, std::deque<vertex_t>>>
{
    std::map<vertex_t, dijkstra_vertex_info> vertex_info;
    priority_queue<vertex_t> q;

    for (vertex_t i = 0; i < size; i++)
    {
        vertex_info.insert({ i, { false, std::nullopt, i }});
    }

    auto& source_info = vertex_info[source];
    source_info.visited = true;
    source_info.distance = 0;
    source_info.previous = source;

    q.insert(source, 0);

    while (!q.empty())
    {
        const auto current = *q.pop();
        std::cout << "In vertex " << current << std::endl;
        const auto range = edges.equal_range(current);
        auto& current_info = vertex_info[current];
        const auto& current_distance = *current_info.distance;

        for (auto it = range.first; it != range.second; ++it)
        {
            auto next = it->second.first;
            auto &next_info = vertex_info[next];
            std::cout << "Checking " << it->second.first << std::endl;
            if (!next_info.visited)
            {
                auto weight = it->second.second;
                if (!next_info.distance.has_value() || current_distance + weight < next_info.distance)
                {
                    next_info.distance = current_distance + weight;
                    q.priority(it->first, *next_info.distance);
                    next_info.previous = current;
                }

                q.insert(next, *next_info.distance);
            }
        }

        current_info.visited = true;
    }

    // for (const auto& v : vertex_info)
    // {
    //     std::cout << v.first << ": " << v.second << std::endl;
    // }

    std::vector<std::pair<weight_t, std::deque<vertex_t>>> paths;
    paths.reserve(edges.size() - 1);
    // for (vertex_t sink = 0; sink < size; ++sink)
    // {
    //     auto& [distance, path] = paths.emplace_back();
    //     path.push_back(sink);
    //     const auto& current_info = vertex_info[sink];
    //     distance = *current_info.distance;
    //     auto current = current_info.previous;

    //     for (auto prev = vertex_info[current].previous; current != prev; prev = vertex_info[prev].previous)
    //     {
    //         path.push_back(current);
    //         current = vertex_info[current].previous;
    //     }
    // }

    for (vertex_t sink = 0; sink < size; ++sink)
    {
        auto& [distance, path] = paths.emplace_back();
        const auto& sink_info = vertex_info[sink];
        if (sink_info.distance.has_value())
        {
            distance = *sink_info.distance;
            using current_t = std::optional<std::pair<vertex_t, std::reference_wrapper<const dijkstra_vertex_info>>>;
            for (current_t current{std::in_place, sink, sink_info};
                current.has_value();
                current = { current->second.get().previous, vertex_info[current->second.get().previous] })
            {
                path.emplace_front(current->first);
                if (current->first == current->second.get().previous)
                    break;
            }
        }
        else
        {
            distance = -1;
        }
        
    }

    return paths;
}
