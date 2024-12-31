#ifndef Table_HPP
#define Table_HPP

#include <vector>
#include <string>
#include "array.hpp"
#include <optional>
#include <unordered_map>

class Array;

template <typename T> class Table{

    private: 
        std::unordered_map<std::string, Array<std::optional<T>>> table;
        size_t numColum;
        size_t numRow;

        std::unordered_map<std::string, std::pair<size_t, size_t>> _createHashmapFromTable(Table t);
        Table leftJoinProcessing(Table left, Table right, std::vector<std::string> columns, std::unordered_map<std::string, std::pair<size_t, size_t>> rightIndexMap);
    public:
        Table(std::unordered_map<std::string, Array<std::optional<T>>> input);
        void appendCol(std::string name, Array<std::optional<T>> input);
        void appendRow(std::vector<std::optional<T>> entry);
        void rename(std::string oldName, std::string newName);
        std::optional<T> dataAt(std::string name, size_t idx);
        Array<std::optional<T>> column(std::string name);
        std::vector<std::any> row(size_t idx); //similar to iloc in pandas
        std::vector<std::vector<std::any>> rows(std::vector<size_t> indexes);
        std::vector<std::vector<std::any>> rows(size_t start_idx, size_t end_idx);
        template <typename Func> void filterRow(Func f);
        void filterCol(std::vector<std::string> choosen, bool remove= false);
        template <typename Func> void map(Func f, std::string name);
        template <typename Func> T aggregateColumn(const std::string& columnName, Func aggFunc, T initialValue) const;
        template <typename Func> std::unordered_map<std::string, T> aggregateColumns(const std::vector<std::string>& columnNames, Func aggFunc, T initialValue) const
        // Table aggregation(Func f, std::vector<std::string> names);
        size_t memoryUsage() const;
        Table innerJoin(Table left, Table right, std::vector<std::string> columns);
        Table outerJoin(Table left, Table right, std::vector<std::string> columns);
        Table leftJoin(Table left, Table right, std::vector<std::string> columns);
        Table rightJoin(Table left, Table right, std::vector<std::string> columns);
}
#endif
