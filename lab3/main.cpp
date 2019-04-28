#include "heap.tpp"
#include "priority_queue.tpp"
#include "Graph.hpp"

#include <iostream>
#include <sstream>
#include <random>
#include <chrono>
#include <map>

namespace impl {
    using std::begin;
    using std::end;

    template<class T>
    using begin_t = decltype(begin(std::declval<const T&>()));
    template<class T>
    using end_t = decltype(end(std::declval<const T&>()));

    template<class T, class = void>
    struct is_print_iterable : std::false_type {};

    template<class T>
    struct is_print_iterable<T, std::void_t<begin_t<T>, end_t<T>, decltype(++std::declval<begin_t<T>&>()), decltype(*std::declval<begin_t<T>&>())>>
        : std::true_type {};
    
    template<class... Ts>
    struct is_print_iterable<std::basic_string<Ts...>, void> : std::false_type {};

    template<int n>
    struct is_print_iterable<char[n], void> : std::false_type {};

    template<class T>
    constexpr bool is_print_iterable_v = is_print_iterable<T>::value;

    template<template<class...> class, class...>
    struct is_instantiation : std::false_type {};

    template<template<class...> class T, class... U>
    struct is_instantiation<T, T<U...>> : std::true_type {};

    template<class T>
    constexpr bool is_iterable_v = is_instantiation<std::vector, T>::value || is_instantiation<std::deque, T>::value;
}

template<class T>
std::enable_if_t<impl::is_print_iterable_v<T>, std::ostream&> operator<<(std::ostream& os, const T& iterable)
{
    char sep[] = "\0 ";
    os << '[';
    for (const auto& val : iterable)
    {
        os << sep << val;
        sep[0] = ',';
    }
    os << ']';
    return os;
}

template<class T, class U>
std::ostream& operator<<(std::ostream& os, const std::pair<T, U> p)
{
    return os << "(" << p.first << ", " << p.second << ")";
}

void main_int(int n, std::mt19937& rng);
void main_double(int n, std::mt19937& rng);
void main_check();
void main_interactive();
void main_dijkstra();

int main(int argc, char *argv[])
{
    std::mt19937 rng{std::random_device{}()};
    int n = 30;
    if (argc > 1) std::stringstream{argv[1]} >> n;
    // main_int(n, rng);
    // main_double(n, rng);
    // main_check();
    // main_interactive();
    main_dijkstra();
}

void main_int(int n, std::mt19937& rng)
{
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

void main_double(int n, std::mt19937& rng)
{
    std::uniform_real_distribution dist(0.0, 10.0);

    std::vector<double> vec(n);
    for (auto &val : vec)
        val = dist(rng);

    std::cout << array_heap<double>{vec} << std::endl;
    array_heap<double>::heapify(vec);
}

void main_check()
{
    std::mt19937 rng{std::random_device{}()};
    int n = 30;
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

void main_interactive()
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
        }
    };
    for (int i = 0; i < ops || ops < 0; i++)
    {
        std::string op;
        std::cout << "Enter operation:\n"
                  << "  insert x p\n"
                  << "  empty\n"
                  << "  top\n"
                  << "  pop\n"
                  << "  priority x p\n"
                  << "  print\n";
        if (!(std::cin >> op))
            return;
        if (auto o = operations.find(op); o != operations.end())
        {
            if (!o->second())
            {
                std::cout << "Wrong arguments\n";
                std::cin.clear();
            }
        }
        else
        {
            std::cout << "Wrong operation\n";
        }
    }
}

void main_dijkstra()
{
    int v, e;
    std::cout << "Enter v, e" << std::endl;
    std::cin >> v >> e;
    Graph graph(v);

    for (int i = 0; i < e; i++)
    {
        Graph::vertex_t u, v;
        Graph::weight_t w;
        std::cout << "Enter edge " << i << std::endl;
        std::cin >> u >> v >> w;
        graph.insert_edge(u, v, w);
    }

    Graph::vertex_t source;
    std::cout << "Enter source" << std::endl;
    std::cin >> source;

    auto before = std::chrono::high_resolution_clock::now();
    auto paths = graph.dijkstra(source);
    auto after = std::chrono::high_resolution_clock::now();
    std::cout << "Paths:" << std::endl;
    std::cout << paths << std::endl;
    std::cout << "Time: " << std::chrono::duration_cast<std::chrono::duration<double, std::micro>>(after - before).count() << "µs" << std::endl;
}
