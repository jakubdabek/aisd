#include "util.hpp"

#include <iostream>
#include <fstream>
#include <sstream>
#include <numeric>
#include <random>
#include <chrono>
#include <optional>
#include <map>
#include <set>
#include <string>
#include <queue>

using std::string_literals::operator ""s;

using set_t = std::set<std::string, std::less<>>;

void main_gplk(int size, const set_t&);
void main_flow(int size, const set_t&);

int main(int argc, char *argv[])
{
    if (argc < 2 || argv[1] != "--size"s)
        return 1;
    
    int size;
    if (!(std::stringstream{argv[2]} >> size))
        return 1;

    set_t options;
    for (int i = 3; i < argc; i++)
    {
        options.emplace(argv[i]);
    }

    if (options.count("--gplk") > 0)
        main_gplk(size, options);
    else
        main_flow(size, options);
}

using vertex_t = int;
using flow_t = int;

struct edmonds_karp_info
{
    edmonds_karp_info() = default;
    explicit edmonds_karp_info(flow_t cap): capacity(cap) {}

    flow_t capacity = 0;
    flow_t flow = 0;

    friend std::ostream& operator<<(std::ostream& os, const edmonds_karp_info& a)
    { 
        return os << "info(" << a.flow << "/" << a.capacity << ")";
    }
};

void bfs(
    const std::vector<std::vector<edmonds_karp_info>>& graph,
    std::vector<std::optional<vertex_t>>& predecessors,
    int bitLength,
    vertex_t graphSize)
{
    const vertex_t source = 0;
    const vertex_t sink = graphSize - 1;
    std::queue<vertex_t> bfsQ;
    bfsQ.push(source);
    while (!bfsQ.empty())
    {
        auto current = bfsQ.front();
        bfsQ.pop();
        auto& adjacent = graph[current];
        for (int i = 0; i < bitLength; i++)
        {
            auto& edge = adjacent[i];
            auto next = current ^ (1 << i);
            auto& pred = predecessors[next];
            if (next != source && !pred.has_value() && edge.capacity > edge.flow)
            {
                pred = i;
                if (next == sink)
                    return;
                bfsQ.emplace(next);
            }
        }
    }
}

flow_t flow(const std::vector<std::vector<edmonds_karp_info>>& graph, const std::vector<std::optional<vertex_t>>& predecessors)
{
    const auto& sinkPredecessor = predecessors.back();
    if (!sinkPredecessor.has_value())
        return 0;

    int currentFlow = graph.size() + 1;
    const vertex_t sink = graph.size() - 1;

    // find possible flow on shortest path
    vertex_t previous = sink;
    for (std::optional<vertex_t> currentPos = sinkPredecessor; currentPos.has_value(); )
    {
        const auto current = previous ^ (1 << *currentPos);

        const auto& edge = graph[current][*currentPos];
        currentFlow = std::min(currentFlow, edge.capacity - edge.flow);

        currentPos = predecessors[current];
        previous = current;
    }

    return currentFlow;
}

void update_edges(
    std::vector<std::vector<edmonds_karp_info>>& graph,
    const std::vector<std::optional<vertex_t>>& predecessors,
    const flow_t currentFlow,
    const vertex_t graphSize)
{
    const auto& sinkPredecessor = predecessors.back();

    const vertex_t sink = graphSize - 1;

    vertex_t previous = sink;
    for (std::optional<vertex_t> currentPos = sinkPredecessor; currentPos.has_value(); )
    {
        const auto current = previous ^ (1 << *currentPos);

        auto& edge = graph[current][*currentPos];
        edge.flow = edge.flow + currentFlow;

        auto& reverseEdge = graph[previous][*currentPos];
        reverseEdge.flow = reverseEdge.flow - currentFlow;

        currentPos = predecessors[current];
        previous = current;
    }
}

void main_flow(const int bitLength, const set_t&)
{
    auto wholeMainStartTime = std::chrono::high_resolution_clock::now();

    const vertex_t graphSize = 1 << bitLength;

    std::vector<uint8_t> hammingWeights(graphSize);
    hammingWeights[0] = 0;
    for (vertex_t i = 1; i < graphSize; i++)
    {
        hammingWeights[i] = (i & 1) + hammingWeights[i / 2];
    }

    std::vector<std::vector<edmonds_karp_info>> graph;
    graph.reserve(graphSize);
    // std::mt19937 rng{std::random_device{}()};
    std::mt19937 rng{42};
    for (vertex_t i = 0; i < graphSize; i++)
    {
        int h = hammingWeights[i];
        auto& c = graph.emplace_back(bitLength);
        std::uniform_int_distribution dist{1, 1 << std::max(h + 1, bitLength - h)};
        for (int j = 0; j < bitLength; j++)
        {
            if ((i & (1 << j)) == 0)
            {
                c[j] = edmonds_karp_info(dist(rng));
            }
        }
    }

    // graph = 
    // {
    //     { edmonds_karp_info(5), edmonds_karp_info(2), edmonds_karp_info(1) },
    //     { edmonds_karp_info(0), edmonds_karp_info(3), edmonds_karp_info(4) },
    //     { edmonds_karp_info(2), edmonds_karp_info(0), edmonds_karp_info(2) },
    //     { edmonds_karp_info(0), edmonds_karp_info(0), edmonds_karp_info(8) },
    //     { edmonds_karp_info(1), edmonds_karp_info(3), edmonds_karp_info(0) },
    //     { edmonds_karp_info(0), edmonds_karp_info(7), edmonds_karp_info(0) },
    //     { edmonds_karp_info(8), edmonds_karp_info(0), edmonds_karp_info(0) },
    //     { edmonds_karp_info(0), edmonds_karp_info(0), edmonds_karp_info(0) },
    // };

    // std::cout << graph << std::endl;

    const int source = 0;
    const int sink = graphSize - 1;

    auto startTime = std::chrono::high_resolution_clock::now();

    int maxFlow = 0;
    int augumentingPathsCount = 0;
    std::vector<std::optional<vertex_t>> predecessors(graphSize);
    while (true)
    {
        std::fill(predecessors.begin(), predecessors.end(), std::nullopt);
        bfs(graph, predecessors, bitLength, graphSize);

        flow_t currentFlow = flow(graph, predecessors);
        if (currentFlow == 0)
            break;
            
        if (currentFlow == (graphSize + 1))
        {
            std::cerr << "Incorrect flow" << std::endl;
            exit(1);
        }
        update_edges(graph, predecessors, currentFlow, graphSize);

        maxFlow += currentFlow;
        augumentingPathsCount++;
        // std::cout << predecessors << std::endl;
        // std::cout << graph << std::endl;
    }

    auto endTime = std::chrono::high_resolution_clock::now();

    // std::cout << graph << std::endl;
    std::cout << "\n" << std::string(50, '-') << "\n\n";
    std::cout << "Size: " << bitLength << "\n";
    std::cout << "Time: " << std::chrono::duration_cast<std::chrono::duration<double, std::milli>>(endTime - startTime).count() << "ms\n";
    std::cout << "Time (with init): " << std::chrono::duration_cast<std::chrono::duration<double, std::milli>>(endTime - wholeMainStartTime).count() << "ms\n";
    std::cout << "Flow: " << maxFlow << "\n";
    std::cout << "Augumenting paths: " << augumentingPathsCount << std::endl;

    // repeat
    //     (Run a bfs to find the shortest s-t path.
    //      We use 'pred' to store the edge taken to get to each vertex,
    //      so we can recover the path afterwards)
    //     q := queue()
    //     q.push(s)
    //     pred := array(graph.length)
    //     while not empty(q)
    //         cur := q.pull()
    //         for Edge e in graph[cur]
    //              if pred[e.t] = null and e.t ≠ s and e.cap > e.flow
    //                 pred[e.t] := e
    //                 q.push(e.t)
    
    //     if not (pred[t] = null)         
    //         (We found an augmenting path.
    //          See how much flow we can send) 
    //         df := ∞
    //         for (e := pred[t]; e ≠ null; e := pred[e.s])
    //             df := min(df, e.cap - e.flow)
    //         (And update edges by that amount)
    //         for (e := pred[t]; e ≠ null; e := pred[e.s])
    //             e.flow  := e.flow + df
    //             e.rev.flow := e.rev.flow - df
    //         flow := flow + df
    
    // until pred[t] = null  (i.e., until no augmenting path was found)
    // return flow
}

void main_gplk(const int, const set_t&) {}
