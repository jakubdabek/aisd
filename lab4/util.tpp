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
inline void Swapper<T>::swap(T& a, T& b)
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
inline bool Comparer<T>::compare(const T& a, const T& b)
{
    if (_verbose)
        std::cerr << "Comparing " << a << " and " << b << std::endl;
    _comparisons++;

    return cmp(a, b);
}
#include <utility>
#include <iostream>

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

    // templater bool is_iterable_v = is_instantiation<std::vector, T>::value || is_instantiation<std::deque, T>::value;
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

inline struct noop_stream_t : std::ostream
{
    template<class T>
    std::ostream& operator<<(const T&) { return *this; }
} noop_stream;

static std::ostream& maybe_stream(bool verbose, std::ostream& verbose_stream = std::cout)
{
    if (verbose)
        return verbose_stream;
    else
        return noop_stream;
}

constexpr bool is_power_of_2(size_t value)
{
    return (value & (value - 1)) == 0;
}
