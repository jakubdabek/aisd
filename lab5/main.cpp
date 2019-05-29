#include "util.hpp"
#include "MaxFlow.hpp"

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

void main_gplk(const int, const set_t&) {}
