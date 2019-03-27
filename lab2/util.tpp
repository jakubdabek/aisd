#pragma once

#include <utility>
#include <functional>
#include <iostream>

template<class T>
class Swapper
{
public:
    void swap(T& a, T& b);

    int swaps() const noexcept
    {
        return _swaps;
    }

    void set_verbose(bool verbose) noexcept
    {
        _verbose = verbose;
    }

    void reset() noexcept
    {
        _swaps = 0;
    }

private:
    int _swaps = 0;
    bool _verbose = false;
};

template<class T>
void Swapper<T>::swap(T& a, T& b)
{
    if (_verbose)
        std::cerr << "Swapping " << a << " and " << b << std::endl;
    _swaps++;
    using std::swap;
    swap(a, b);
}

template<class T>
class Comparer
{
public:
    using cmp_t = std::function<bool(const T&, const T&)>;

    Comparer() : cmp{std::less<T>{}} {}
    Comparer(cmp_t f) : cmp{std::move(f)} {}

    bool compare(const T& a, const T& b);

    int comparisons() const noexcept
    {
        return _comparisons;
    }

    void set_verbose(bool verbose) noexcept
    {
        _verbose = verbose;
    }

    void reset() noexcept
    {
        _comparisons = 0;
    }

private:
    int _comparisons = 0;
    bool _verbose = false;
    std::function<bool(const T&, const T&)> cmp;
};

template<class T>
bool Comparer<T>::compare(const T& a, const T& b)
{
    if (_verbose)
        std::cerr << "Comparing " << a << " and " << b << std::endl;
    _comparisons++;

    return cmp(a, b);
}
