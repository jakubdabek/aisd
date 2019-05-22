#ifndef TYPE
 #define TYPE std::string
#endif

#include "BinarySearchTree.hpp"
#include "RedBlackTree.hpp"
#include "SplayTree.hpp"
#include "util.hpp"

#include <iostream>
#include <fstream>
#include <sstream>
#include <numeric>
#include <random>
#include <chrono>
#include <map>
#include <set>
#include <string>

using std::string_literals::operator ""s;


int modification_count = 0;

void main_bst(const std::set<std::string>&);
void main_rbt(const std::set<std::string>&);
void main_splay(const std::set<std::string>&);

int main(int argc, char *argv[])
{
    std::map<std::string, std::function<void(const std::set<std::string>&)>> types
    {
        {
            "bst",
            main_bst,
        },
        {
            "rbt",
            main_rbt,
        },
        {
            "splay",
            main_splay,
        },
    };

    if (argc < 3 || argv[1] != "--type"s)
        return 1;
    
    auto type = argv[2];

    std::set<std::string> options;
    for (int i = 3; i < argc; i++)
    {
        options.emplace(argv[i]);
    }

    if (auto c = types.find(type); c != types.end())
    {
        c->second(options);
    }
    else
    {
        std::cout << "Supported options: bst|rbt|splay\n";
        return 1;
    }
    
}

template<template<class> class Tree, class T = TYPE>
void main_tree(const std::set<std::string>&)
{
    Tree<T> bst;
    int ops;
    std::cout << "Enter number of operations\n";
    if (!(std::cin >> ops))
    {
        std::cout << "Wrong input\n";
        return;
    }

    const auto load_and = [&](auto&& F)
    {
        std::string filename;
        if (!(std::cin >> filename))
            return false;
        std::ifstream file{filename};
        if (!file.is_open())
        {
            std::cout << "Couldn't open file\n";
            return true;
        }
        T value;
        // int counter = 0;
        while (file >> value)
        {
            std::invoke(F, value);
            // if (++counter % 1000 == 0)
                // std::cout << "Loaded " << counter << " values\n";
        }
        return true;
    };

    int current_size = 0;
    int max_size = current_size;
    int insert_count = 0;
    const auto do_insert = [&](const T& value)
    {
        bool inserted = bst.insert(value);
        // if (!bst.check())
        //     std::cerr << "ERROR" << std::endl;
        if (inserted)
        {
            current_size++;
            if (current_size > max_size)
                max_size = current_size;
        }
        insert_count++;

        return inserted;
    };

    int search_count = 0;
    const auto do_search = [&](const T& value)
    {
        bool found = bst.search(value);
        // if (!bst.check())
        //     std::cerr << "ERROR" << std::endl;
        search_count++;

        return found;
    };

    int delete_count = 0;
    const auto do_delete = [&](const T& value)
    {
        bool deleted = bst.remove(value);
        // if (!bst.check())
        //     std::cerr << "ERROR" << std::endl;
        if (deleted)
        {
            current_size--;
            if (current_size < 0)
                throw std::logic_error("size negative");
        }
        delete_count++;

        return deleted;
    };

    int inorder_count = 0;
    const auto do_inorder = [&](std::ostream& os)->std::ostream&
    {
        inorder_count++;
        return bst.inorder(os);
    };

    std::unordered_map<std::string, std::function<bool()>> operations
    {
        {
            "insert",
            [&]
            {
                T value;
                if (!(std::cin >> value))
                    return false;
                do_insert(value);
                return true;
            }
        },
        {
            "delete",
            [&]
            {
                T value;
                if (!(std::cin >> value))
                    return false;
                do_delete(value);
                return true;
            }
        },
        {
            "search",
            [&]
            {
                T value;
                if (!(std::cin >> value))
                    return false;
                std::cout << do_search(value) << std::endl;
                return true;
            }
        },
        {
            "load",
            [&]
            {
                return load_and(do_insert);
            }
        },
        {
            "load_search",
            [&]
            {
                // return load_and([&](const T& value)
                // {
                //     std::cout << value << " " << do_search(value) << std::endl;
                // });
                return load_and(do_search);
            }
        },
        {
            "load_delete",
            [&]
            {
                return load_and(do_delete);
            }
        },
        {
            "clear",
            [&]
            {
                bst.clear();
                return true;
            }
        },
        {
            "size",
            [&]
            {
                std::cout << current_size << std::endl;
                return true;
            }
        },
        {
            "inorder",
            [&]
            {
                do_inorder(std::cout) << std::endl;
                return true;
            }
        },
        {
            "levelorder",
            [&]
            {
                bst.levelorder(std::cout) << std::endl;
                return true;
            }
        },
        {
            "help",
            [&]
            {
                std::cout << "Enter operation:"
                          << " insert x |"
                          << " delete x |"
                          << " search x |"
                          << " inorder |"
                          << " load filename |"
                          << " load_search filename |"
                          << " load_delete filename |"
                          << " clear |"
                          << " help\n";
                return true;
            }
        },
    };

    operations.at("help")();

    std::vector<std::tuple<std::string, double, double, double>> partial_stats;
    partial_stats.reserve(20);
    long last_comparison_count = 0;
    long last_modification_count = 0;

    auto start_time = std::chrono::high_resolution_clock::now();
    auto last_time = start_time;

    for (int i = 0; i < ops || ops < 0; i++)
    {
        std::string op;
        
        std::cin.clear();
        if (!(std::cin >> op))
            break;

        if (auto o = operations.find(op); o != operations.end())
        {
            if (!o->second())
            {
                std::cout << "Wrong arguments\n";
                std::cin.clear();
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            }
            else
            {
                const auto current_time = std::chrono::high_resolution_clock::now();
                const auto current_comparison_count = bst.comparer().comparisons();
                const auto current_modification_count = modification_count;
                partial_stats.emplace_back(
                    std::move(op),
                    std::chrono::duration_cast<std::chrono::duration<double, std::milli>>(current_time - last_time).count(),
                    current_comparison_count - last_comparison_count,
                    current_modification_count - last_modification_count
                );

                last_time = current_time;
                last_comparison_count = current_comparison_count;
                last_modification_count = current_modification_count;
            }
        }
        else
        {
            std::cout << "Wrong operation\n";
        }
    }

    auto end_time = std::chrono::high_resolution_clock::now();
    std::cout << "Time: " << std::chrono::duration_cast<std::chrono::duration<double, std::milli>>(end_time - start_time).count() << "ms" << std::endl;

    std::cout << "Number of insertions: " << insert_count << std::endl;
    std::cout << "Number of searches: " << search_count << std::endl;
    std::cout << "Number of deletions: " << delete_count << std::endl;
    std::cout << "Number of inorders: " << inorder_count << std::endl;
    std::cout << "Number of comparisons: " << bst.comparer().comparisons() << std::endl;
    std::cout << "Number of modifications: " << modification_count << std::endl;
    std::cout << "Partial results: " << partial_stats << std::endl;
    for (auto& s : partial_stats)
    {
        std::get<1>(s) /= insert_count;
        std::get<1>(s) *= 1000.0;
        std::get<2>(s) /= insert_count;
        std::get<3>(s) /= insert_count;
    }
    std::cout << "Partial results per one op: " << partial_stats << std::endl;
    std::cout << "Max size: " << max_size << std::endl;
    std::cout << "Current size: " << current_size << std::endl;
}

void main_bst(const std::set<std::string>& options)
{
    main_tree<BinarySearchTree>(options);
}

void main_rbt(const std::set<std::string>& options)
{
    main_tree<RedBlackTree>(options);
}

void main_splay(const std::set<std::string>& options)
{
    main_tree<SplayTree>(options);
}
