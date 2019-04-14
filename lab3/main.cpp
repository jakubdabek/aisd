#include "heap.tpp"
#include "priority_queue.tpp"

#include <iostream>
#include <sstream>
#include <random>
#include <map>

template<class T>
std::ostream& operator<<(std::ostream& os, const std::vector<T>& vec)
{
    char sep[] = "\0 ";
    for (size_t i = 0; i < vec.size(); i++)
    {
        os << sep << vec.at(i);
        sep[0] = ',';
    }
    return os;
}

void main_int(int n, std::mt19937& rng);
void main_double(int n, std::mt19937& rng);

int main(int argc, char *argv[])
{
    std::mt19937 rng{std::random_device{}()};
    int n = 30;
    if (argc > 1) std::stringstream{argv[1]} >> n;
    // main_int(n, rng);
    // main_double(n, rng);

    
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
        for (auto& x : p.map)
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
