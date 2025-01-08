

#ifndef Array_HPP
#define Array_HPP
#include <vector>
#include <string>
#include <optional>
#include <variant>

using ArrayType = std::variant<int, float, double, std::string>;

class Array{

    private: 
        std::vector<std::optional<ArrayType>> sequence;
        size_t size;
        size_t mem_id;

    public:
        Array();
        Array(std::vector<std::optional<ArrayType>> input);
        Array(std::optional<ArrayType> data);
        void append(std::optional<ArrayType> data);
        void extend(const Array& col);
        void appendNull();

        template <typename Func> 
        void map(Func f);

        template <typename Func> 
        void filter(Func f);

        size_t size();
        std::optional<ArrayType> operator[](int idx) const;
        size_t memoryUsage() const;
        void fillNulls(ArrayType val);
        bool isNull(int idx) const;
        std::string typedef_() const; 

        template <typename Func> 
        std::vector<size_t> filteredIndex(Func f);

        template <typename Func> 
        ArrayType aggregate(Func aggFunc, ArrayType initialValue) const;
        std::vector<std::optional<ArrayType>> getArray();

        void reportMemoryUsage() const;  
}

#endif
