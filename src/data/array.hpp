

#ifndef Array_HPP
#define Array_HPP
#include <vector>
#include <string>
#include <optional>

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
        void map(Func f);
        void filter(Func f);
        // std::optional<T> aggregate(Func f);
        std::optional<T> getByIndex(int idx) const;
        size_t memoryUSsage() const;
        void fillNulls(T val);
        bool isNull(int idx) const;
        std::string typedef_() const; 
        std::vector<size_t> filteredIndex(Func f);
        
}

#endif
