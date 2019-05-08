#include "BinarySearchTree.tpp"
#include "util.tpp"

#include <iostream>
#include <sstream>
#include <numeric>
#include <random>
#include <chrono>
#include <map>
#include <set>
#include <string>

using std::string_literals::operator ""s;


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

template<template<class> class Tree>
void main_tree(const std::set<std::string>&)
{
    Tree<int> bst;
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
                int value;
                if (!(std::cin >> value))
                    return false;
                bst.insert(value);
                return true;
            }
        },
        {
            "delete",
            [&]
            {
                int value;
                if (!(std::cin >> value))
                    return false;
                bst.remove(value);
                return true;
            }
        },
        {
            "search",
            [&]
            {
                int value;
                if (!(std::cin >> value))
                    return false;
                std::cout << bst.search(value) << std::endl;
                return true;
            }
        },
        {
            "inorder",
            [&]
            {
                bst.inorder(std::cout) << std::endl;
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

void main_bst(const std::set<std::string>& options)
{
    main_tree<BinarySearchTree>(options);
}

void main_rbt(const std::set<std::string>&)
{
    // main_tree<RedBlackTree>(options);
}

void main_splay(const std::set<std::string>&)
{
    // main_tree<SplayTree>(options);
}
