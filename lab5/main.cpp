#include "util.hpp"
#include "MaxFlow.hpp"
#include "BipartiteMatching.hpp"

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


int main(int argc, char *argv[])
{
    if (argc < 2 || argv[1] != "--size"s)
        return 1;
    
    int size;
    if (!(std::stringstream{argv[2]} >> size))
        return 1;
    
    int lastArg = 3;
    int degree = -1;
    if (argc >= 5 && argv[3] == "--degree"s)
    {
        lastArg = 5;
        if (!(std::stringstream{argv[2]} >> degree))
            return 1;
    }

    options_t options;
    for (int i = lastArg; i < argc; i++)
    {
        options.emplace_back(argv[i]);
    }

    if (degree > 0)
    {
        main_match(size, degree, options);
    }
    else
    {
        main_flow(size, options);
    }
    
}
