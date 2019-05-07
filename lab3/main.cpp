#include "heap.tpp"
#include "priority_queue.tpp"
#include "DirectedGraph.hpp"
#include "RealUndirectedGraph.hpp"
#include "util.hpp"

#include <iostream>
#include <sstream>
#include <numeric>
#include <random>
#include <chrono>
#include <map>
#include <set>


void main_int(const std::set<std::string>&);
void main_double(const std::set<std::string>&);
void main_check(const std::set<std::string>&);
void main_priority_queue(const std::set<std::string>&);
void main_dijkstra(const std::set<std::string>&);
void main_kruskal(const std::set<std::string>&);
void main_prim(const std::set<std::string>&);
void main_kosaraju(const std::set<std::string>&);
void main_tarjan(const std::set<std::string>&);

int main(int argc, char *argv[])
{
    std::map<std::string, std::function<void(const std::set<std::string>&)>> commands
    {
        {
            "-q",
            main_priority_queue,
        },
        {
            "-k",
            main_kruskal,
        },
        {
            "-p",
            main_prim,
        },
        {
            "-d",
            main_dijkstra,
        },
        {
            "-scck",
            main_kosaraju,
        },
        {
            "-scct",
            main_tarjan,
        },
        {
            "-scc",
            main_tarjan,
        },
    };

    std::string command = "-q";
    if (argc > 1)
        command = argv[1];
    
    std::set<std::string> options;
    for (int i = 2; i < argc; i++)
    {
        options.emplace(argv[i]);
    }

    if (auto c = commands.find(command); c != commands.end())
    {
        c->second(options);
    }
}

void main_int(const std::set<std::string>& options)
{
    std::mt19937 rng{std::random_device{}()};
    int n = 30;
    if (!options.empty()) std::stringstream{*options.begin()} >> n;
    std::uniform_int_distribution dist(1, 1000);

    std::vector<int> vec(n);
    for (auto &val : vec)
        val = dist(rng);
    int i = 0;
    for (auto &val : vec)
        val = i++;

    std::cout << vec << std::endl;

    auto heap = array_heap<int>{vec};
    std::cout << heap << std::endl;
    while (heap.size() > 0)
    {
        std::cout << "\n" << std::string(70, '*') << "\n";
        std::cout << "Popping a value\n";
        std::cout << heap.pop().value_or(0) << std::endl;
    }
    // array_heap<int>::heapify(vec);
}

void main_double(const std::set<std::string>& options)
{
    std::mt19937 rng{std::random_device{}()};
    int n = 30;
    if (!options.empty()) std::stringstream{*options.begin()} >> n;
    std::uniform_real_distribution dist(0.0, 10.0);

    std::vector<double> vec(n);
    for (auto &val : vec)
        val = dist(rng);

    std::cout << array_heap<double>{vec} << std::endl;
    array_heap<double>::heapify(vec);
}

void main_check(int n)
{
    std::mt19937 rng{std::random_device{}()};
    priority_queue<int> p;

    std::uniform_int_distribution dist(1, 1000);
    std::vector<int> vec(n);
    for (auto& val : vec)
        val = dist(rng);
    int i = n - 1;
    for (auto &val : vec)
        val = i--;
    for (const auto& val : vec)
        p.insert(dist(rng), val);
    
    std::cout << p.print() << std::endl;
    auto check = [&]()
    {
        for (const auto& x : p.map)
        {
            // std::cout << x.first << " " << x.second << "\n";
            if (std::get<1>(p.vec[x.second]) != x.first)
                std::cout << "wrong\n";
        }
    };
    check();
    std::cout << "pop " << *p.pop() << "\n";
    check();
    std::cout << "pop " << *p.pop() << "\n";
    check();
    std::cout << "pop " << *p.pop() << "\n";
    check();
    std::cout << "pop " << *p.pop() << "\n";
    check();
    std::cout << p.print() << "\n";
}

void main_check(const std::set<std::string>& options)
{
    int n = 30;
    if (!options.empty()) std::stringstream{*options.begin()} >> n;
    main_check(n);
}

void main_priority_queue(const std::set<std::string>&)
{
    priority_queue<int> q;
    int ops;
    std::cout << "Enter number of operations\n";
    if (!(std::cin >> ops))
    {
        std::cout << "Wrong input\n";
        return;
    }

    std::unordered_map<std::string, std::function<bool()>> operations
    {
        {
            "insert",
            [&]
            {
                int value, priority;
                if (!(std::cin >> value >> priority))
                    return false;
                q.insert(value, priority);
                return true;
            }
        },
        {
            "empty",
            [&]
            {
                std::cout << q.empty() << std::endl;
                return true;
            }
        },
        {
            "top",
            [&]
            {
                auto a = q.top();
                if (a)
                    std::cout << *a << '\n';
                else
                    std::cout << "no top\n";

                return true;
            }
        },
        {
            "pop",
            [&]
            {
                auto a = q.pop();
                if (a)
                    std::cout << *a << '\n';
                else
                    std::cout << "nothing to pop\n";

                return true;
            }
        },
        {
            "priority",
            [&]
            {
                int value, priority;
                if (!(std::cin >> value >> priority))
                    return false;
                q.priority(value, priority);
                return true;
            }
        },
        {
            "print",
            [&]
            {
                std::cout << q.print() << '\n';\
                return true;
            }
        },
        {
            "help",
            [&]
            {
                std::cout << "Enter operation:"
                          << " insert x p |"
                          << " empty |"
                          << " top |"
                          << " pop |"
                          << " priority x p |"
                          << " print |"
                          << " help\n";
                return true;
            }
        },
    };

    operations.at("help")();

    for (int i = 0; i < ops || ops < 0; i++)
    {
        std::string op;
        
        std::cin.clear();
        if (!(std::cin >> op))
            return;

        if (auto o = operations.find(op); o != operations.end())
        {
            if (!o->second())
            {
                std::cout << "Wrong arguments\n";
                std::cin.clear();
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            }
        }
        else
        {
            std::cout << "Wrong operation\n";
        }
    }
}

void main_dijkstra(const std::set<std::string>& options)
{
    std::ostream& os = maybe_stream(options.count("-i") > 0);
    int v, e;
    os << "Enter v, e" << std::endl;
    std::cin >> v >> e;
    DirectedGraph graph(v);

    for (int i = 0; i < e; i++)
    {
        DirectedGraph::vertex_t u, v;
        DirectedGraph::weight_t w;
        os << "Enter edge " << i << std::endl;
        std::cin >> u >> v >> w;
        graph.insert_edge(u, v, w);
    }

    DirectedGraph::vertex_t source;
    os << "Enter source" << std::endl;
    std::cin >> source;

    auto before = std::chrono::high_resolution_clock::now();
    auto paths = graph.dijkstra(source, options.count("-v") > 0);
    auto after = std::chrono::high_resolution_clock::now();
    std::cout << "Paths:" << std::endl;
    std::cout << paths << std::endl;
    std::cout << "Time: " << std::chrono::duration_cast<std::chrono::duration<double, std::milli>>(after - before).count() << "ms" << std::endl;
}

void main_kruskal(const std::set<std::string>& options)
{
    std::ostream& os = maybe_stream(options.count("-i") > 0);
    int v, e;
    os << "Enter v, e" << std::endl;
    std::cin >> v >> e;
    RealUndirectedGraph graph(v);

    for (int i = 0; i < e; i++)
    {
        RealUndirectedGraph::vertex_t u, v;
        RealUndirectedGraph::weight_t w;
        os << "Enter edge " << i << std::endl;
        std::cin >> u >> v >> w;
        graph.insert_edge(u, v, w);
    }

    auto before = std::chrono::high_resolution_clock::now();
    auto tree = graph.kruskal();
    auto after = std::chrono::high_resolution_clock::now();
    std::cout << "tree:" << std::endl;
    std::cout << tree << std::endl;
    std::cout << "Time: " << std::chrono::duration_cast<std::chrono::duration<double, std::milli>>(after - before).count() << "ms" << std::endl;
}

void main_prim(const std::set<std::string>& options)
{
    std::ostream& os = maybe_stream(options.count("-i") > 0);
    int v, e;
    os << "Enter v, e" << std::endl;
    std::cin >> v >> e;
    RealUndirectedGraph graph(v);

    for (int i = 0; i < e; i++)
    {
        RealUndirectedGraph::vertex_t u, v;
        RealUndirectedGraph::weight_t w;
        os << "Enter edge " << i << std::endl;
        std::cin >> u >> v >> w;
        graph.insert_edge(u, v, w);
    }

    auto before = std::chrono::high_resolution_clock::now();
    auto tree = graph.prim(options.count("-v") > 0);
    auto after = std::chrono::high_resolution_clock::now();
    std::cout << "tree:" << std::endl;
    std::cout << tree << std::endl;
    std::cout << "Time: " << std::chrono::duration_cast<std::chrono::duration<double, std::milli>>(after - before).count() << "ms" << std::endl;
}

template<class F>
void main_scc(const std::set<std::string>& options, F&& func)
{
    std::ostream& os = maybe_stream(options.count("-i") > 0);
    int v, e;
    os << "Enter v, e" << std::endl;
    std::cin >> v >> e;
    std::vector<std::pair<DirectedGraph::vertex_t, DirectedGraph::vertex_t>> edges(v);

    for (int i = 0; i < e; i++)
    {
        DirectedGraph::vertex_t u, v;
        os << "Enter edge " << i << std::endl;
        std::cin >> u >> v;
        edges.emplace_back(u, v);
    }
    auto doOnce = [&](const auto& e)
    {
        DirectedGraph graph(v);
        for (const auto& edge : e)
            graph.insert_edge(edge.first, edge.second);

        auto before = std::chrono::high_resolution_clock::now();
        auto paths = func(graph, options.count("-v") > 0);
        auto after = std::chrono::high_resolution_clock::now();
        std::cout << "Strongly connected components:" << std::endl;
        std::cout << paths << std::endl;
        std::cout << "Time: " << std::chrono::duration_cast<std::chrono::duration<double, std::milli>>(after - before).count() << "ms" << std::endl;
    };
    
    doOnce(edges);

    if (options.count("-sccv") > 0)
    {
        std::mt19937 rng{std::random_device{}()};

        for (int i = 0; i < 5; i++)
        {
            auto lol = edges;
            std::vector<DirectedGraph::vertex_t> permutation(v);
            std::iota(permutation.begin(), permutation.end(), 0);
            std::shuffle(permutation.begin(), permutation.end(), rng);
            std::transform(lol.begin(), lol.end(), lol.begin(), [&](auto val)
            {
                val.first = permutation[val.first];
                val.second = permutation[val.second];

                return val;
            });
            doOnce(lol);
        }
    }
}

void main_kosaraju(const std::set<std::string>& options)
{
    main_scc(options, [&](const auto& graph, bool verbose) { return graph.kosaraju(verbose); });
}

void main_tarjan(const std::set<std::string>& options)
{
    main_scc(options, [&](const auto& graph, bool verbose) { return graph.tarjan(verbose); });
}
