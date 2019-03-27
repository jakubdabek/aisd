#pragma once

#include "util.tpp"

#include <memory>
#include <vector>
#include <iostream>
#include <string>


template<class T, class Comparer = std::less<T>>
class tree_heap
{
public:
    void push(const T& value);
    void push(T&& value);
    T pop();

private:
    struct node
    {
        T value;
        std::shared_ptr<node> left, right;
        std::weak_ptr<node> parent;
    };
    
    std::shared_ptr<node> root;
};

template<class T, class Comparer>
void tree_heap<T, Comparer>::push(const T& value)
{

}


/*
*******************************************************************
*******************************************************************
*/

template<class T>
class array_heap;

template<class T>
std::ostream& operator<<(std::ostream& os, const array_heap<T>& val);

template<class T>
class array_heap
{
public:
    array_heap();
    array_heap(Swapper<T> swp, Comparer<T> cmp);
    array_heap(const std::vector<T>& vec);
    array_heap(const std::vector<T>& vec, Swapper<T> swp, Comparer<T> cmp);
    array_heap(std::vector<T>&& vec);
    array_heap(std::vector<T>&& vec, Swapper<T> swp, Comparer<T> cmp);

    void push(const T& value);
    void push(T&& value);
    T pop();

    Swapper<T>& swapper() { return swp; }
    const Swapper<T>& swapper() const { return swp; }
    Comparer<T>& comparer() { return cmp; }
    const Comparer<T>& comparer() const { return cmp; }

    friend std::ostream& operator<<<T>(std::ostream& os, const array_heap& val);

public:
    static void heapify(std::vector<T>& vec, Swapper<T>& swp, Comparer<T>& cmp);
    template<class Iter>
    static void heapify(Iter begin, Iter end, Swapper<T>& swp, Comparer<T>& cmp);
    template<class U, class Iter>
    friend void heap_sort(Iter begin, Iter end, Swapper<U>& swp, Comparer<U>& cmp);

private:
    Comparer<T> cmp;
    Swapper<T> swp;
    std::vector<T> values;

private:
    size_t parent(size_t index) const noexcept;
    size_t left_child(size_t index) const noexcept;
    size_t right_child(size_t index) const noexcept;
    void sift_up(size_t index, size_t top = 0)
        noexcept(noexcept(std::declval<Swapper<T>&>().swap(std::declval<T&>(), std::declval<T&>())) && 
                 noexcept(std::declval<Comparer<T>&>().compare(std::declval<const T&>(), std::declval<const T&>())));
    void sift_down(size_t index, size_t end);
    void heapify();
};

bool is_power_of_2(size_t value)
{
    return (value & (value - 1)) == 0;
}

template<class T>
std::ostream& operator<<(std::ostream& os, const array_heap<T>& val)
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

template<class T>
array_heap<T>::array_heap() 
    : cmp{}, swp{}
{
}

template<class T>
array_heap<T>::array_heap(Swapper<T> swp, Comparer<T> cmp) 
    : cmp{std::move(cmp)}, swp{std::move(swp)}
{
}

template<class T>
array_heap<T>::array_heap(const std::vector<T>& vec) 
    : cmp{}, swp{}, values{vec}
{
    this->heapify();
}

template<class T>
array_heap<T>::array_heap(const std::vector<T>& vec, Swapper<T> swp, Comparer<T> cmp) 
    : cmp{std::move(cmp)}, swp{std::move(swp)}, values{vec}
{
    this->heapify();
}

template<class T>
array_heap<T>::array_heap(std::vector<T>&& vec) 
    : cmp{}, swp{}, values{std::move(vec)}
{
    this->heapify();
}

template<class T>
array_heap<T>::array_heap(std::vector<T>&& vec, Swapper<T> swp, Comparer<T> cmp) 
    : cmp{std::move(cmp)}, swp{std::move(swp)}, values{std::move(vec)}
{
    this->heapify();
}

template<class T>
inline void array_heap<T>::push(const T& value)
{
    values.push_back(value);
    sift_up(values.size() - 1);
}

template<class T>
inline void array_heap<T>::push(T&& value)
{
    values.emplace_back(std::move(value));
    sift_up(values.size() - 1);
}

template<class T>
inline size_t array_heap<T>::parent(size_t index) const noexcept
{
    return (index - 1) / 2;
}

template<class T>
inline size_t array_heap<T>::left_child(size_t index) const noexcept
{
    return 2 * index + 1;
}

template<class T>
inline size_t array_heap<T>::right_child(size_t index) const noexcept
{
    return 2 * index + 2;
}

template<class T>
inline void array_heap<T>::sift_up(size_t index, size_t top)
    noexcept(noexcept(std::declval<Swapper<T>&>().swap(std::declval<T&>(), std::declval<T&>())) && 
             noexcept(std::declval<Comparer<T>&>().compare(std::declval<const T&>(), std::declval<const T&>())))
{
    for (auto par = parent(index);
         index > top && cmp.compare(values[par], values[index]);
         par = parent(index)
    ) {
        swp.swap(values[par], values[index]);
        index = par;
    }
}

template<class T>
inline void array_heap<T>::sift_down(size_t index, size_t end)
{
    auto has_children = [&](size_t i) { return left_child(index) < end; };
    while (has_children(index))
    {
        auto to_swap = index;
        auto left = left_child(index);
        auto right = right_child(index);

        if (cmp.compare(values[index], values[left]))
        {
            to_swap = left;
        }

        if (right < end && cmp.compare(values[to_swap], values[right]))
        {
            to_swap = right;
        }

        if (to_swap == index)
        {
            return;
        }
        else
        {
            swp.swap(values[index], values[to_swap]);
            index = to_swap;
        }
    }
}

template<class T>
inline void array_heap<T>::heapify()
{
    for (std::make_signed_t<size_t> i = parent(values.size() - 1); i >= 0; i--)
    {
        sift_down(i, values.size());
    }
}

template<class T>
inline void array_heap<T>::heapify(std::vector<T>& vec, Swapper<T>& swp, Comparer<T>& cmp)
{
    array_heap heap(std::move(vec), swp, cmp);
    vec = std::move(heap.values);
}

template<class T>
template<class Iter>
inline void array_heap<T>::heapify(Iter begin, Iter end, Swapper<T>& swp, Comparer<T>& cmp)
{
    std::vector<T> lol{begin, end};
    array_heap heap(std::move(lol), swp, cmp);
    std::move(heap.values.begin(), heap.values.end(), begin);
}
