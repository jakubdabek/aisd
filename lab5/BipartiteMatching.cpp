#include "MaxFlow.hpp"

#include <queue>
#include <iostream>
#include <vector>
#include <chrono>
#include <string>
#include <random>
#include <fstream>


namespace {
using std::string_literals::operator ""s;
using vertex_t = int;
using flow_t = int;

struct edmonds_karp_info
{
    edmonds_karp_info() = delete;
    explicit edmonds_karp_info(vertex_t vertex, int8_t capacity = 0): sink(vertex), capacity(capacity) {}

    vertex_t sink;
    edmonds_karp_info *reversed;
    int8_t capacity;
    int8_t flow;

    friend std::ostream& operator<<(std::ostream& os, const edmonds_karp_info& a)
    { 
        return os << "info(" << a.flow << "/" << a.capacity << ")";
    }
};

struct my_queue
{

};

void bfs(
    const std::vector<std::vector<edmonds_karp_info>>& graph,
    std::vector<std::optional<std::pair<vertex_t, int8_t>>>& predecessors,
    int degree,
    const vertex_t source,
    const vertex_t sink)
{
    std::queue<vertex_t> bfsQ;
    bfsQ.push(source);
    while (!bfsQ.empty())
    {
        auto current = bfsQ.front();
        bfsQ.pop();
        auto& adjacent = graph[current];
        for (int i = 0; i < adjacent.size(); i++)
        {
            auto& edge = adjacent[i];
            auto next = edge.sink;
            auto& pred = predecessors[next];
            if (next != source && !pred.has_value() && edge.capacity > edge.flow)
            {
                pred.emplace(current, i);
                if (next == sink)
                    return;
                bfsQ.emplace(next);
            }
        }
    }
}

flow_t flow(
    const std::vector<std::vector<edmonds_karp_info>>& graph,
    const std::vector<std::optional<std::pair<vertex_t, int8_t>>>& predecessors,
    const vertex_t sink)
{
    const auto& sinkPredecessor = predecessors[sink];
    if (!sinkPredecessor.has_value())
        return 0;

    int currentFlow = 2;

    // find possible flow on shortest path
    for (auto current = sinkPredecessor; current.has_value(); )
    {
        const auto& edge = graph[current->first][current->second];
        currentFlow = std::min(currentFlow, edge.capacity - edge.flow);

        current = predecessors[current->first];
    }

    return currentFlow;
}

void update_edges(
    std::vector<std::vector<edmonds_karp_info>>& graph,
    const std::vector<std::optional<std::pair<vertex_t, int8_t>>>& predecessors,
    const flow_t currentFlow,
    const vertex_t sink)
{
    const auto& sinkPredecessor = predecessors[sink];

    for (auto current = sinkPredecessor; current.has_value(); )
    {
        auto& edge = graph[current->first][current->second];
        edge.flow = edge.flow + currentFlow;
        edge.reversed->flow = edge.reversed->flow - currentFlow;

        current = predecessors[current->first];
    }
}

bool find_match(const std::vector<std::vector<vertex_t>>& graph,
    std::vector<bool>& seen,
    std::vector<vertex_t>& match,
    const vertex_t i)
{
    for (vertex_t j : graph[i])
    {
        if (!seen[j])
        {
            seen[j] = true;
            if (match[j] < 0 || find_match(graph, seen, match, match[j]))
            {
                match[j] = i;
                return true;
            }
        }
    }
    return false;
}

int bpm(std::vector<std::vector<vertex_t>>& graph)
{
    int matches = 0;
    std::vector<bool> seen(graph.size(), false);
    std::vector<vertex_t> match(graph.size(), -1);

    for (int i = 0; i < graph.size(); i++)
    {
        if (find_match(graph, seen, match, i))
            matches++;
    }

    return matches;
}
} // namespace

void main_match(const int graphExponent, const int degree, const options_t& options)
{
    auto wholeMainStartTime = std::chrono::high_resolution_clock::now();

    const vertex_t graphSize = 1 << graphExponent;

    std::vector<std::vector<vertex_t>> graph;
    graph.reserve(graphSize);

    if (options[0] == "--read")
    {
        std::fstream file(options[1]);
        if (!file.is_open())
        {
            std::cerr << "Wrong file: " << options[1] << std::endl;
            exit(1);
        }

        for (vertex_t i = 0; i < graphSize; i++)
        {
            auto& c = graph.emplace_back();
            c.reserve(degree);

            for (int j = 0; j < degree; j++)
            {
                int a;
                if (!(file >> a))
                {
                    std::cerr << "Wrong data in file: " << options[1] << std::endl;
                    exit(1);
                }

                c.push_back(a);
            }
        }
    }
    else
    {
        // std::mt19937 rng{std::random_device{}()};
        std::minstd_rand rng{std::random_device{}()};
        // std::mt19937 rng{42};
        for (vertex_t i = 0; i < graphSize; i++)
        {
            auto& c = graph.emplace_back();
            c.reserve(degree);
            while (c.size() < degree)
            {
                std::uniform_int_distribution dist(0, graphSize - 1);
                vertex_t val = dist(rng);
                if (std::find(c.begin(), c.end(), val) == c.end())
                    c.push_back(val);
            }
            std::sort(c.begin(), c.end());
            // for (int populationSize = graphSize, count = degree, i = 0; count > 0; ++i, --populationSize)
            // {
            //     std::uniform_int_distribution dist(0, populationSize - 1);
            //     if (dist(rng) < count)
            //     {
            //         --count;
            //         c.emplace_back(i);
            //     }
            // }
        }

        if (options[0] == "--write")
        {
            std::fstream file(options[1], std::fstream::out | std::fstream::trunc);
            if (!file.is_open())
            {
                std::cerr << "Wrong file: " << options[1] << std::endl;
                exit(1);
            }

            for (vertex_t i = 0; i < graphSize; i++)
            {
                for (int j = 0; j < degree; j++)
                {
                    vertex_t v = graph[i][j];
                    // std::cout << v << " ";
                    if (!(file << v))
                    {
                        std::cerr << "Wrong data in file: " << options[1] << std::endl;
                        exit(1);
                    }
                    file << " ";
                }
                file << "\n";
            }

            file.close();
            auto endTime = std::chrono::high_resolution_clock::now();
            std::cout << "Done writing: " << options[1] 
                      << " in " << std::chrono::duration_cast<std::chrono::duration<double, std::milli>>(endTime - wholeMainStartTime).count() << "ms" << std::endl;

            exit(0);
        }
    }

    const bool terse = std::find(options.begin(), options.end(), "--terse") != options.end();
    
    auto startTime = std::chrono::high_resolution_clock::now();

    int result = bpm(graph);

    auto endTime = std::chrono::high_resolution_clock::now();
    if (terse)
    {
        std::cout << graphExponent << " "
                  << degree << " "
                  << result << " " 
                  << std::chrono::duration_cast<std::chrono::duration<double, std::milli>>(endTime - startTime).count() << std::endl;
    }
    else
    {
        std::cout << "\n" << std::string(50, '-') << "\n\n";
        // std::cout << graph << "\n";
        std::cout << "Size: " << graphExponent << "\n";
        std::cout << "Time: " << std::chrono::duration_cast<std::chrono::duration<double, std::milli>>(endTime - startTime).count() << "ms\n";
        std::cout << "Time (with init): " << std::chrono::duration_cast<std::chrono::duration<double, std::milli>>(endTime - wholeMainStartTime).count() << "ms\n";
        std::cout << "Result: " << result << "\n";
    }
    
}

void main_match2(const int graphExponent, const int degree, const options_t&)
{
    auto wholeMainStartTime = std::chrono::high_resolution_clock::now();

    const vertex_t graphSize = 1 << graphExponent;

    std::vector<std::vector<edmonds_karp_info>> graph;
    graph.reserve(graphSize * 2 + 2);
    std::mt19937 rng{std::random_device{}()};
    // std::mt19937 rng{42};
    for (vertex_t i = 0; i < graphSize; i++)
    {
        auto& c = graph.emplace_back();
        c.reserve(degree);
        for (int populationSize = graphSize, count = degree, i = 0; count > 0; ++i, --populationSize)
        {
            std::uniform_int_distribution dist(0, populationSize - 1);
            if (dist(rng) < count)
            {
                --count;
                c.emplace_back(i, 1);
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

    const int source = graphSize * 2;
    const int sink =  graphSize * 2 + 1;

    auto startTime = std::chrono::high_resolution_clock::now();

    int maxFlow = 0;
    int augumentingPathsCount = 0;
    std::vector<std::optional<std::pair<vertex_t, int8_t>>> predecessors(graphSize);
    while (true)
    {
        std::fill(predecessors.begin(), predecessors.end(), std::nullopt);
        bfs(graph, predecessors, degree, source, sink);

        flow_t currentFlow = flow(graph, predecessors, sink);
        if (currentFlow == 0)
            break;
            
        if (currentFlow == 2)
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
    std::cout << "Size: " << graphExponent << "\n";
    std::cout << "Time: " << std::chrono::duration_cast<std::chrono::duration<double, std::milli>>(endTime - startTime).count() << "ms\n";
    std::cout << "Time (with init): " << std::chrono::duration_cast<std::chrono::duration<double, std::milli>>(endTime - wholeMainStartTime).count() << "ms\n";
    std::cout << "Flow: " << maxFlow << "\n";
    std::cout << "Augumenting paths: " << augumentingPathsCount << std::endl;
}