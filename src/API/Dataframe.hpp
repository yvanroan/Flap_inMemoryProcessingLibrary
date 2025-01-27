#pragma once

#include <vector>
#include <string>
#include "Series.hpp"
#include "Table.hpp"
#include <unordered_map>



class Dataframe{

    private: 
        std::shared_ptr<Table> table;

    protected:
        std::shared_ptr<Table> getTable();

    public:
        Dataframe();
        Dataframe(Dataframe& d);
        Dataframe(Table t);
        void appendCol(std::string name, Series input);
        void rename(std::string oldName, std::string newName);
        Series column(std::string name);

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

        bool operator==(Dataframe& d);
        bool operator!=(Dataframe& d);
        
        Dataframe innerJoin(Dataframe left, Dataframe right, std::set<std::string> columns);
        Dataframe outerJoin(Dataframe left, Dataframe right, std::set<std::string> columns);
        Dataframe leftJoin(Dataframe left, Dataframe right, std::set<std::string> columns);
        Dataframe rightJoin(Dataframe left, Dataframe right, std::set<std::string> columns);

        size_t getNumColumn();
        size_t getNumRow();
        Series& operator[](const std::string& columnName) ;
        // std::any collect();
};
