#pragma once

#include <utility>
#include <functional>
#include <optional>
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

template<class T, class U, class V>
std::ostream& operator<<(std::ostream& os, const std::tuple<T, U, V> t)
{
    return os << "(" << std::get<0>(t) << ", " << std::get<1>(t) << ", " << std::get<2>(t) << ")";
}

template<class T1, class T2, class T3, class T4>
std::ostream& operator<<(std::ostream& os, const std::tuple<T1, T2, T3, T4> t)
{
    return os << "(" << std::get<0>(t) << ", " << std::get<1>(t) << ", " << std::get<2>(t) << ", " << std::get<3>(t) << ")";
}

template<class T>
std::ostream& operator<<(std::ostream& os, const std::optional<T> opt)
{
    if (opt.has_value())
        os << *opt;
    else
        os << "()";
    
    return os;
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


//using options_t = std::set<std::string, std::less<>>;
using options_t = std::vector<std::string>;
