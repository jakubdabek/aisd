#include "DirectedGraph.hpp"

#include "priority_queue.tpp"
#include "util.hpp"

#include <map>
#include <stack>
#include <optional>
#include <algorithm>


void DirectedGraph::insert_edge(vertex_t source, vertex_t sink, weight_t weight)
{
    edges.emplace(source, std::make_pair(sink, weight));
}

struct dijkstra_vertex_info
{
    bool visited;
    std::optional<DirectedGraph::distance_t> distance;
    DirectedGraph::vertex_t previous;

    friend std::ostream& operator<<(std::ostream& os, const dijkstra_vertex_info& info)
    {
        return os << "{ " 
                  << std::boolalpha << info.visited << ", " 
                  << info.distance.value_or(-1) << ", "
                  << info.previous
                  << " }";
    }
};

auto DirectedGraph::dijkstra(vertex_t source, bool verbose) const -> std::vector<std::pair<weight_t, std::deque<vertex_t>>>
{
    if (source >= size)
        return {};
    
    std::ostream& os = maybe_stream(verbose);

    std::vector<dijkstra_vertex_info> vertex_info;
    vertex_info.reserve(size);

    for (vertex_t i = 0; i < size; i++)
    {
        vertex_info.push_back({ false, std::nullopt, i });
    }

    auto& source_info = vertex_info[source];
    source_info.visited = true;
    source_info.distance = 0;
    source_info.previous = source;

    priority_queue<vertex_t> q;
    q.insert(source, 0);

    while (!q.empty())
    {
        const auto current = *q.pop();
        os << "In vertex " << current << std::endl;
        const auto range = edges.equal_range(current);
        auto& current_info = vertex_info[current];
        const auto& current_distance = *current_info.distance;

        for (auto it = range.first; it != range.second; ++it)
        {
            auto next = it->second.first;
            auto &next_info = vertex_info[next];
            os << "Checking " << it->second.first << ", " << next_info << std::endl;
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

    for (vertex_t i = 0; i < size; i++)
    {
        os << i << ": " << vertex_info[i].previous << std::endl;
    }

    std::vector<std::pair<weight_t, std::deque<vertex_t>>> paths;
    paths.reserve(edges.size() - 1);

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
                path.emplace_front(current->first + 1);
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

auto DirectedGraph::kosaraju(bool verbose) const -> std::map<vertex_t, std::vector<vertex_t>>
{
    std::ostream& os = maybe_stream(verbose);

    std::vector<vertex_t> l;
    std::vector<bool> visited(size, false);
    std::multimap<vertex_t, vertex_t> transposed;

    std::function<void(vertex_t)> dfs = [&](vertex_t v)
    {
        if (visited[v])
            return;
        
        visited[v] = true;
        os << "Visiting " << v << std::endl;
        auto range = edges.equal_range(v);
        for (auto it = range.first; it != range.second; ++it)
        {
            auto next = it->second.first;
            transposed.emplace(next, v);
            os << "Entering " << next << std::endl;
            dfs(next);
            os << "Back to " << v << std::endl;
        }
        l.push_back(v);
    };

    dfs(0);

    visited = std::vector<bool>(size, false);
    std::map<vertex_t, std::vector<vertex_t>> components;

    std::function<void(vertex_t, vertex_t)> assign = [&](vertex_t v, vertex_t root)
    {
        if (visited[v])
            return;

        os << "Visiting " << v << std::endl;

        auto [it, done] = components.try_emplace(root);
        it->second.push_back(v);
        visited[v] = true;

        auto range = transposed.equal_range(v);
        for (auto it = range.first; it != range.second; ++it)
        {
            auto next = it->second;
            assign(next, root);
        }
    };

    os << l << std::endl;

    for (auto it = l.rbegin(); it != l.rend(); ++it)
    {
        assign(*it, *it);
    }

    return components;
}

struct tarjan_vertex_info
{
    std::optional<int> index;
    int lowlink;
    bool onStack = false;
};

auto DirectedGraph::tarjan(bool verbose) const -> std::vector<std::vector<vertex_t>>
{
    std::ostream& os = maybe_stream(verbose);

    std::vector<tarjan_vertex_info> infos(size);
    std::for_each(infos.begin(), infos.end(), [i = 0](tarjan_vertex_info& info) mutable
    {
        info.lowlink = i++;
    });

    int index = 0;
    std::stack<vertex_t> s;
    std::vector<std::vector<vertex_t>> sccs;

    std::function<void(vertex_t)> strongconnect = [&](vertex_t v)
    {
        os << "Entering " << v << std::endl;
        auto &info = infos[v];
        info.index = index;
        info.lowlink = index;
        index++;
        s.push(v);
        info.onStack = true;

        auto range = edges.equal_range(v);
        for (auto it = range.first; it != range.second; ++it)
        {
            auto &next_info = infos[it->second.first];
            if (!next_info.index.has_value())
            {
                strongconnect(it->second.first);
                info.lowlink = std::min(info.lowlink, next_info.lowlink);
            }
            else if (next_info.onStack)
            {
                info.lowlink = std::min(info.lowlink, *next_info.index);
            }
        }

        if (info.index == info.lowlink)
        {
            auto& scc = sccs.emplace_back();
            while (true)
            {
                auto w = s.top(); s.pop();
                infos[w].onStack = false;
                scc.push_back(w + 1);
                if (w == v)
                    break;
            }
        }
    };


    for (vertex_t i = 0; i < size; i++)
    {
        if (!infos[i].index.has_value())
            strongconnect(i);
    }

    std::reverse(sccs.begin(), sccs.end());
    return sccs;
}
