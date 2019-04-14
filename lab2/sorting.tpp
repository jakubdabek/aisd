#pragma once

#include "util.tpp"
#include "heap.tpp"


template<class T, class Iter>
void heap_sort(Iter begin, Iter end, Swapper<T>& swp, Comparer<T>& cmp)
{
    std::vector<T> lol{begin, end};
    array_heap heap(std::move(lol), swp, cmp);
    //std::cout << heap << std::endl;
    for (auto end = heap.values.size() - 1; end > 0; end--)
    {
        swp.swap(heap.values[0], heap.values[end]);
        heap.sift_down(0, end);
    }
    swp = heap.swapper();
    cmp = heap.comparer();
    std::move(heap.values.begin(), heap.values.end(), begin);
}

template<class T, class Iter>
void insertion_sort(Iter begin, Iter end, Swapper<T>& swp, Comparer<T>& cmp)
{
    if (begin == end)
        return;
    
    for (auto i = begin; i != end; i++)
    {
        for (auto j = i; j > begin && cmp.compare(*j, *(j - 1)); j--)
        {
            swp.swap(*j, *(j - 1));
        }
    }
}

template<class T, class Iter>
void selection_sort(Iter begin, Iter end, Swapper<T>& swp, Comparer<T>& cmp)
{
    if (begin == end)
        return;
    
    for (auto i = begin; i < (end - 1); i++)
    {
        auto min_index = i;
        for (auto j = i + 1; j < end; j++)
        {
            if (cmp.compare(*j, *min_index))
            {
                min_index = j;
            }
        }
        if (min_index != i)
        {
            swp.swap(*min_index, *i);
        }
    }
}

template<class T, class Iter>
auto qs_partition(Iter begin, Iter end, Iter pivot, Swapper<T>& swp, Comparer<T>& cmp)
{
    if (pivot != (end - 1))
    {
        swp.swap(*(end - 1), *pivot);
        pivot = end - 1;
    }
    auto to_swap = begin;
    for (auto i = begin; i != end; i++)
    {
        if (cmp.compare(*i, *pivot))
        {
            swp.swap(*i, *to_swap);
            to_swap++;
        }
    }
    swp.swap(*to_swap, *pivot);
    return to_swap;
}

template<class T, class Iter>
void quick_sort_(Iter begin, Iter end, Swapper<T>& swp, Comparer<T>& cmp)
{
    if (begin < end)
    {
        // std::cout << "partitoning from " << begin << " to " << end << std::endl;
        auto pivot = qs_partition(begin, end, begin, swp, cmp);
        if (pivot > begin)
            quick_sort_(begin, pivot, swp, cmp);
        quick_sort_(pivot + 1, end, swp, cmp);
    }
}

template<class T, class Iter>
void quick_sort(Iter begin, Iter end, Swapper<T>& swp, Comparer<T>& cmp)
{
    quick_sort_(begin, end, swp, cmp);
}

template<class T, class Iter>
auto median(Iter a, Iter b, Iter c, Comparer<T>& cmp)
{
    if (cmp.compare(*a, *c))
    {
        if(!cmp.compare(*b, *c))
            return c;
        if (cmp.compare(*b, *a))
            return a;
        return b;
    }
    else
    {
        if (!cmp.compare(*b, *a))
            return a;
        if (cmp.compare(*b, *c))
            return c;
        return b;
    }
}

template<class T, class Iter>
void quick_sort_modified_(Iter begin, Iter end, Swapper<T>& swp, Comparer<T>& cmp)
{    
    if (end - begin < 16)
    {
        insertion_sort(begin, end, swp, cmp);
    }
    else
    {
        // std::cout << "partitoning from " << begin << " to " << end << std::endl;
        auto pivot = median(begin, begin + ((end - begin) / 2), end - 1, cmp);
        pivot = qs_partition(begin, end, pivot, swp, cmp);
        if (pivot > begin)
            quick_sort_modified_(begin, pivot, swp, cmp);
        quick_sort_modified_(pivot + 1, end, swp, cmp);
    }
}

template<class T, class Iter>
void quick_sort_modified(Iter begin, Iter end, Swapper<T>& swp, Comparer<T>& cmp)
{
    quick_sort_modified_(begin, end, swp, cmp);
}

template<class T, class Container>
void quick_sort_modified(Container& c, Swapper<T>& swp, Comparer<T>& cmp)
{
    quick_sort_modified_(std::begin(c), std::end(c), swp, cmp);
}

#pragma region old
#if 0
template<class T>
void insertion_sort(std::vector<T>& vec, Swapper<T>& swp, Comparer<T>& cmp)
{
    if (vec.empty())
        return;
    
    for (size_t i = 0; i < vec.size(); i++)
    {
        for (size_t j = i; j > 0 && cmp.compare(vec[j], vec[j - 1]); j--)
        {
            swp.swap(vec[j], vec[j - 1]);
        }
    }
}

template<class T>
void selection_sort(std::vector<T>& vec, Swapper<T>& swp, Comparer<T>& cmp)
{
    if (vec.empty())
        return;
    
    for (size_t i = 0; i < vec.size() - 1; i++)
    {
        size_t min_index = i;
        for (size_t j = i + 1; j < vec.size(); j++)
        {
            if (cmp.compare(vec[j], vec[min_index]))
            {
                min_index = j;
            }
        }
        if (min_index != i)
        {
            swp.swap(vec[min_index], vec[i]);
        }
    }
}

template<class T>
size_t qs_partition(std::vector<T>& vec, Swapper<T>& swp, Comparer<T>& cmp, size_t begin, size_t end, size_t pivot)
{
    if (pivot != end)
    {
        swp.swap(vec[end], vec[pivot]);
        pivot = end;
    }
    size_t to_swap = begin;
    for (size_t i = begin; i < end; i++)
    {
        if (cmp.compare(vec[i], vec[pivot]))
        {
            swp.swap(vec[i], vec[to_swap]);
            to_swap++;
        }
    }
    swp.swap(vec[to_swap], vec[pivot]);
    return to_swap;
}

template<class T>
void quick_sort_(std::vector<T>& vec, Swapper<T>& swp, Comparer<T>& cmp, size_t begin, size_t end)
{
    if (begin < end)
    {
        // std::cout << "partitoning from " << begin << " to " << end << std::endl;
        auto pivot = qs_partition(vec, swp, cmp, begin, end, begin);
        if (pivot > 0)
            quick_sort_(vec, swp, cmp, begin, pivot - 1);
        quick_sort_(vec, swp, cmp, pivot + 1, end);
    }
}

template<class T>
void quick_sort(std::vector<T>& vec, Swapper<T>& swp, Comparer<T>& cmp)
{
    quick_sort_(vec, swp, cmp, 0, vec.size() - 1);
}
#endif
#pragma endregion
