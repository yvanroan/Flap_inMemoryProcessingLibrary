

#include "table.hpp"
#include <stdexcept>


Table::Table(){
    mem_id = memManager::getInstance().registerInstance(0);
}


Table::Table(Table t): table{t.getTable()}{
    mem_id = memManager::getInstance().registerInstance(memoryUsage());
}

Table::Table(std::unordered_map<std::string, Array> input): table{input} {
    mem_id = memManager::getInstance().registerInstance(memoryUsage());
}

size_t Table::reportMemoryUsage() const{
    int sum = 0;
    for(auto x: table){
        sum += sizeof(x.first) + x.second.memoryUsage();
    }

    return sum;
}

void Table::reportMemoryUsage() const{
    memManager::getInstance().updateInstance(memoryUsage());
}

void Table::appendCol(std::string name, Array input){
    if(input.find(name) != input.end()){
        throw std:::invalid_argument("Column name already exist, please enter something else");
    }
    table[name] = input;
    reportMemoryUsage();
}
void Table::appendRow(std::vector<std::optional<ArrayType>> entry){
    if (entry.size() != numColumn ) {
        throw std::range_error("the number of column is different from the number of element in the entry");
    }

    int idx=0;

    for(it = input.begin(); it!= input.end(); it++, idx++){
        it->second.append(entry[idx]);
    }
    reportMemoryUsage();
}
void Table::rename(std::string oldName, std::string newName){

    oldPresent, newPresent = false, false

    for(it = table.begin(); it!= table.end(); it++){
        if(it->first == oldName){
            oldPresent=true;
        }
        if(it->first == newName){
            newPresent = true;
        }
    }

    if(!oldPresent){
        throw std::invalid_argument("Column name doesn't exist in the table");
    }

    if(newPresent){
        throw std:::invalid_argument("Column name already exist, please enter something else");
    }

    table[newName] = table[oldName];
    table.erase(oldName);

    reportMemoryUsage();
}
std::optional<ArrayType> Table::dataAt(std::string name, int idx){
    if (table.find(name) == table.end() ) {
        throw std::invalid_argument("Column name doesn't exist in the table");
    }
    if(idx >= numRow || idx<0){
        throw std::out_of_range("Index out of range");
    }

    return table[name][idx];
}
Array Table::column(std::string name){
    if (table.find(name) == table.end() ) {
        throw std::invalid_argument("Column name doesn't exist in the table");
    }

    return table[name];
}
std::vector<std::optional<ArrayType>> Table::row(size_t idx){ //similar to iloc in pandas
    if(idx >= numRow || idx<0){
        throw std::out_of_range("Index out of range");
    }
    size_t curMem=0
    std::vector<std::optional<ArrayType>> row = {};
    for(it = table.begin(); it!= table.end(); it++){
        row.emplace_back((it->second)[idx]);
    }
    memManager::getInstance().updateInstance(memoryUsage() + (row.capacity()*sizeof(ArrayType)));
    return row;
}
std::vector<std::vector<std::optional<ArrayType>>> Table::rows(std::vector<size_t> indexes){
    
    for(auto idx: indexes){
        if( idx >= numRow || idx<0){
            throw std::out_of_range("Index out of range");
        }   
    }

    size_t curMem= 0;

    std::vector<std::vector<std::optional<ArrayType>>> rows = {};
    for(auto idx: indexes){
        std::vector<std::optional<ArrayType>> row = {};
        for(it = table.begin(); it!= table.end(); it++){
            row.emplace_back((it->second)[idx]);
            
        }
        curMem += sizeof(row) + (row.capacity() * sizeof(std::optional<ArrayType>));
        rows.emplace_back(row);
    }

    memManager::getInstance().updateInstance(memoryUsage()+curMem);
    return rows;
}
std::vector<std::optional<ArrayType>> Table::rows(size_t start_idx, size_t end_idx){
    
    for(auto idx= start_idx; idx <= end_idx; idx++){
        if( idx >= numRow || idx<0){
            throw std::out_of_range("Index out of range");
        }   
    }

    size_t curMem= 0;
    
    std::vector<std::vector<std::optional<ArrayType>>> rows = {};

    for(auto idx= start_idx; idx <= end_idx; idx++){
        std::vector<ArrayType> row;
        for(it = table.begin(); it!= table.end(); it++){
            row.emplace_back((it->second)[idx]);
        }
        curMem += sizeof(row) + (row.capacity() * sizeof(std::optional<ArrayType>));
        rows.emplace_back(row);
    }

    memManager::getInstance().updateInstance(memoryUsage()+curMem);
    return rows;
}

void Table::removeCol(std::string colName){
    if(table.find(colName)==table.end()){
        throw std::invalid_argument("the column name provided doesn't exist");
    }
    table.erase(colName);
}
void Table::removeRows(std::vector<size_t> rowNumbers){

    for(auto& [_, arr]: table){
        for(size_t idx: rowNumbers){
            arr.removeByIndex(idx);
        }
    }
}

template <typename Func> 
void Table::filterRow(Func f, std::string columnName){
    
    std::vector<size_t> indexes = table[columnName].filteredIndex(f);
    
    removeRows(indexes);

    reportMemoryUsage();
}
void Table::filterCol(std::vector<std::string> choosen, bool remove){
    int numChoosen = choosen.size();
    //check for duplicates
    for(auto name: choose){
        if(table.find(name)!= table.end()){
            numChoosen--;
        }
    }

    if(numChoosen != 0){
        return;
    }

    if(remove){
        for(auto name: choose){
            table.erase(name);
        }
    }
    else{
        table.erase(
            std::remove_if(
                table.begin(), 
                table.end(),
                [&](int value) {
                    return choosen.find(value) == choosen.end(); 
                }
            ),
            table.end()
        );  
    }
    reportMemoryUsage();
}
template <typename Func> 
void Table::map(Func f, std::string name){
    if(input.find(name) == input.end()){
        throw std:::invalid_argument("Column name doesn't exist in this table");
    }
    table[name].map(f);
    reportMemoryUsage();
}


std::unordered_map<std::string, std::pair<size_t, size_t>> Table::_createHashmapFromTable(Table t){
    std::unordered_map<std::string, std::pair<size_t, size_t>> map;

    // Create a hash map for the right table
    for(size_t i = 0; i < t.getTable().at(columns[0]).size(); ++i){
        std::string key = "";
        for(const auto& col : columns){
            key += t.table.at(col)[i].value_or("")+ "|";
        }
        map[key] = make_pair(i,0);
    }

    return map;
}
Table Table::innerJoin( Table right, std::vector<std::string> columns) const{
    
    for(const auto col: columns){
        if(table.find(col) == table.end() or right.getTable().find(col) == right.getTable().end()){
            throw std::invalid_argument("Column name is absent in one of the tables");
        }
    }
    
    Table result;
    std::unordered_map<std::string, std::pair<size_t, size_t>> rightIndexMap = _createHashmapFromTable(right);
    // Perform the join
    for(size_t i = 0; i < this.table.at(columns[0]).size(); ++i){
        std::string key = "";
        for(const auto& col : columns){
            key += this.table.at(col)[i].value_or("")+ "|";
        }

        if(rightIndexMap.find(key) != rightIndexMap.end()){
            size_t rightIndex = rightIndexMap[key].first;
            rightIndexMap[key].second++;

            for(const auto& [col, values] : this.table){
                result.table[col].append(values[i]);
            }

            for(const auto& [col, values] : right.table){
                if(result.table.find(col) == result.table.end()){
                    result.table[col].append(values[rightIndex]);
                }
            }
        }
    }
    return result;
    
}
Table Table::outerJoin( Table right, std::vector<std::string> columns ) const{
    
    for(const auto col: columns){
        if(table.find(col) == table.end() or right.getTable().find(col) == right.getTable().end()){
            throw std::invalid_argument("Column name is absent in one of the tables");
        }
    }
    std::unordered_map<std::string, std::pair<size_t, size_t>> rightIndexMap = _createHashmapFromTable(right);
    
    // Perform the join
    Table result = leftJoinProcessing( right, columns, rightIndexMap);

    for(const auto& [k,v]: rightIndexMap){
        if(v.second == 0){

            for(const auto& [col, values] : right.table){
                result.table[col].append(values[v.first]);
            }

            //
            for(const auto& x : this.table){
                result.table[x.first].append(std::nullopt);
            }
        }
    }

    return result;
}
Table Table::leftJoinProcessing(Table right, std::vector<std::string> columns, std::unordered_map<std::string, std::pair<size_t, size_t>> rightIndexMap) const {
    Table result;

    

    for(size_t i = 0; i < this.table.at(columns[0]).size(); ++i){
        std::string key = "";
        for(const auto& col : columns){
            key += this.table.at(col)[i].value_or("")+ "|";
        }

        if(rightIndexMap.find(key) != rightIndexMap.end()){
            size_t rightIndex = rightIndexMap[key].first;
            rightIndexMap[key].second++;
            

            for(const auto& [col, values] : this.table){
                result.table[col].append(values[i]);
            }

            for(const auto& [col, values] : right.table){
                if(result.table.find(col) == result.table.end()){
                    result.table[col].append(values[rightIndex]);
                }
            }
        }
        else{

            for(const auto& [col, values] : this.table){
                result.table[col].append(values[i]);
            }
            
            for(const auto& x : right.table){
                result.table[x.first].append(std::nullopt);
            }

        }
    }
    return result;
}
Table Table::leftJoin( Table right, std::vector<std::string> columns) const{
    for(const auto col: columns){
        if(table.find(col) == table.end() or right.getTable().find(col) == right.getTable().end()){
            throw std::invalid_argument("Column name is absent in one of the tables");
        }
    }
    std::unordered_map<std::string, std::pair<size_t, size_t>> rightIndexMap = _createHashmapFromTable(right);
    
    // Perform the join
    return leftJoinProcessing(right, columns, rightIndexMap);
}
Table Table::rightJoin( Table right, std::vector<std::string> columns)const{

    return right.leftJoin(this, columns);
}

template <typename Func> 
ArrayType Table::aggregateColumn(const std::string& columnName, Func aggFunc, ArrayType initialValue) const{
    if (table.find(columnName) == table.end()) {
        throw std::invalid_argument("Column not found");
    }
    return table.at(columnName).aggregate(aggFunc, initialValue);
}

template <typename Func> 
Table Table::aggregateColumns(const std::vector<std::string>& columnNames, Func aggFunc, ArrayType initialValue) const {
    std::unordered_map<std::string, ArrayType> results;
    for (const auto& columnName : columnNames) {
        results[columnName] = aggregateColumn(columnName, aggFunc, initialValue);
    }
    return Table(results);
}
std::unordered_map<std::string, Array> Table::getTable(){
    return table;
}

size_t Table::getNumColumn(){
    return numColumn;
}
size_t Table::getNumRow(){
    return numRow;
}

Array& Table::operator[](const std::string& columnName) {
    
    if (table.find(columnName) == table.end()) {
        table[columnName] = Array();
        ++numColum;
    }
    return table[columnName];
}