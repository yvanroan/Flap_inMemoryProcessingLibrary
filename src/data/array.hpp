

#ifndef Array_HPP
#define Array_HPP
#include <vector>
#include <string>
#include <optional>




template <typename T> class Array{

    private: 
        std::vector<std::optional<T>> sequence;
        size_t size=0;

    public:
        Array(std::vector<std::optional<T>> input);
        void append(std::optional<T> data);
        void append(const Array<std::optional<T>>& col);
        void appendNull();
        template <typename Func> void map(Func f);
        template <typename Func> void filter(Func f);
        size_t size();
        // std::optional<T> aggregate(Func f);
        std::optional<T> getByIndex(int idx) const;
        size_t memoryUSsage() const;
        void fillNulls(T val);
        bool isNull(int idx) const;
        std::string typedef_() const; 
        template <typename Func> std::vector<size_t> filteredIndex(Func f);
        template <typename Func> T aggregate(Func aggFunc, T initialValue) const
        
}

#endif
