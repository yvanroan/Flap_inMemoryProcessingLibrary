#ifndef Table_HPP
#define Table_HPP

#include <vector>
#include <string>
#include "array.hpp"
#include <variant>
#include <optional>
#include <unordered_map>


// supported types
using ArrayType = std::variant<int, double, float, std::string>;

class Table{

    private: 
        std::unordered_map<std::string, Array> table;
        size_t numColum;
        size_t numRow;
        size_t mem_id;

        std::unordered_map<std::string, std::pair<size_t, size_t>> _createHashmapFromTable(Table t);
        Table leftJoinProcessing(Table left, Table right, std::vector<std::string> columns, std::unordered_map<std::string, std::pair<size_t, size_t>> rightIndexMap);
    public:
        Table();
        Table(Table t);
        Table(std::unordered_map<std::string, Array> input);
        void appendCol(std::string name, Array input);
        void appendRow(std::vector<std::optional<ArrayType>> entry);
        void rename(std::string oldName, std::string newName);
        std::optional<ArrayType> dataAt(std::string name, size_t idx);
        Array column(std::string name);
        std::vector<std::optional<ArrayType>> row(size_t idx); //similar to iloc in pandas
        std::vector<std::vector<std::optional<ArrayType>>> rows(std::vector<size_t> indexes);
        std::vector<std::vector<std::optional<ArrayType>>> rows(size_t start_idx, size_t end_idx);
        void removeCol(std::string colName);
        void removeRows(std::vector<size_t> rowNumbers);
        template <typename Func> 
        void filterRow(Func f, std::string columnName);
        void filterCol(std::vector<std::string> choosen, bool remove= false);
        template <typename Func> 
        void map(Func f, std::string name);
        template <typename Func> 
        ArrayType aggregateColumn(const std::string& columnName, Func aggFunc, ArrayType initialValue) const;
        template <typename Func> 
        Table aggregateColumns(const std::vector<std::string>& columnNames, Func aggFunc, ArrayType initialValue) const;
        void reportMemoryUsage() const;
        size_t memoryUsage() const;
        Table innerJoin(Table left, Table right, std::vector<std::string> columns);
        Table outerJoin(Table left, Table right, std::vector<std::string> columns);
        Table leftJoin(Table left, Table right, std::vector<std::string> columns);
        Table rightJoin(Table left, Table right, std::vector<std::string> columns);
        std::unordered_map<std::string, Array> getTable();
        size_t getNumColumn();
        size_t getNumRow();
        Array& operator[](const std::string& columnName) ;
}
#endif
