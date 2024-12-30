

#ifndef Array_HPP
#define Array_HPP
#include <vector>
#include <string>

template <typename T>
template <typename Func>

class Array{

    private: 
        std::vector<std::optional<T>> sequence;
        size_t size=0;

    public:
        Array(std::vector<std::optional<T>> input);
        void append(std::optional<T> data);
        void append(const Array<std::optional<T>>& col);
        void appendNull();
        Array<std::optional<T>> map(Func f);
        Array<std::optional<T>> filter(Func f);
        // std::optional<T> aggregate(Func f);
        std::optional<T> getByIndex(int idx) const;
        size_t memory_usage() const;
        void fill_nulls(T val);
        bool is_null(int idx);

}

#endif
