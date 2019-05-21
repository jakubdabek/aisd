#pragma once

#include <utility>
#include <functional>
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



template<class T>
class Swapper
{
public:
    void swap(T& a, T& b) noexcept(std::is_nothrow_swappable_v<T>);

    long swaps() const noexcept
    {
        return _swaps;
    }

    void set_verbose(bool verbose, std::ostream& os = std::cerr) noexcept
    {
        _os = maybe_stream(verbose, os);
    }

    void reset() noexcept
    {
        _swaps = 0;
    }

private:
    long _swaps = 0;
    std::reference_wrapper<std::ostream> _os{noop_stream};
};

template<class T>
inline void Swapper<T>::swap(T& a, T& b) noexcept(std::is_nothrow_swappable_v<T>)
{
    _os.get() << "Swapping " << a << " and " << b << std::endl;
    _swaps++;
    using std::swap;
    swap(a, b);
}

template<>
class Swapper<void>
{
public:
    template<class T>
    void swap(T& a, T& b) noexcept(std::is_nothrow_swappable_v<T>);

    long swaps() const noexcept
    {
        return _swaps;
    }

    void set_verbose(bool verbose, std::ostream& os = std::cerr) noexcept
    {
        _os = maybe_stream(verbose, os);
    }

    void reset() noexcept
    {
        _swaps = 0;
    }

private:
    long _swaps = 0;
    std::reference_wrapper<std::ostream> _os{noop_stream};
};

template<class T>
inline void Swapper<void>::swap(T& a, T& b) noexcept(std::is_nothrow_swappable_v<T>)
{
    _os.get() << "Swapping " << a << " and " << b << std::endl;
    _swaps++;
    using std::swap;
    swap(a, b);
}

template<class T>
class Comparer
{
public:
    using cmp_t = std::function<bool(const T&, const T&) noexcept>;

    Comparer() : _cmp{std::less<T>{}} {}
    Comparer(cmp_t f) : _cmp{std::move(f)} {}

    bool compare(const T& a, const T& b) noexcept;

    long comparisons() const noexcept
    {
        return _comparisons;
    }

    void set_verbose(bool verbose, std::ostream& os = std::cerr) noexcept
    {
        _os = maybe_stream(verbose, os);
    }

    void reset() noexcept
    {
        _comparisons = 0;
    }

private:
    long _comparisons = 0;
    std::function<bool(const T&, const T&)> _cmp;
    std::reference_wrapper<std::ostream> _os{noop_stream};
};

template<class T>
inline bool Comparer<T>::compare(const T& a, const T& b) noexcept
{
    _os.get() << "Comparing " << a << " and " << b << std::endl;
    _comparisons++;

    return _cmp(a, b);
}
