
#include <vector>
#include <string>
#include <array.hpp>
#include <table.hpp>
#include <optional>
#include <unordered_map>

template <typename T>
template <typename Func>


Table(std::unordered_map<std::string, Array<std::optional<T>>> input): table{input} {}
void appendCol(std::string name, Array<std::optional<T>> input){
    if(input.find(name) != input.end()){
        throw std:::invalid_argument("Column name already exist, please enter something else");
    }
    table[name] = input;
}
void appendRow(std::vector<std::optional<T>> entry){
    if (sizeof(entry) != numColumn ) {
        throw std::range_error("the number of column is different from the number of element in the entry");
    }
    int idx=0;
    for(it = input.begin(); it!= input.end(); it++, idx++){
        it->second.append(entry[idx]);
    }

}
void remove(std::string name){
    if (table.find(name) == table.end() ) {
        throw std::invalid_argument("Column name doesn't exist in the table");
    }

    table.erase(name);
}
void rename(std::string oldName, std::string newName){

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
}
T dataAt(std::string name, int idx){
    if (table.find(name) == table.end() ) {
        throw std::invalid_argument("Column name doesn't exist in the table");
    }
    if(idx > numRow || idx<0){
        throw std::out_of_range("Index out of range");
    }

    return table[name].getByIndex(idx);
}
Array<std::optional<T>> column(std::string name){
    if (table.find(name) == table.end() ) {
        throw std::invalid_argument("Column name doesn't exist in the table");
    }

    return table[name];
}
std::vector<std::any> row(size_t idx){ //similar to iloc in pandas
    if(idx > numRow || idx<0){
        throw std::out_of_range("Index out of range");
    }
    std::vector<std::any> row = {};
    for(it = table.begin(); it!= table.end(); it++){
        row.emplace_back(it->second.getByIndex(idx))
    }
    return row;
}
std::vector<std::any> rows(std::vector<size_t> indexes){
    //check for duplicates
    for(auto idx: indexes){
        if( idx > numRow || idx<0){
            throw std::out_of_range("Index out of range");
        }   
    }

    std::vector<std::vector<std::any>> rows = {};
    for(auto idx: indexes){
        std::vector<std::any> row = {};
        for(it = table.begin(); it!= table.end(); it++){
            row.emplace_back(it->second.getByIndex(idx));
        }
        rows.emplace_back(row);
    }
    return rows;
}
std::vector<std::optional<T>> rows(size_t start_idx, size_t end_idx){
    for(auto idx= start_idx; idx <= end_idx; idx++){
        if( idx > numRow || idx<0){
            throw std::out_of_range("Index out of range");
        }   
    }
    
    std::vector<std::vector<std::any>> rows = {};
    for(auto idx= start_idx; idx <= end_idx; idx++){
        std::vector<std::any> row = {};
        for(it = table.begin(); it!= table.end(); it++){
            row.emplace_back(it->second.getByIndex(idx));
        }
        rows.emplace_back(row);
    }
    return rows;
}
void filterRow(Func f){
    for(auto col: table){
        col.filter(f);
    }
}
void filterCol(std::vector<std::string> choosen, bool remove){
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
}
void map(Func f, std::string name){
    if(input.find(name) == input.end()){
        throw std:::invalid_argument("Column name doesn't exist in this table");
    }
    table[name].map(f);
}

// Table aggregation(Func f, std::vector<std::string> names){}
size_t memoryUsage() const{
    int sum = 0;
    for(auto x: table){
        sum += sizeof(x.first) + x.second.memoryUsage();
    }

    return sum;
}
Table innerJoin(Table left, Table right, std::vector<std::string> columns){
    Table result;
    std::unordered_map<std::string, size_t> rightIndexMap;

    // Create a hash map for the right table
    for (size_t i = 0; i < right.table.at(columns[0]).size(); ++i) {
        std::string key = "";
        for (const auto& col : columns) {
            key += right.table.at(col)[i].value_or("");
        }
        rightIndexMap[key] = i;
    }

    // Perform the join
    for (size_t i = 0; i < left.table.at(columns[0]).size(); ++i) {
        std::string key = "";
        for (const auto& col : columns) {
            key += left.table.at(col)[i].value_or("");
        }

        if (rightIndexMap.find(key) != rightIndexMap.end()) {
            size_t rightIndex = rightIndexMap[key];

            for (const auto& [col, values] : left.table) {
                result.table[col].push_back(values[i]);
            }

            for (const auto& [col, values] : right.table) {
                if (result.table.find(col) == result.table.end()) {
                    result.table[col].push_back(values[rightIndex]);
                }
            }
        }
    }
    return result;
    
}
Table outerJoin(Table left, Table right, std::vector<std::string> columns){}
Table leftJoin(Table left, Table right, std::vector<std::string> columns){}
Table rightJoin(Table left, Table right, std::vector<std::string> columns){}
