#ifndef DATAFRAME_HPP
#define DATAFRAME_HPP

#include <vector>
#include <string>
#include "Series.hpp"
#include "Table.hpp"
#include <variant>
#include <unordered_map>


// supported types
using SeriesType = std::variant<int, double, float, std::string>;
using FuncReturnType = std::variant<std::vector<std::optional<SeriesType>>, std::vector<std::vector<std::optional<SeriesType>>>, Array*, Table* ,std::vector<size_t>, SeriesType, std::optional<SeriesType>>;

class Dataframe{

    private: 
        std::shared_ptr<Table> table;

        enum Type {VOID, SCALAR, TABLE, MATRIX, ARRAY, ROW_VECTOR};

    protected:
        std::shared_ptr<Table> getTable();

    public:
        Dataframe();
        Dataframe(Dataframe d);
        void appendCol(std::string name, Series input);
        void appendRow(std::vector<sSeriesType> entry);
        void rename(std::string oldName, std::string newName);
        Series column(std::string name);
        std::vector<SeriesType> row(size_t idx); //similar to iloc in pandas
        std::vector<std::vector<SeriesType>> rows(std::vector<size_t> indexes);
        std::vector<std::vector<SeriesType>> rows(size_t start_idx, size_t end_idx);
        void removeCol(std::string colName);
        void removeRows(std::vector<size_t> rowNumbers);
        template <typename Func> 
        void filterRow(Func f, std::string columnName);
        void filterCol(std::vector<std::string> choosen, bool remove= false);
        template <typename Func> 
        void map(Func f, std::string name);
        template <typename Func> 
        SeriesType aggregateColumn(const std::string& columnName, Func aggFunc, SeriesType initialValue) const;
        template <typename Func> 
        Dataframe aggregateColumns(const std::vector<std::string>& columnNames, Func aggFunc, SeriesType initialValue) const;
        Dataframe innerJoin(Dataframe left, Dataframe right, std::vector<std::string> columns);
        Dataframe outerJoin(Dataframe left, Dataframe right, std::vector<std::string> columns);
        Dataframe leftJoin(Dataframe left, Dataframe right, std::vector<std::string> columns);
        Dataframe rightJoin(Dataframe left, Dataframe right, std::vector<std::string> columns);

        size_t getNumColumn();
        size_t getNumRow();
        Series& operator[](const std::string& columnName) ;
        std::any collect();
}
#endif
