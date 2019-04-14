#include "heap.tpp"

#include <map>

template<class T>
class priority_queue
{
private:
    using value_type = std::tuple<int, T, typename std::multimap<T, size_t>::iterator>;

    static std::ostream& print(std::ostream& os, const value_type& v)    
    {
        return os << "(" << std::get<1>(v) << "," << std::get<0>(v) << ")";
    }

    struct Printer
    {
        std::vector<value_type>& v;

        std::ostream& print(std::ostream& os) const
        {
            const auto& values = v;
            if constexpr (false)
            {
                constexpr auto width = std::is_integral_v<T> ? 3 : 6;
                constexpr auto precision = 4;

                auto height = log2s(values.size());
                int indent_magnitude = 1 << height;
                size_t offset = 0;
                size_t next_offset_delta = 1;
                for (size_t level = 0; level <= height; level++)
                {
                    printf("%*s", (indent_magnitude - 1) * width, "");
                    if constexpr (static_cast<void>(precision), std::is_integral_v<T>)
                        printf("%*ld", width, static_cast<long>(values.at(offset)));
                    else
                        printf("%*.*lf", width, precision, static_cast<double>(values.at(offset)));

                    bool left_align = true;
                    for (size_t elem = offset + 1; elem < offset + next_offset_delta && elem < values.size(); elem++)
                    {
                        printf("%*s", (2 * indent_magnitude - 1) * width, "");
                        if constexpr (std::is_integral_v<T>)
                            printf("%*ld", (left_align ? -1 : 1) * width, static_cast<long>(values.at(elem)));
                        else
                            printf("%*.*lf", (left_align ? -1 : 1) * width, precision, static_cast<double>(values.at(offset)));
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
                for (size_t i = 0; i < values.size(); i++)
                {
                    os << sep;
                    priority_queue::print(os, values.at(i));
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
    };
public:
    bool empty() const { return vec.empty(); }
    void insert(const T& value, int priority)
    {
        auto map_elem = map.emplace(value, vec.size());
        h::push(vec, value_type{priority, value, map_elem});
    }
    std::optional<T> top() const { return vec.empty() ? std::nullopt : vec[0]; }
    std::optional<T> pop()
    {
        auto ret = h::pop(vec);
        if (!ret)
            return std::nullopt;
        map.erase(std::get<2>(*ret));

        return std::get<1>(*ret);
    }

    const struct Printer& print() const { return printer; }
    friend std::ostream& operator<<(std::ostream& os, const Printer& p)
    {
        return p.print(os);
    }
private:
    
    // using value_type = std::tuple<int, T, int>;
    
    
    struct Comparer
    {
        std::greater<int> g;
        bool operator()(const value_type& a, const value_type& b) const
        {
            return g(std::get<0>(a), std::get<0>(b));
        }
    };
    std::vector<value_type> vec;
    std::multimap<T, size_t> map;

    Printer printer{vec};

    struct Swapper
    {
        void operator()(std::vector<value_type>& vec, size_t a, size_t b) const
        {
            auto &x = vec[a];
            auto &y = vec[b];
            using std::swap;
            swap(x, y);
            swap(std::get<2>(x)->second, std::get<2>(y)->second);
        }
    };
    using h = heap<value_type, Comparer, Swapper>;

    // array_heap<std::tuple<int, T, std::multimap<T, size_t>::iterator>, Comparer> heap;

    friend int main(int, char**);
};
