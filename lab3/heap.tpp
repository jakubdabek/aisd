#pragma once

#include <memory>
#include <vector>
#include <iostream>
#include <optional>
#include <functional>
#include <string>



template<class T, class ComparerT = std::less<T>>
class iterator_heap
{
public:
    template<class Iter>
    static void heapify(Iter begin, Iter end, const ComparerT& cmp = {});

    template<class Container>
    static void push(Container& c, const T& value, const ComparerT& cmp = {})
    {
        c.push_back(value);
        using std::begin;
        using std::end;
        using std::prev;
        after_push(begin(c), end(c), prev(end(c)), cmp);
    }

    template<class Iter>
    static void after_push(Iter begin, Iter end, Iter elem, const ComparerT& cmp = {})
    {
        sift_up(begin, end, elem, cmp);
    }

    template<class Container>
    static T pop(Container& c, const ComparerT& cmp = {})
    {
        if (c.empty())
            throw std::out_of_range{};

        using std::begin;
        using std::end;
        if (c.size() == 1)
        {
            auto val = std::move(*begin(c));
            c.pop_back();
            return val;
        }

        using std::prev;
        auto first = begin(c);
        auto last = prev(end(c));
        my_swap(first, last);
        auto val = std::move(*last);
        c.pop_back();
        after_pop(begin(c), end(c), cmp);

        return val;
    }

    template<class Iter>
    static void sift_down(Iter begin, Iter end, Iter elem, const ComparerT& cmp = {})
    {
        auto has_children = [&](Iter i)
        {
            auto l = left_child(begin, end, i);
            return l && (*l < end); 
        };

        while (has_children(elem))
        {
            auto to_swap = elem;
            Iter left = *left_child(begin, end, elem);
            std::optional<Iter> right = right_child(begin, end, elem);

            if (cmp(*elem, *left))
            {
                to_swap = left;
            }

            if (right && cmp(*to_swap, **right))
            {
                to_swap = *right;
            }

            if (to_swap == elem)
                return;

            my_swap(elem, to_swap);
            elem = to_swap;            
        }
    }

    template<class Iter>
    static void sift_up(Iter begin, Iter end, Iter elem, const ComparerT& cmp = {})
    {
        auto step = [&]() { return parent(begin, end, elem); };
        for (auto par = step(); par > begin && cmp(*elem, **par); par = step())
        {
            my_swap(elem, *par);
            elem = *par;
        }
    }

    template<class Iter>
    static std::optional<Iter> parent(Iter begin, Iter end, Iter elem)
    {
        return manipulate_iter_by_distance(begin, end, elem, [](auto dist){ return (dist - 1) / 2; });
    }
    template<class Iter>
    static std::optional<Iter> left_child(Iter begin, Iter end, Iter elem)
    {
        return manipulate_iter_by_distance(begin, end, elem, [](auto dist){ return dist * 2 + 1; });
    }
    template<class Iter>
    static std::optional<Iter> right_child(Iter begin, Iter end, Iter elem)
    {
        return manipulate_iter_by_distance(begin, end, elem, [](auto dist){ return dist * 2 + 2; });
    }

private:
    template<class Iter>
    static void my_swap(Iter a, Iter b)
    {
        using std::swap;
        swap(*a, *b);
    }

    template<class Iter>
    static void after_pop(Iter begin, Iter end, const ComparerT& cmp = {})
    {
        sift_down(begin, end, begin, cmp);
    }

    template<class Iter, class F>
    std::optional<Iter> manipulate_iter_by_distance(Iter begin, Iter end, Iter elem, F transform)
    {
        using std::distance;
        auto dist = distance(begin, elem);
        auto index = transform(dist);
        if (index < 0 || index >= distance(begin, end))
            return std::nullopt;
        using std::advance;
        advance(begin);
        return begin;
    }
};

template<class T>
struct IndexSwapper
{
    template<class Container>
    void operator()(Container& c, size_t a, size_t b)
    {
        using std::swap;
        swap(c[a], c[b]);
    }
};

template<class T, class ComparerT = std::less<T>, class SwapperT = IndexSwapper<T>>
class heap_util
{
public:
    template<class Container>
    static void heapify(Container& c, const ComparerT& cmp = {}, const SwapperT& swp = {})
    {
        for (std::make_signed_t<size_t> i = parent(c.size() - 1); i >= 0; i--)
        {
            sift_down(c, i, c.size(), cmp, swp);
        }
    }

    template<class Container>
    static void push(Container& c, const T& value, const ComparerT& cmp = {}, const SwapperT& swp = {})
    {
        c.push_back(value);
        after_push(c, cmp, swp);
    }

    template<class Container>
    static void after_push(Container& c, const ComparerT& cmp = {}, const SwapperT& swp = {})
    {
        sift_up(c, c.size() - 1, 0, cmp, swp);
    }

    template<class Container>
    static std::optional<T> pop(Container& c, const ComparerT& cmp = {}, const SwapperT& swp = {})
    {
        if (c.empty())
            return std::nullopt;

        if (c.size() == 1)
        {
            auto val = std::move(c[0]);
            c.pop_back();
            return val;
        }

        swp(c, 0, c.size() - 1);
        auto val = std::move(c[c.size() - 1]);
        c.pop_back();
        after_pop(c, cmp, swp);

        return val;
    }

    template<class Container>
    static void sift_down(Container& c, size_t index, size_t end, const ComparerT& cmp = {}, const SwapperT& swp = {})
    {
        auto has_children = [&](size_t i) { return left_child(i) < end; };
        while (has_children(index))
        {
            auto to_swap = index;
            auto left = left_child(index);
            auto right = right_child(index);

            if (cmp(c[index], c[left]))
            {
                to_swap = left;
            }

            if (right < end && cmp(c[to_swap], c[right]))
            {
                to_swap = right;
            }

            if (to_swap == index)
            {
                return;
            }
            else
            {
                swp(c, index, to_swap);
                index = to_swap;
            }
        }
    }

    template<class Container>
    static void sift_up(Container& c, size_t index, size_t top = 0, const ComparerT& cmp = {}, const SwapperT& swp = {})
    {
        for (auto par = parent(index);
            index > top && cmp(c[par], c[index]);
            par = parent(index)
        ) {
            swp(c, par, index);
            index = par;
        }
    }

    static size_t parent(size_t index)
    {
        return (index - 1) / 2;
    }

    static size_t left_child(size_t index)
    {
        return 2 * index + 1;
    }

    static size_t right_child(size_t index)
    {
        return 2 * index + 2;
    }


private:
    template<class Container>
    static void after_pop(Container& c, const ComparerT& cmp = {}, const SwapperT& swp = {})
    {
        sift_down(c, 0, c.size(), cmp, swp);
    }
};


template<class T, class ComparerT>
class array_heap;

template<class T, class ComparerT>
std::ostream& operator<<(std::ostream& os, const array_heap<T, ComparerT>& val);

template<class T, class ComparerT = std::less<T>>
class array_heap
{
public:
    array_heap();
    array_heap(ComparerT cmp);
    array_heap(const std::vector<T>& vec);
    array_heap(const std::vector<T>& vec, ComparerT cmp);
    array_heap(std::vector<T>&& vec);
    array_heap(std::vector<T>&& vec, ComparerT cmp);

    void push(const T& value);
    void push(T&& value);
    std::optional<T> top() const;
    std::optional<T> pop();
    // const T& top() const &;
    // T top() const &&;
    // T pop();
    size_t size() { return values.size(); }

    ComparerT& comparer() { return cmp; }
    const ComparerT& comparer() const { return cmp; }

    // template<class ComparerU>
    // friend std::ostream& operator<<<T, ComparerU>(std::ostream& os, const array_heap<T, ComparerU>& val);

    friend std::ostream& operator<<<T, ComparerT>(std::ostream& os, const array_heap& val);

public:
    static void heapify(std::vector<T>& vec);
    static void heapify(std::vector<T>& vec, ComparerT& cmp);
    template<class Iter>
    static void heapify(Iter begin, Iter end);
    template<class Iter>
    static void heapify(Iter begin, Iter end, ComparerT& cmp);
    template<class U, class ComparerU, class Iter>
    friend void heap_sort(Iter begin, Iter end);
    template<class U, class ComparerU, class Iter>
    friend void heap_sort(Iter begin, Iter end, ComparerU& cmp);

private:
    ComparerT cmp;
    std::vector<T> values;

private:
    size_t parent(size_t index) const noexcept;
    size_t left_child(size_t index) const noexcept;
    size_t right_child(size_t index) const noexcept;
    void my_swap(T& a, T& b)
    {
        using std::swap;
        swap(a, b);
    }
    void sift_up(size_t index, size_t top = 0)
        noexcept(noexcept(std::declval<ComparerT&>().compare(std::declval<const T&>(), std::declval<const T&>())));
    void sift_down(size_t index, size_t end);
    void heapify();
};

constexpr bool is_power_of_2(size_t value)
{
    return (value & (value - 1)) == 0;
}

constexpr size_t log2s(size_t value)
{
    size_t i = 0;
    for ( ; value >= 2; i++)
    {
        value /= 2;
    }
    return i;
}

// template<class T, class ComparerT, class SFINAE = std::void_t<std::enable_if_t<std::is_integral_v<T>>>>
// std::ostream& operator<<(std::ostream& os, const array_heap<T, ComparerT>& val)
// {
    
// }

template<class T, class ComparerT>
std::ostream& operator<<(std::ostream& os, const array_heap<T, ComparerT>& val)
{
    if constexpr (std::is_integral_v<T> || std::is_floating_point_v<T>)
    {
        constexpr auto width = std::is_integral_v<T> ? 3 : 6;
        constexpr auto precision = 4;

        auto height = log2s(val.values.size());
        int indent_magnitude = 1 << height;
        size_t offset = 0;
        size_t next_offset_delta = 1;
        for (size_t level = 0; level <= height; level++)
        {
            printf("%*s", (indent_magnitude - 1) * width, "");
            if constexpr (static_cast<void>(precision), std::is_integral_v<T>)
                printf("%*ld", width, static_cast<long>(val.values.at(offset)));
            else
                printf("%*.*lf", width, precision, static_cast<double>(val.values.at(offset)));

            bool left_align = true;
            for (size_t elem = offset + 1; elem < offset + next_offset_delta && elem < val.values.size(); elem++)
            {
                printf("%*s", (2 * indent_magnitude - 1) * width, "");
                if constexpr (std::is_integral_v<T>)
                    printf("%*ld", (left_align ? -1 : 1) * width, static_cast<long>(val.values.at(elem)));
                else
                    printf("%*.*lf", (left_align ? -1 : 1) * width, precision, static_cast<double>(val.values.at(offset)));
                left_align = !left_align;
            }
            printf("\n");
            offset += next_offset_delta;
            next_offset_delta *= 2;
            indent_magnitude /= 2;
        }
        return os;
    }
    else
    {
        char sep[] = "\0 ";
        for (size_t i = 0; i < val.values.size(); i++)
        {
            os << sep << val.values.at(i);
            if (is_power_of_2(i + 2))
            {
                os << "\n";
                sep[0] = '\0';
            }
            else
            {
                sep[0] = ',';
            }
        }
        return os;
    }
}

template<class T, class ComparerT>
inline array_heap<T, ComparerT>::array_heap() 
    : cmp{}
{
}

template<class T, class ComparerT>
inline array_heap<T, ComparerT>::array_heap(ComparerT cmp) 
    : cmp{std::move(cmp)}
{
}

template<class T, class ComparerT>
inline array_heap<T, ComparerT>::array_heap(const std::vector<T>& vec) 
    : cmp{}, values{vec}
{
    this->heapify();
}

template<class T, class ComparerT>
inline array_heap<T, ComparerT>::array_heap(const std::vector<T>& vec, ComparerT cmp) 
    : cmp{std::move(cmp)}, values{vec}
{
    this->heapify();
}

template<class T, class ComparerT>
inline array_heap<T, ComparerT>::array_heap(std::vector<T>&& vec) 
    : cmp{}, values{std::move(vec)}
{
    this->heapify();
}

template<class T, class ComparerT>
inline array_heap<T, ComparerT>::array_heap(std::vector<T>&& vec, ComparerT cmp) 
    : cmp{std::move(cmp)}, values{std::move(vec)}
{
    this->heapify();
}

template<class T, class ComparerT>
inline void array_heap<T, ComparerT>::push(const T& value)
{
    values.push_back(value);
    sift_up(values.size() - 1);
}

template<class T, class ComparerT>
inline void array_heap<T, ComparerT>::push(T&& value)
{
    values.emplace_back(std::move(value));
    sift_up(values.size() - 1);
}

template<class T, class ComparerT>
inline std::optional<T> array_heap<T, ComparerT>::top() const
{
    return values.empty() ? std::nullopt : values.at(0);
}

template<class T, class ComparerT>
inline std::optional<T> array_heap<T, ComparerT>::pop()
{
    if (values.empty())
        return std::nullopt;

    if (values.size() > 1)
        my_swap(values.at(0), values.at(values.size() - 1));

    auto val = std::move(values.at(values.size() - 1));
    values.pop_back();
    sift_down(0, values.size());
    return val;
}

template<class T, class ComparerT>
inline size_t array_heap<T, ComparerT>::parent(size_t index) const noexcept
{
    return (index - 1) / 2;
}

template<class T, class ComparerT>
inline size_t array_heap<T, ComparerT>::left_child(size_t index) const noexcept
{
    return 2 * index + 1;
}

template<class T, class ComparerT>
inline size_t array_heap<T, ComparerT>::right_child(size_t index) const noexcept
{
    return 2 * index + 2;
}

template<class T, class ComparerT>
inline void array_heap<T, ComparerT>::sift_up(size_t index, size_t top)
    noexcept(noexcept(std::declval<ComparerT&>().compare(std::declval<const T&>(), std::declval<const T&>())))
{
    for (auto par = parent(index);
         index > top && cmp(values[par], values[index]);
         par = parent(index)
    ) {
        std::cout << "sift_up(" << index << ", " << top << "):" << "\n";
        std::cout << *this << "\n";
        std::cout << "Swapping " << values[par] << " and " << values[index] << "\n";
        my_swap(values[par], values[index]);
        index = par;
    }
}

template<class T, class ComparerT>
inline void array_heap<T, ComparerT>::sift_down(size_t index, size_t end)
{
    auto has_children = [&](size_t i) { return left_child(i) < end; };
    while (has_children(index))
    {
        auto to_swap = index;
        auto left = left_child(index);
        auto right = right_child(index);

        if (cmp(values[index], values[left]))
        {
            to_swap = left;
        }

        if (right < end && cmp(values[to_swap], values[right]))
        {
            to_swap = right;
        }

        if (to_swap == index)
        {
            return;
        }
        else
        {
            std::cout << "sift_down(" << index << ", " << end << "):\n";
            std::cout << *this << "\n";
            std::cout << "Swapping " << values[index] << " and " << values[to_swap] << "\n\n";
            my_swap(values[index], values[to_swap]);
            index = to_swap;
        }
    }
}

template<class T, class ComparerT>
inline void array_heap<T, ComparerT>::heapify()
{
    for (std::make_signed_t<size_t> i = parent(values.size() - 1); i >= 0; i--)
    {
        sift_down(i, values.size());
    }
}

template<class T, class ComparerT>
inline void array_heap<T, ComparerT>::heapify(std::vector<T>& vec)
{
    ComparerT cmp;
    heapify(vec, cmp);
}

template<class T, class ComparerT>
inline void array_heap<T, ComparerT>::heapify(std::vector<T>& vec, ComparerT& cmp)
{
    array_heap heap(std::move(vec), cmp);
    vec = std::move(heap.values);
}

template<class T, class ComparerT>
template<class Iter>
inline void array_heap<T, ComparerT>::heapify(Iter begin, Iter end)
{
    ComparerT cmp;
    heapify(begin, end, cmp);
}

template<class T, class ComparerT>
template<class Iter>
inline void array_heap<T, ComparerT>::heapify(Iter begin, Iter end, ComparerT& cmp)
{
    std::vector<T> lol{begin, end};
    array_heap heap(std::move(lol), cmp);
    std::move(heap.values.begin(), heap.values.end(), begin);
}
