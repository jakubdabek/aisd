#include "heap.tpp"
#include "util.tpp"
#include "sorting.tpp"

#include <iostream>
#include <iomanip>
#include <unordered_map>
#include <functional>
#include <random>
#include <sstream>
#include <chrono>
#include <fstream>

template<class T>
std::ostream& operator<<(std::ostream& os, const std::vector<T>& vec)
{
    char sep[] = "\0 ";
    std::cerr << "[";
    for (const auto& val : vec)
    {
        std::cerr << sep << val;
        sep[0] = ',';
    }
    std::cerr << "]";

    return os;
}

using sort_func_t = void(std::vector<int>::iterator, std::vector<int>::iterator, Swapper<int>&, Comparer<int>&);
const std::unordered_map<std::string, std::function<sort_func_t>> sorts
{
    { "select", static_cast<sort_func_t*>(selection_sort<int>) },
    { "insert", static_cast<sort_func_t*>(insertion_sort<int>) },
    { "quick", static_cast<sort_func_t*>(quick_sort<int>) },
    { "mquick", static_cast<sort_func_t*>(quick_sort_modified<int>) },
    { "heap", static_cast<sort_func_t*>(heap_sort<int>) },
};

int simple_tests();

template<class T, class Sorter>
void test(std::string name, const std::vector<T>& vec, Swapper<T>& swp, Comparer<T>& cmp, const Sorter& srt);

template<class T>
void test(const std::vector<T>& vec, const std::function<sort_func_t>& srt, struct result& res);

void update_mean_variance(const size_t count, double& mean, double& M2, const size_t new_value)
{
    const double delta = new_value - mean;
    mean += delta / (count + 1);
    const double delta2 = new_value - mean;
    M2 += delta * delta2;
}

struct manip
{
    std::function<std::ostream&(std::ostream&)> func;

    friend std::ostream& operator<<(std::ostream& os, const manip& m)
    {
        return m.func(os);
    }
};

struct result
{
    result(size_t data_size) : data_size{data_size} {}
    size_t data_size;
    size_t repetitions = 0;
    double comparisons_mean = 0.0;
    double comparisons_M2 = 0.0;
    double swaps_mean = 0.0;
    double swaps_M2 = 0.0;
    double time_mean = 0.0;
    double time_M2 = 0.0;
    double comparisons_stddev() const { return std::sqrt(comparisons_M2 / repetitions); }
    double swaps_stddev() const { return std::sqrt(swaps_M2 / repetitions); }
    double time_stddev() const { return std::sqrt(time_M2 / repetitions); }

    void update(size_t new_comparisons, size_t new_swaps, std::chrono::duration<double, std::micro> new_time)
    {
        update(new_comparisons, new_swaps, new_time.count());
    }

    void update(size_t new_comparisons, size_t new_swaps, double new_time)
    {
        update_mean_variance(repetitions, comparisons_mean, comparisons_M2, new_comparisons);
        update_mean_variance(repetitions, swaps_mean, swaps_M2, new_swaps);
        update_mean_variance(repetitions, time_mean, time_M2, new_time);
        repetitions++;
    }

    friend std::ostream& operator<<(std::ostream& os, const result& res)
    {
        os << "{n=" << res.data_size << "; "
           << "cmp_mean=" << res.comparisons_mean << "; "
           << "cmp_stddev=" << res.comparisons_stddev() << "; "
           << "swap_mean=" << res.swaps_mean << "; "
           << "swap_stddev=" << res.swaps_stddev() << "; "
           << "time_mean=" << res.time_mean << "µs; "
           << "time_stddev=" << res.time_stddev() << "µs}";
        return os;
    }

    manip csv() const
    {
        return {[&](std::ostream& os) -> std::ostream& {
            os << data_size << ","
               << comparisons_mean << ","
               << comparisons_stddev() << ","
               << swaps_mean << ","
               << swaps_stddev() << ","
               << time_mean << ","
               << time_stddev() << ",";
            return os;
        }};
    }
};


int test_all(std::string filename, std::string number_s)
{
    int number;
    if (!(std::stringstream{number_s} >> number))
        return 1;
    
    std::mt19937 rng{std::random_device{}()};
    std::uniform_int_distribution<int> dist{0, 100'000};

    std::unordered_map<std::string, std::vector<result>> results;
    for (const auto& [sort_type, func] : sorts)
    {
        auto [i, b] = results.try_emplace(sort_type);
        i->second.reserve(number);
    }

    const int iterations = 100;

    for (int i = 1; i <= iterations; i++)
    {
        size_t size = 100ull * i;

        std::unordered_map<std::string, std::reference_wrapper<result>> current_results;
        for (auto& [t, res] : results)
        {
            auto& current_res = res.emplace_back(size);
            current_results.try_emplace(t, current_res);
        }

        std::vector<int> values(size);
        for (int reps = 0; reps < number; reps++)
        {
            for (size_t j = 0; j < size; j++)
            {
                values[j] = dist(rng);
            }
            
            for (const auto& [sort_type, func] : sorts)
            {
                test(values, func, current_results.at(sort_type));
            }
        }
    }

    //std::cerr << "Opening file " << filename << std::endl; 
    std::fstream file{filename, file.trunc | file.out };
    if (!file.is_open())
        return 2;
    
    for (const auto& [sort_type, res] : results)
    {
        //std::cout << "results for " << sort_type << ": " << res << "\n\n" << std::endl;
        std::cerr << sort_type << "," << iterations << "\n";
        for (const auto& r : res)
        {
            std::cerr << std::setprecision(10) << r.csv() << "\n";
        }
    }
    
    return 0;
}

int usage()
{
    std::cout << "Enter proper arguments: main {--type select|insert|heap|quick --asc|--desc} | {--stat filename n}" << std::endl;
    return 1;
}

int main(int argc, char *argv[])
{
    if (argc < 4)
    {
        return simple_tests();
    }
    std::string argv_s[] = { std::string{argv[1]}, std::string{argv[2]}, std::string{argv[3]} };
    if (argv_s[0] == "--stat")
    {
        return test_all(argv_s[1], argv_s[2]);
    }
    if (argv_s[0] == "--type")
    {
        std::string dir_str = argv_s[2];
        bool asc = false;
        if (dir_str == "--asc")
            asc = true;
        else if (dir_str != "--desc")
            return usage();

        if (auto found = sorts.find(argv[2]); found != sorts.end())
        {
            std::cout << "Enter n" << std::endl;
            int n;
            while (!(std::cin >> n)) { std::cout << "Wrong input" << std::endl; }
            std::vector<int> vec;
            vec.reserve(n);
            for (int i = 0; i < n; i++)
            {
                int a;
                if (!(std::cin >> a))
                {
                    std::cout << "Wrong input" << std::endl;
                    return 2;
                }
                vec.push_back(a);
            }
            Swapper<int> swapper;
            Comparer<int>::cmp_t f;
            if (asc)
                f = std::less<int>{};
            else
                f = std::greater<int>{};
            Comparer<int> comparer{f};
            test(found->first, vec, swapper, comparer, found->second);
        }
        else
        {
            return usage();
        }
    }
}

int simple_tests()
{
    // array_heap<int> heap_push_each({}, {});

    std::vector<int> values { 26, 60, 21, 57, 28, 77, 2, 96, 70, 37, 5, 100, 56, 3, 72 };
    // std::cerr << "Inserting single elements into heap:" << std::endl;
    // {
    //     for (auto i : values)
    //     {
    //         std::cerr << "Inserting " << i << std::endl;
    //         heap_push_each.push(i);
    //         std::cerr << "heap:\n" << heap_push_each << std::endl;
    //     }

    //     std::cerr << "Comparisons: " << heap_push_each.comparer().comparisons() 
    //               << " Swaps: " << heap_push_each.swapper().swaps() << std::endl;
    // }

    // std::cerr << "Constructing heap:" << std::endl;
    // {
    //     array_heap<int> heap_constructed(values, {}, {});
    //     std::cerr << "heap:\n" << heap_constructed << std::endl;
    //     std::cerr << "Comparisons: " << heap_constructed.comparer().comparisons() 
    //               << " Swaps: " << heap_constructed.swapper().swaps() << std::endl;
    // }

    std::cerr << "Insertion sort:" << std::endl;
    {
        auto copy = values;
        std::cerr << "Before: " << copy << std::endl;
        Swapper<int> swapper;
        Comparer<int> comparer;
        insertion_sort(copy.begin(), copy.end(), swapper, comparer);
        std::cerr << "Sorted: " << copy << std::endl;
        std::cerr << "Comparisons: " << comparer.comparisons() 
                  << " Swaps: " << swapper.swaps() << std::endl;
    }

    std::cerr << "Selection sort:" << std::endl;
    {
        auto copy = values;
        std::cerr << "Before: " << copy << std::endl;
        Swapper<int> swapper;
        Comparer<int> comparer;
        selection_sort(copy.begin(), copy.end(), swapper, comparer);
        std::cerr << "Sorted: " << copy << std::endl;
        std::cerr << "Comparisons: " << comparer.comparisons() 
                  << " Swaps: " << swapper.swaps() << std::endl;
    }
    std::cerr << "Quick sort:" << std::endl;
    {
        auto copy = values;
        std::cerr << "Before: " << copy << std::endl;
        Swapper<int> swapper;
        Comparer<int> comparer;
        quick_sort(copy.begin(), copy.end(), swapper, comparer);
        std::cerr << "Sorted: " << copy << std::endl;
        std::cerr << "Comparisons: " << comparer.comparisons() 
                  << " Swaps: " << swapper.swaps() << std::endl;
    }
    std::cerr << "Quick sort modified:" << std::endl;
    {
        auto copy = values;
        std::cerr << "Before: " << copy << std::endl;
        Swapper<int> swapper;
        Comparer<int> comparer;
        quick_sort_modified(copy.begin(), copy.end(), swapper, comparer);
        std::cerr << "Sorted: " << copy << std::endl;
        std::cerr << "Comparisons: " << comparer.comparisons() 
                  << " Swaps: " << swapper.swaps() << std::endl;
    }
    std::cerr << "Heap sort:" << std::endl;
    {
        auto copy = values;
        std::cerr << "Before: " << copy << std::endl;
        Swapper<int> swapper;
        Comparer<int> comparer;
        heap_sort(copy.begin(), copy.end(), swapper, comparer);
        std::cerr << "Sorted: " << copy << std::endl;
        std::cerr << "Comparisons: " << comparer.comparisons() 
                  << " Swaps: " << swapper.swaps() << std::endl;
    }

    return 0;
}

template<class T, class Sorter>
void test(std::string name, const std::vector<T>& vec, Swapper<T>& swp, Comparer<T>& cmp, const Sorter& srt)
{
    auto copy = vec;
    swp.set_verbose(true);
    cmp.set_verbose(true);
    std::cerr << "Sorting " << vec  << " with " << name << std::endl;
    auto start_time = std::chrono::high_resolution_clock::now();
    srt(copy.begin(), copy.end(), swp, cmp);
    auto end_time = std::chrono::high_resolution_clock::now();

    std::cerr << "Time taken: " << std::chrono::duration<double>{end_time - start_time}.count() << "s" << std::endl;
    std::cerr << "Comparisons: " << cmp.comparisons() << " Swaps: " << swp.swaps() << std::endl;

    bool sorted = std::is_sorted(copy.begin(), copy.end(), [=](const T& a, const T& b) mutable { return cmp.compare(a, b); });
    std::cout << "Sorted " << copy.size() << " elements: " << copy << (sorted ? " OK": " incorrect") << std::endl;
}

template<class T>
void test(const std::vector<T>& vec, const std::function<sort_func_t>& srt, result& res)
{
    auto copy = vec;
    Swapper<T> swapper;
    Comparer<T> comparer;

    auto start_time = std::chrono::high_resolution_clock::now();
    srt(copy.begin(), copy.end(), swapper, comparer);
    auto end_time = std::chrono::high_resolution_clock::now();

    res.update(comparer.comparisons(), swapper.swaps(), end_time - start_time);
}
