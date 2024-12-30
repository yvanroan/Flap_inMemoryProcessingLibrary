#include <vector>
#include <string>

template <typename T>
template <typename Func>

//Array class should represent a single Array of data
//and should support the following:
//      Efficient storage of homogenous types.
//      Operations like filtering & aggregations
//      Type safety and validation

class Array{

    private: 
        std::vector<std::optional<T>> sequence;
        size_t size=0;
        string name;

    public:
        Array(string name);
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
