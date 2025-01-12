

#ifndef Series_HPP
#define Series_HPP
#include <vector>
#include <string>
#include <variant>
#include <memory>
#include "Array.hpp"

using SeriesType = std::variant<size_t, int, float, double, std::string, bool>;
using FuncReturnType = std::variant<std::vector<std::optional<SeriesType>>, std::vector<std::vector<std::optional<SeriesType>>>, Array*, Table* ,std::vector<size_t>, SeriesType, std::optional<SeriesType>>;

class Series{

    private: 
        std::shared_ptr<Array> arr;

        enum Type {VOID, SCALAR, VECTOR_SIZE};

    protected:
        std::shared_ptr<Array> getArray();

    public:
        Series();
        Series(Array arr);
        Series(std::vector<SeriesType> input);
        Series(SeriesType data);
        void append(SeriesType data);
        void extend(const Series& col);
        
        void appendNull();

        template <typename Func> 
        void map(Func f);

        template <typename Func> 
        void filter(Func f);

        size_t size();
        SeriesType operator[](int idx) const;
        void fillNulls(SeriesType val);
        bool isNull(int idx) const;

        template <typename Func> 
        std::vector<size_t> filteredIndex(Func f);

        void removeByIndex(const std::vector<size_t> indexes);

        template <typename Func> 
        SeriesType aggregate(Func aggFunc, SeriesType initialValue) const;

        std::any collect();
        
}

#endif
