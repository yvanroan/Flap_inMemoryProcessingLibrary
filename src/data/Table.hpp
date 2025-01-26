#pragma once

#include <string>
#include <set>
#include <any>
#include "Array.hpp"
#include <functional>

#include <vector>
#include <unordered_map>


// supported types
// using ArrayType = std::variant<int, double, float, std::string>;

class Table{

    private: 
        std::unordered_map<std::string, std::shared_ptr<Array>> table;
        size_t numColumn=0;
        size_t numRow=0;
        size_t mem_id;

        static std::unordered_map<std::string, size_t> _createHashmapFromTable(Table t,  std::set<std::string> columns);
        static Table leftJoinProcessing(Table left, Table right, std::set<std::string> columns, std::unordered_map<std::string, size_t> rightIndexMap);

        public:

        Table();
        Table(Table& t);
        Table(std::unordered_map<std::string, std::shared_ptr<Array>> input);
        void appendCol(std::string name, std::shared_ptr<Array> input);
        void appendRow(std::vector<std::pair<std::string, std::any>> entry);
        void rename(std::string oldName, std::string newName);
        std::vector<std::pair<std::string, std::any>> row(size_t idx); //similar to iloc in pandas
        std::vector<std::vector<std::pair<std::string, std::any>>> rows(std::vector<size_t> indexes);
        std::vector<std::vector<std::pair<std::string, std::any>>> rows(size_t start_idx, size_t end_idx);
        void removeCol(std::string colName);
        void removeRows(std::vector<size_t> rowNumbers);
        
        void filterRow(std::function<bool(const int&)> f, std::string columnName);
        void filterRow(std::function<bool(const std::string&)> f, std::string columnName);
        void filterRow(std::function<bool(const float&)> f, std::string columnName);
        void filterCol(std::set<std::string> choosen, bool remove= false);
        
        void map(std::function<void(int&)> f, std::string name);
        void map(std::function<void(std::string&)> f, std::string name);
        void map(std::function<void(float&)> f, std::string name);
        
        int aggregateColumn( std::string& columnName, std::function<int(int, int)> aggFunc, int initialValue) ;
        std::string aggregateColumn( std::string& columnName, std::function<std::string(std::string, std::string)> aggFunc, std::string initialValue) ;
        float aggregateColumn( std::string& columnName, std::function<float(float, float)> aggFunc, float initialValue) ;

        Table aggregateColumn( std::vector<std::string>& columnNames, std::function<int(int, int)> aggFunc, int initialValue);
        Table aggregateColumn( std::vector<std::string>& columnNames, std::function<std::string(std::string, std::string)> aggFunc, std::string initialValue) ;
        Table aggregateColumn( std::vector<std::string>& columnNames, std::function<float(float, float)> aggFunc, float initialValue) ;

        bool operator==(Table& table);
        bool operator!=(Table& table);

        void reportMemoryUsage() const;
        size_t memoryUsage() const;
        static Table innerJoin(Table left, Table right, std::set<std::string> columns);
        static Table outerJoin(Table left, Table right, std::set<std::string> columns);
        static Table leftJoin(Table left, Table right, std::set<std::string> columns);
        static Table rightJoin(Table left, Table right, std::set<std::string> columns);
        std::unordered_map<std::string, std::shared_ptr<Array>> getTable() const;
        size_t getNumColumn();
        size_t getNumRow();
        std::shared_ptr<Array>& operator[](const std::string& columnName);
};

std::ostream& operator<<(std::ostream& os, const Table& arr ) ;
