#include "RealUndirectedGraph.hpp"

#include "priority_queue.tpp"
#include "util.hpp"

#include <vector>
#include <algorithm>
#include <optional>
#include <tuple>


void RealUndirectedGraph::insert_edge(vertex_t v, vertex_t u, weight_t weight)
{
    edges.emplace(v, std::make_pair(u, weight));
    edges.emplace(u, std::make_pair(v, weight));
}

struct kruskal_vertex_info
{
    RealUndirectedGraph::vertex_t parent;
    int rank;
};

auto RealUndirectedGraph::kruskal() -> std::pair<weight_t, std::vector<std::pair<vertex_t, vertex_t>>>
{
    std::vector<std::tuple<vertex_t, vertex_t, weight_t>> unique_edges;
    unique_edges.reserve(edges.size() / 2);

    for (const auto& e : edges)
    {
        if (e.first < e.second.first)
            unique_edges.emplace_back(e.first, e.second.first, e.second.second);
    }

    std::sort(unique_edges.begin(), unique_edges.end(), [](const auto& a, const auto& b) 
    { 
        return std::get<2>(a) < std::get<2>(b);
    });

    std::vector<kruskal_vertex_info> forest(size);
    std::for_each(forest.begin(), forest.end(), [i = 0](auto& info) mutable
    {
        info.parent = i++;
        info.rank = 0;
    });

    auto find = [&](vertex_t i) -> kruskal_vertex_info&
    {
        while (forest[i].parent != i)
            i = forest[i].parent;
        
        return forest[i];
    };

    auto my_union = [&](kruskal_vertex_info& info_u, kruskal_vertex_info& info_v)
    {
        if (info_u.rank <= info_v.rank)
        {
            info_v.parent = info_u.parent;
            if (info_u.rank == info_v.rank)
                info_u.rank++;
        }
        else
        {
            info_u.parent = info_v.parent;
        }
    };

    weight_t total_weight = 0;

    std::vector<std::pair<vertex_t, vertex_t>> tree;
    tree.reserve(size - 1);

    for (auto& [u, v, w] : unique_edges)
    {
        auto& info_u = find(u);
        auto& info_v = find(v);

        if (info_u.parent != info_v.parent)
        {
            my_union(info_u, info_v);
            total_weight += w;
            tree.emplace_back(u, v);
        }
    }
    
    return { total_weight, std::move(tree) };
}

struct prim_vertex_info
{
    RealUndirectedGraph::vertex_t parent;
    std::optional<RealUndirectedGraph::weight_t> weight;
    bool visited = false;

    friend std::ostream& operator<<(std::ostream& os, const prim_vertex_info& info)
    {
        return os << "{ " 
                  << std::boolalpha << info.visited << ", " 
                  << info.weight.value_or(-1) << ", "
                  << info.parent
                  << " }";
    }
};

auto RealUndirectedGraph::prim(bool verbose) -> std::pair<weight_t, std::vector<std::pair<vertex_t, vertex_t>>>
{
    std::ostream& os = maybe_stream(verbose);

    const vertex_t source = 0;

    std::vector<prim_vertex_info> infos(size);
    std::for_each(infos.begin(), infos.end(), [i = 0](prim_vertex_info& info) mutable
    {
        info.parent = i++;
    });

    infos[source].weight = 0;

    priority_queue<vertex_t, weight_t> q;
    q.insert(source, 0);

    weight_t total_weight = 0;

    while (!q.empty())
    {
        const auto current = *q.pop();
        const auto range = edges.equal_range(current);
        auto& current_info = infos[current];
        if (!current_info.visited)
            total_weight += *current_info.weight;
        current_info.visited = true;
        os << "In vertex " << current << std::endl;

        for (auto it = range.first; it != range.second; ++it)
        {
            const auto& [u, w] = it->second;
            auto& info = infos[u];
            os << "Checking " << u << ", " << info << std::endl;
            if (!info.visited && (!info.weight.has_value() || *info.weight > w))
            {
                info.weight = w;
                q.insert(u, w);
                // q.priority(u, w);
                info.parent = current;
            }
        }
    }

    std::vector<std::pair<vertex_t, vertex_t>> tree;
    tree.reserve(size - 1);

    for (vertex_t i = 0; i < size; i++)
    {
        if (i == source)
            continue;

        auto other = infos[i].parent;
        if (other < i)
            tree.emplace_back(other, i);
        else
            tree.emplace_back(i, other);
    }

    return { total_weight, std::move(tree) };
}
