

#include "Table.hpp"
#include "ArrayInt.hpp"
#include "ArrayString.hpp"
#include "ArrayFloat.hpp"
#include <stdexcept>
#include <limits>
#include <algorithm>


Table::Table(){
    // mem_id = memManager::getInstance().registerInstance(0);
}


Table::Table(Table& t): numColumn{t.getNumColumn()}, 
    numRow{t.getNumRow()}{
    
    for (const auto& [colName, colArray] : t.getTable()) {
        if (auto intCol = std::dynamic_pointer_cast<ArrayInt>(colArray)) {
            table[colName] = std::make_shared<ArrayInt>(*intCol); 
        } else if (auto stringCol = std::dynamic_pointer_cast<ArrayString>(colArray)) {
            table[colName] = std::make_shared<ArrayString>(*stringCol); 
        } else if (auto floatCol = std::dynamic_pointer_cast<ArrayFloat>(colArray)) {
            table[colName] = std::make_shared<ArrayFloat>(*floatCol); 
        } else {
            throw std::runtime_error("Unknown column type during deep copy");
        }
    }
    // mem_id = memManager::getInstance().registerInstance(memoryUsage());
}

Table::Table(std::unordered_map<std::string, std::shared_ptr<Array>> input){

    if (!input.empty()) {
        numColumn = input.size();

        auto it = input.begin();

        if (it != input.end() && it->second) {
            numRow = it->second->getSize();
        } else {
            numRow = 0;
        }

        for (const auto& [colName, colArray] : input) {
            if (auto intCol = std::dynamic_pointer_cast<ArrayInt>(colArray)) {
                table[colName] = std::make_shared<ArrayInt>(*intCol); // Deep copy ArrayInt
            } else if (auto stringCol = std::dynamic_pointer_cast<ArrayString>(colArray)) {
                table[colName] = std::make_shared<ArrayString>(*stringCol); // Deep copy ArrayString
            } else if (auto floatCol = std::dynamic_pointer_cast<ArrayFloat>(colArray)) {
                table[colName] = std::make_shared<ArrayFloat>(*floatCol); // Deep copy ArrayFloat
            } else {
                throw std::runtime_error("Unknown column type during deep copy");
            }
        }
        // for (; it != input.end(); ++it) {
        //     if (!it->second || it->second->getSize() != numRow) {
                
        //         throw std::runtime_error("Inconsistent Array sizes or null pointers in input map.");
        //     }
        // }
    }
    // mem_id = memManager::getInstance().registerInstance(memoryUsage());
}

size_t Table::memoryUsage() const{
    int sum = 0;
    for(auto x: table){
        sum += sizeof(x.first) + x.second->memoryUsage();
    }

    return sum;
}

void Table::reportMemoryUsage() const{
    // memManager::getInstance().updateInstance(memoryUsage());
}

void Table::appendCol(std::string name, std::shared_ptr<Array> input){
    
    if(table.find(name) != table.end()){
        throw std::invalid_argument("Column name already exist, please enter something else");
    }

    if (!input) {
        throw std::invalid_argument("Input column cannot be null.");
    }    
    
    table.insert({name, input->clone()});
    numColumn++;
    reportMemoryUsage();
}
void Table::appendRow(std::vector<std::pair<std::string, std::any>> entry){
    if (entry.size() != numColumn ) {
        throw std::range_error("the number of column is different from the number of element in the entry");
    }

    bool noRows = false;

    if(getNumRow()==0){
        noRows = true;
    }

    int i=0;

    for (const auto& [name, input] : entry) {
        if (table.find(name) == table.end()) {
            throw std::invalid_argument("Column name " + name + " does not exist.");
        }
        if(noRows){
            break;
        }
        const auto& columnType = table[name]->typedef_();

        if (columnType == "int" && input.type() != typeid(int)) {
            throw std::invalid_argument("Type mismatch: expected int at column " + name);
        } else if (columnType == "string" && input.type() != typeid(std::string)) {
            throw std::invalid_argument("Type mismatch: expected string at column " + name);
        } else if (columnType == "float" && input.type() != typeid(float)) {
            throw std::invalid_argument("Type mismatch: expected float at column " + name);
        }
    }
   

    for(auto i = 0; i< entry.size(); i++){
        const auto& columnName = entry[i].first;
        auto& columnArray = table[columnName];
        const auto& inputValue = entry[i].second;


        if (noRows) {
            if (inputValue.type() == typeid(int)) {
                auto intCol = std::make_shared<ArrayInt>();
                intCol->append(std::any_cast<int>(inputValue));
                table[columnName] = intCol;
            } else if (inputValue.type() == typeid(std::string)) {
                auto strCol = std::make_shared<ArrayString>();
                strCol->append(std::any_cast<std::string>(inputValue));
                table[columnName] = strCol;
            } else if (inputValue.type() == typeid(float)) {
                auto floatCol = std::make_shared<ArrayFloat>();
                floatCol->append(std::any_cast<float>(inputValue));
                table[columnName] = floatCol;
            } else {
                throw std::runtime_error("Unsupported data type for new column " + columnName);
            }
        } else {
            if (auto intCol = std::dynamic_pointer_cast<ArrayInt>(columnArray)) {
                intCol->append(std::any_cast<int>(inputValue));
            } else if (auto stringCol = std::dynamic_pointer_cast<ArrayString>(columnArray)) {
                stringCol->append(std::any_cast<std::string>(inputValue));
            } else if (auto floatCol = std::dynamic_pointer_cast<ArrayFloat>(columnArray)) {
                floatCol->append(std::any_cast<float>(inputValue));
            } else {
                throw std::runtime_error("Unsupported column type at column " + columnName);
            }
        }        
    }
    numRow++;
    reportMemoryUsage();
}

void Table::rename(std::string oldName, std::string newName){

    bool oldInTable =  false;
    bool newInTable = false;

    for(auto it = table.begin(); it!= table.end(); it++){
        if(it->first == oldName){
            oldInTable=true;
        }
        if(it->first == newName){
            newInTable = true;
        }
    }

    if(!oldInTable){
        throw std::invalid_argument("Column name doesn't exist in the table");
    }

    if(newInTable){
        throw std::invalid_argument("Column name already exist, please enter something else");
    }

    table[newName] = table[oldName];
    table.erase(oldName);

    reportMemoryUsage();
}


std::vector<std::pair<std::string, std::any>> Table::row(size_t idx){ //similar to iloc in pandas
    if(idx >= numRow || idx<0){
        throw std::out_of_range("Index out of range");
    }

    std::vector<std::pair<std::string, std::any>> row = {};

    for (auto & [name, col]: table){
        std::string type = col->typedef_();
        if(type == "int"){
            std::shared_ptr<ArrayInt> arr = std::dynamic_pointer_cast<ArrayInt>(col);
            row.emplace_back(std::make_pair(name,(*arr)[idx]));
        }
        else if(type == "string"){
            std::shared_ptr<ArrayString> arr = std::dynamic_pointer_cast<ArrayString>(col);
            row.emplace_back(std::make_pair(name,(*arr)[idx]));
        }
        else if(type == "float"){
            std::shared_ptr<ArrayFloat> arr = std::dynamic_pointer_cast<ArrayFloat>(col);
            row.emplace_back(std::make_pair(name,(*arr)[idx]));
        }
        else{
            throw std::invalid_argument("the array is null");
        }
    }

    // memManager::getInstance().updateInstance(memoryUsage() + (row.capacity()*sizeof(std::string)));
    return row;
}

std::vector<std::vector<std::pair<std::string, std::any>>> Table::rows(std::vector<size_t> indexes){
    
    for(auto idx: indexes){
        if( idx >= numRow || idx<0){
            throw std::out_of_range("Index out of range");
        }   
    }

    size_t curMem= 0;

    std::vector<std::vector<std::pair<std::string, std::any>>> rows = {};
    for(auto idx: indexes){
        std::vector<std::pair<std::string, std::any>> row = this->row(idx);
        rows.emplace_back(row);
        //memManager is updated via row method
    }

    // memManager::getInstance().updateInstance(memoryUsage()+curMem);
    return rows;
}

std::vector<std::vector<std::pair<std::string, std::any>>> Table::rows(size_t start_idx, size_t end_idx){
    
    for(auto idx= start_idx; idx <= end_idx; idx++){
        if( idx >= numRow || idx<0){
            throw std::out_of_range("Index out of range");
        }   
    }
    
    std::vector<std::vector<std::pair<std::string, std::any>>> rows = {};

    for(auto idx= start_idx; idx <= end_idx; idx++){
        std::vector<std::pair<std::string, std::any>> row = this->row(idx);
        rows.emplace_back(row);
    }

    // memManager::getInstance().updateInstance(memoryUsage()+curMem);

    return rows;
}

void Table::removeCol(std::string colName){
    if(table.find(colName)==table.end()){
        throw std::invalid_argument("the column name provided doesn't exist");
    }
    table.erase(colName);
    numColumn--;
}

void Table::removeRows(std::vector<size_t> rowNumbers){

    for(size_t cur: rowNumbers){
        if(cur<0 or cur> numRow){
            throw std::invalid_argument("one of the row numbers are out of range");
        }
    }

    std::sort(rowNumbers.rbegin(), rowNumbers.rend());

    for (auto& [_, col] : table) {
        for (size_t idx : rowNumbers) {
            std::string type = col->typedef_();

            if (type == "int") {
                auto arr = std::dynamic_pointer_cast<ArrayInt>(col);
                if (!arr || idx >= arr->getSize()) {
                    throw std::out_of_range("Index out of range for ArrayInt");
                }
                arr->removeByIndex(idx);
            } else if (type == "string") {
                auto arr = std::dynamic_pointer_cast<ArrayString>(col);
                if (!arr || idx >= arr->getSize()) {
                    throw std::out_of_range("Index out of range for ArrayString");
                }
                arr->removeByIndex(idx);
            } else if (type == "float") {
                auto arr = std::dynamic_pointer_cast<ArrayFloat>(col);
                if (!arr || idx >= arr->getSize()) {
                    throw std::out_of_range("Index out of range for ArrayFloat");
                }
                arr->removeByIndex(idx);
            } else {
                throw std::invalid_argument("Unsupported array type or null array");
            }
        }
    }
    numRow-= rowNumbers.size();
}

void Table::filterRow(std::function<bool(const int&)> f, std::string columnName){
    if (table.find(columnName) == table.end()) {
        throw std::invalid_argument("Column name does not exist in the table");
    }
    std::shared_ptr<ArrayInt> arr = std::dynamic_pointer_cast<ArrayInt>(table[columnName]);
    
    if (!arr) {
        throw std::invalid_argument("Column is not of type ArrayFloat");
    }
    
    std::vector<size_t> indexes = arr->filteredIndex(f);
    
    if (!indexes.empty()) {
        removeRows(indexes);
    }
    reportMemoryUsage();
}

void Table::filterRow(std::function<bool(const std::string&)> f, std::string columnName){
    if (table.find(columnName) == table.end()) {
        throw std::invalid_argument("Column name does not exist in the table");
    }
    std::shared_ptr<ArrayString> arr = std::dynamic_pointer_cast<ArrayString>(table[columnName]);
    
    if (!arr) {
        throw std::invalid_argument("Column is not of type ArrayFloat");
    }
    
    std::vector<size_t> indexes = arr->filteredIndex(f);
    
    if (!indexes.empty()) {
        removeRows(indexes);
    }

    reportMemoryUsage();
}

void Table::filterRow(std::function<bool(const float&)> f, std::string columnName){
    if (table.find(columnName) == table.end()) {
        throw std::invalid_argument("Column name does not exist in the table");
    }

    auto arr = std::dynamic_pointer_cast<ArrayFloat>(table[columnName]);
    if (!arr) {
        throw std::invalid_argument("Column is not of type ArrayFloat");
    }

    std::vector<size_t> indexes = arr->filteredIndex(f);
    if (!indexes.empty()) {
        removeRows(indexes);
    }
    reportMemoryUsage();
}

void Table::filterCol(std::set<std::string> choosen, bool remove){
    int numChoosen = choosen.size();
    for(auto name: choosen){
        if(table.find(name)!= table.end()){
            numChoosen--;
        }
    }

    if(numChoosen > 0){
        throw std::out_of_range("One or more columns are not present in the table");
    }

    if(remove){
        for(auto name: choosen){
            table.erase(name);
        }

        numColumn -= choosen.size();
    }
    else{
        for (auto it = table.begin(); it != table.end(); ) {
            if (choosen.find(it->first) == choosen.end()) {
                it = table.erase(it); 
            } else {
                ++it;
            }
        }

        numColumn = choosen.size();
    }
    reportMemoryUsage();
}


void Table::map(std::function<void(int&)> f, std::string name){
    if(table.find(name) == table.end()){
        throw std::invalid_argument("Column name doesn't exist in this table");
    }
    std::shared_ptr<ArrayInt> arr = std::dynamic_pointer_cast<ArrayInt>(table[name]);
    
    if (!arr) {
        throw std::invalid_argument("Column type mismatch: expected ArrayInt for column " + name);
    }
    arr->map(f);
    reportMemoryUsage();
}

void Table::map(std::function<void(std::string&)> f, std::string name){
    if(table.find(name) == table.end()){
        throw std::invalid_argument("Column name doesn't exist in this table");
    }

    std::shared_ptr<ArrayString> arr = std::dynamic_pointer_cast<ArrayString>(table[name]);
    
    if (!arr) {
        throw std::invalid_argument("Column type mismatch: expected ArrayString for column " + name);
    }
    arr->map(f);
    reportMemoryUsage();
}

void Table::map(std::function<void(float&)> f, std::string name){
    if(table.find(name) == table.end()){
        throw std::invalid_argument("Column name doesn't exist in this table");
    }
    std::shared_ptr<ArrayFloat> arr = std::dynamic_pointer_cast<ArrayFloat>(table[name]);
    
    if (!arr) {
        throw std::invalid_argument("Column type mismatch: expected ArrayFloat for column " + name);
    }
    arr->map(f);
    reportMemoryUsage();
}


std::unordered_map<std::string, size_t> Table::_createHashmapFromTable(Table t, std::set<std::string> columns){
    
    std::unordered_map<std::string, size_t> map;
    auto sub_table = t.getTable();
    for(auto& cols : columns){
        if(sub_table.find(cols)==sub_table.end()){
            throw std::invalid_argument("one of the columns is not in the right table");
        }
    }
    // Create a hash map for the right table
    for(size_t i = 0; i < t.getNumRow(); ++i){
        std::string key = "";
        for(auto& cols : columns){
            auto col = sub_table[cols];

            std::string type = col->typedef_();
            if(type == "int"){
                std::shared_ptr<ArrayInt> arr = std::dynamic_pointer_cast<ArrayInt>(col);
                key += std::to_string((*arr)[i]) + "|";
            }
            else if(type == "string"){
                std::shared_ptr<ArrayString> arr = std::dynamic_pointer_cast<ArrayString>(col);
                key += (*arr)[i] + "|";
            }
            else if(type == "float"){
                std::shared_ptr<ArrayFloat> arr = std::dynamic_pointer_cast<ArrayFloat>(col);
                key += std::to_string((*arr)[i]) + "|";
            }
            else{
                throw std::invalid_argument("the array is null");
            }
        }
        map[key]= i;
        
    }

    return map;
}

Table Table::innerJoin( Table left, Table right, std::set<std::string> columns) {

    std::unordered_map<std::string, size_t> rightIndexMap = _createHashmapFromTable(right, columns);
    std::unordered_map<std::string, std::shared_ptr<Array>> left_table = left.getTable();
    std::unordered_map<std::string, std::shared_ptr<Array>> right_table = right.getTable();


    for(auto& col: columns){
        if(left_table.find(col) == left_table.end()){
            throw std::invalid_argument("one of the column doesn't exist in the left table");
        }
    }

    Table result;

    for(auto& [name, col]: left_table){

        auto type = col->typedef_();
        if (type == "int") {
            result.appendCol(name, std::make_shared<ArrayInt>());
        } else if (type == "string") {
            result.appendCol(name, std::make_shared<ArrayString>());
        } else if (type == "float") {
            result.appendCol(name, std::make_shared<ArrayFloat>());
        }
    }

    for(auto& [name, col]: right_table){
        if(columns.find(name) != columns.end()){
            continue;
        }

        auto type = col->typedef_();
        if (type == "int") {
            result.appendCol(name, std::make_shared<ArrayInt>());
        } else if (type == "string") {
            result.appendCol(name, std::make_shared<ArrayString>());
        } else if (type == "float") {
            result.appendCol(name, std::make_shared<ArrayFloat>());
        }
    }

    

    for(size_t i = 0; i < left.getNumRow(); ++i){
        std::string key = "";
        for(const auto& col : columns){
            std::string type = left_table[col]->typedef_();
            if(type == "int"){
                std::shared_ptr<ArrayInt> arr = std::dynamic_pointer_cast<ArrayInt>(left_table[col]);
                key += std::to_string((*arr)[i]) + "|";
            }
            else if(type == "string"){
                std::shared_ptr<ArrayString> arr = std::dynamic_pointer_cast<ArrayString>(left_table[col]);
                key += (*arr)[i] + "|";
            }
            else if(type == "float"){
                std::shared_ptr<ArrayFloat> arr = std::dynamic_pointer_cast<ArrayFloat>(left_table[col]);
                key += std::to_string((*arr)[i]) + "|";
            }
            else{
                throw std::invalid_argument("the array is null");
            }
        }

        if(rightIndexMap.find(key) != rightIndexMap.end()){
            size_t rightIndex = rightIndexMap[key];

            std::vector<std::pair<std::string, std::any>> rightRow = right.row(rightIndex);
            std::vector<std::pair<std::string, std::any>> leftRow = left.row(i);

            for(auto& [columnName, inputValue]: leftRow){
                auto type = result[columnName]->typedef_();

                if (type == "int" and inputValue.type() == typeid(int)){
                    std::shared_ptr<ArrayInt> arr = std::dynamic_pointer_cast<ArrayInt>(result[columnName]);
                    arr->append(std::any_cast<int>(inputValue));

                } else if (type == "string" and inputValue.type() == typeid(std::string)) {
                    std::shared_ptr<ArrayString> arr = std::dynamic_pointer_cast<ArrayString>(result[columnName]);
                    arr->append(std::any_cast<std::string>(inputValue));
                } else if (type == "float" and  inputValue.type() == typeid(float)) {
                    std::shared_ptr<ArrayFloat> arr = std::dynamic_pointer_cast<ArrayFloat>(result[columnName]);
                    arr->append(std::any_cast<float>(inputValue));
                } else {
                    throw std::runtime_error("Unsupported data type for new column " + columnName);
                }
            }

            for(auto& [columnName, inputValue]: rightRow){

                if(columns.find(columnName) != columns.end()){
                    continue;
                }
                auto type = result[columnName]->typedef_();

                if (type == "int" and inputValue.type() == typeid(int)){
                    std::shared_ptr<ArrayInt> arr = std::dynamic_pointer_cast<ArrayInt>(result[columnName]);
                    arr->append(std::any_cast<int>(inputValue));

                } else if (type == "string" and inputValue.type() == typeid(std::string)) {
                    std::shared_ptr<ArrayString> arr = std::dynamic_pointer_cast<ArrayString>(result[columnName]);
                    arr->append(std::any_cast<std::string>(inputValue));
                } else if (type == "float" and  inputValue.type() == typeid(float)) {
                    std::shared_ptr<ArrayFloat> arr = std::dynamic_pointer_cast<ArrayFloat>(result[columnName]);
                    arr->append(std::any_cast<float>(inputValue));
                } else {
                    throw std::runtime_error("Unsupported data type for new column " + columnName);
                }
            }
        
        }
        
    }
    return result;
    
}

Table Table::outerJoin(Table left, Table right, std::set<std::string> columns) {
    
    std::unordered_map<std::string, size_t> rightIndexMap = _createHashmapFromTable(right, columns);
    std::unordered_map<std::string, std::shared_ptr<Array>> left_table = left.getTable();
    std::unordered_map<std::string, std::shared_ptr<Array>> right_table = right.getTable();

    std::set<int> matchedRightIdx;

    for(auto& col: columns){
        if(left_table.find(col) == left_table.end()){
            throw std::invalid_argument("one of the column doesn't exist in the left table");
        }
    }

    Table result;

    for(auto& [name, col]: left_table){

        auto type = col->typedef_();
        if (type == "int") {
            result.appendCol(name, std::make_shared<ArrayInt>());
        } else if (type == "string") {
            result.appendCol(name, std::make_shared<ArrayString>());
        } else if (type == "float") {
            result.appendCol(name, std::make_shared<ArrayFloat>());
        }
    }

    for(auto& [name, col]: right_table){
        if(columns.find(name) != columns.end()){
            continue;
        }

        auto type = col->typedef_();
        if (type == "int") {
            result.appendCol(name, std::make_shared<ArrayInt>());
        } else if (type == "string") {
            result.appendCol(name, std::make_shared<ArrayString>());
        } else if (type == "float") {
            result.appendCol(name, std::make_shared<ArrayFloat>());
        }
    }
    
    for(size_t i = 0; i < left.getNumRow(); ++i){
        std::string key = "";
        for(const auto& col : columns){
            std::string type = left_table[col]->typedef_();
            if(type == "int"){
                std::shared_ptr<ArrayInt> arr = std::dynamic_pointer_cast<ArrayInt>(left_table[col]);
                key += std::to_string((*arr)[i]) + "|";
            }
            else if(type == "string"){
                std::shared_ptr<ArrayString> arr = std::dynamic_pointer_cast<ArrayString>(left_table[col]);
                key += (*arr)[i] + "|";
            }
            else if(type == "float"){
                std::shared_ptr<ArrayFloat> arr = std::dynamic_pointer_cast<ArrayFloat>(left_table[col]);
                key += std::to_string((*arr)[i]) + "|";
            }
            else{
                throw std::invalid_argument("the array is null");
            }
        }

        std::vector<std::pair<std::string, std::any>> leftRow = left.row(i);

        if(rightIndexMap.find(key) != rightIndexMap.end()){
            size_t rightIndex = rightIndexMap[key];
            matchedRightIdx.insert(rightIndex);
            std::vector<std::pair<std::string, std::any>> rightRow = right.row(rightIndex);

            for(auto& [columnName, inputValue]: leftRow){
                auto type = result[columnName]->typedef_();

                if (type == "int" and inputValue.type() == typeid(int)){
                    std::shared_ptr<ArrayInt> arr = std::dynamic_pointer_cast<ArrayInt>(result[columnName]);
                    arr->append(std::any_cast<int>(inputValue));

                } else if (type == "string" and inputValue.type() == typeid(std::string)) {
                    std::shared_ptr<ArrayString> arr = std::dynamic_pointer_cast<ArrayString>(result[columnName]);
                    arr->append(std::any_cast<std::string>(inputValue));
                } else if (type == "float" and  inputValue.type() == typeid(float)) {
                    std::shared_ptr<ArrayFloat> arr = std::dynamic_pointer_cast<ArrayFloat>(result[columnName]);
                    arr->append(std::any_cast<float>(inputValue));
                } else {
                    throw std::runtime_error("Unsupported data type for new column " + columnName);
                }
            }

            for(auto& [columnName, inputValue]: rightRow){
                if(columns.find(columnName) != columns.end()){
                    continue;
                }
                auto type = result[columnName]->typedef_();

                if (type == "int" and inputValue.type() == typeid(int)){
                    std::shared_ptr<ArrayInt> arr = std::dynamic_pointer_cast<ArrayInt>(result[columnName]);
                    arr->append(std::any_cast<int>(inputValue));

                } else if (type == "string" and inputValue.type() == typeid(std::string)) {
                    std::shared_ptr<ArrayString> arr = std::dynamic_pointer_cast<ArrayString>(result[columnName]);
                    arr->append(std::any_cast<std::string>(inputValue));
                } else if (type == "float" and  inputValue.type() == typeid(float)) {
                    std::shared_ptr<ArrayFloat> arr = std::dynamic_pointer_cast<ArrayFloat>(result[columnName]);
                    arr->append(std::any_cast<float>(inputValue));
                } else {
                    throw std::runtime_error("Unsupported data type for new column " + columnName);
                }
            }
        
        }
        else{
            
            for(auto& [columnName, inputValue]: leftRow){
                auto type = result[columnName]->typedef_();

                if (type == "int" and inputValue.type() == typeid(int)){
                    std::shared_ptr<ArrayInt> arr = std::dynamic_pointer_cast<ArrayInt>(result[columnName]);
                    arr->append(std::any_cast<int>(inputValue));

                } else if (type == "string" and inputValue.type() == typeid(std::string)) {
                    std::shared_ptr<ArrayString> arr = std::dynamic_pointer_cast<ArrayString>(result[columnName]);
                    arr->append(std::any_cast<std::string>(inputValue));
                } else if (type == "float" and  inputValue.type() == typeid(float)) {
                    std::shared_ptr<ArrayFloat> arr = std::dynamic_pointer_cast<ArrayFloat>(result[columnName]);
                    arr->append(std::any_cast<float>(inputValue));
                } else {
                    throw std::runtime_error("Unsupported data type for new column " + columnName);
                }
            }

            for(const auto& [name, _] : right_table){

                if(columns.find(name) != columns.end()){
                    continue;
                }
                std::string type = right_table[name]->typedef_();
                if(type == "int"){
                    
                    std::shared_ptr<ArrayInt> arr1 = std::dynamic_pointer_cast<ArrayInt>(result.table[name]);
                    arr1-> append(std::numeric_limits<int>::max());
                }
                else if(type == "string"){
                    
                    std::shared_ptr<ArrayString> arr1 = std::dynamic_pointer_cast<ArrayString>(result.table[name]);
                    arr1-> append("null");
                }
                else if(type == "float"){
                    
                    std::shared_ptr<ArrayFloat> arr1 = std::dynamic_pointer_cast<ArrayFloat>(result.table[name]);
                    arr1-> append(std::numeric_limits<float>::infinity());
                }
                else{
                    throw std::invalid_argument("the array is null");
                }
            }
        }
    }

    for(size_t i = 0; i < right.getNumRow(); ++i){

        if(matchedRightIdx.find(i) != matchedRightIdx.end()){
            continue;
        }

        std::vector<std::pair<std::string, std::any>> rightRow = right.row(i);

        for(auto& [columnName, inputValue]: rightRow){
            auto type = result[columnName]->typedef_();

            if (type == "int" and inputValue.type() == typeid(int)){
                std::shared_ptr<ArrayInt> arr = std::dynamic_pointer_cast<ArrayInt>(result[columnName]);
                arr->append(std::any_cast<int>(inputValue));

            } else if (type == "string" and inputValue.type() == typeid(std::string)) {
                std::shared_ptr<ArrayString> arr = std::dynamic_pointer_cast<ArrayString>(result[columnName]);
                arr->append(std::any_cast<std::string>(inputValue));
            } else if (type == "float" and  inputValue.type() == typeid(float)) {
                std::shared_ptr<ArrayFloat> arr = std::dynamic_pointer_cast<ArrayFloat>(result[columnName]);
                arr->append(std::any_cast<float>(inputValue));
            } else {
                throw std::runtime_error("Unsupported data type for new column " + columnName);
            }
        }

        for(const auto& [name, _] : left_table){

            if(columns.find(name) != columns.end()){
                continue;
            }
            std::string type = left_table[name]->typedef_();
            if(type == "int"){
                
                std::shared_ptr<ArrayInt> arr1 = std::dynamic_pointer_cast<ArrayInt>(result.table[name]);
                arr1-> append(std::numeric_limits<int>::max());
            }
            else if(type == "string"){
                
                std::shared_ptr<ArrayString> arr1 = std::dynamic_pointer_cast<ArrayString>(result.table[name]);
                arr1-> append("null");
            }
            else if(type == "float"){
                
                std::shared_ptr<ArrayFloat> arr1 = std::dynamic_pointer_cast<ArrayFloat>(result.table[name]);
                arr1-> append(std::numeric_limits<float>::infinity());
            }
            else{
                throw std::invalid_argument("the array is null");
            }
        }
    }

    return result;
}

Table Table::leftJoinProcessing(Table left, Table right, std::set<std::string> columns, std::unordered_map<std::string, size_t> rightIndexMap) {

    std::unordered_map<std::string, std::shared_ptr<Array>> left_table = left.getTable();
    std::unordered_map<std::string, std::shared_ptr<Array>> right_table = right.getTable();

    for(auto& col: columns){
        if(left_table.find(col) == left_table.end()){
            throw std::invalid_argument("one of the column doesn't exist in the left table");
        }
    }


    Table result(left);

    for(auto& [name, col]: right_table){
        if(columns.find(name) != columns.end()){
            continue;
        }

        auto type = col->typedef_();
        if (type == "int") {
            result.appendCol(name, std::make_shared<ArrayInt>());
        } else if (type == "string") {
            result.appendCol(name, std::make_shared<ArrayString>());
        } else if (type == "float") {
            result.appendCol(name, std::make_shared<ArrayFloat>());
        }
    }

    
    for(size_t i = 0; i < left.getNumRow(); ++i){
        std::string key = "";
        for(const auto& col : columns){
            std::string type = left_table[col]->typedef_();
            if(type == "int"){
                std::shared_ptr<ArrayInt> arr = std::dynamic_pointer_cast<ArrayInt>(left_table[col]);
                key += std::to_string((*arr)[i]) + "|";
            }
            else if(type == "string"){
                std::shared_ptr<ArrayString> arr = std::dynamic_pointer_cast<ArrayString>(left_table[col]);
                key += (*arr)[i] + "|";
            }
            else if(type == "float"){
                std::shared_ptr<ArrayFloat> arr = std::dynamic_pointer_cast<ArrayFloat>(left_table[col]);
                key += std::to_string((*arr)[i]) + "|";
            }
            else{
                throw std::invalid_argument("the array is null");
            }
        }

        if(rightIndexMap.find(key) != rightIndexMap.end()){
            size_t rightIndex = rightIndexMap[key];
            std::vector<std::pair<std::string, std::any>> rightRow = right.row(rightIndex);

            for(auto& [columnName, inputValue]: rightRow){
                if(columns.find(columnName) != columns.end()){
                    continue;
                }
                auto type = result[columnName]->typedef_();

                if (type == "int" and inputValue.type() == typeid(int)){
                    std::shared_ptr<ArrayInt> arr = std::dynamic_pointer_cast<ArrayInt>(result[columnName]);
                    arr->append(std::any_cast<int>(inputValue));

                } else if (type == "string" and inputValue.type() == typeid(std::string)) {
                    std::shared_ptr<ArrayString> arr = std::dynamic_pointer_cast<ArrayString>(result[columnName]);
                    arr->append(std::any_cast<std::string>(inputValue));
                } else if (type == "float" and  inputValue.type() == typeid(float)) {
                    std::shared_ptr<ArrayFloat> arr = std::dynamic_pointer_cast<ArrayFloat>(result[columnName]);
                    arr->append(std::any_cast<float>(inputValue));
                } else {
                    throw std::runtime_error("Unsupported data type for new column " + columnName);
                }
            }
        
        }
        else{
            
            for(const auto& [name, _] : right_table){

                if(columns.find(name) != columns.end()){
                    continue;
                }
                std::string type = right_table[name]->typedef_();
                if(type == "int"){
                    
                    std::shared_ptr<ArrayInt> arr1 = std::dynamic_pointer_cast<ArrayInt>(result.table[name]);
                    arr1-> append(std::numeric_limits<int>::max());
                }
                else if(type == "string"){
                    
                    std::shared_ptr<ArrayString> arr1 = std::dynamic_pointer_cast<ArrayString>(result.table[name]);
                    arr1-> append("null");
                }
                else if(type == "float"){
                    
                    std::shared_ptr<ArrayFloat> arr1 = std::dynamic_pointer_cast<ArrayFloat>(result.table[name]);
                    arr1-> append(std::numeric_limits<float>::infinity());
                }
                else{
                    throw std::invalid_argument("the array is null");
                }
            }
        }
    }

    for(auto& [name, _]: right_table){
        if(result[name]->getSize()!= left.getNumRow()){
            std::cout <<"\nCur left join:\n" << result<< std::endl;
            throw std::invalid_argument("there was an error with the join operation");
        }
    }

    return result;
}


Table Table::leftJoin(Table left, Table right, std::set<std::string> columns){
    
    std::unordered_map<std::string, size_t> rightIndexMap = _createHashmapFromTable(right, columns);
    
    // Perform the join
    return leftJoinProcessing(left, right, columns, rightIndexMap);
}

Table Table::rightJoin( Table left, Table right, std::set<std::string> columns){

    return leftJoin(right, left, columns);
}

int Table::aggregateColumn(std::string& columnName, std::function<int(int, int)> aggFunc, int initialValue){
    
    if (table.find(columnName) == table.end()) {
        throw std::invalid_argument("Column not found");
    }
    std::shared_ptr<ArrayInt> arr1 = std::dynamic_pointer_cast<ArrayInt>(table[columnName]);
    
    return arr1->aggregate(aggFunc, initialValue);
}

std::string Table::aggregateColumn( std::string& columnName, std::function<std::string(std::string, std::string)> aggFunc, std::string initialValue){
    if (table.find(columnName) == table.end()) {
        throw std::invalid_argument("Column not found");
    }
    std::shared_ptr<ArrayString> arr1 = std::dynamic_pointer_cast<ArrayString>(table[columnName]);
    
    return arr1-> aggregate(aggFunc, initialValue);
}

float Table::aggregateColumn( std::string& columnName, std::function<float(float, float)> aggFunc, float initialValue){
    if (table.find(columnName) == table.end()) {
        throw std::invalid_argument("Column not found");
    }
    std::shared_ptr<ArrayFloat> arr1 = std::dynamic_pointer_cast<ArrayFloat>(table[columnName]);
    
    return arr1-> aggregate(aggFunc, initialValue);
}


Table Table::aggregateColumn( std::vector<std::string>& columnNames, std::function<int(int, int)> aggFunc, int initialValue){
    Table result;
    for ( auto& columnName : columnNames) {
        std::shared_ptr<ArrayInt> arr1;
        arr1->append(aggregateColumn(columnName, aggFunc, initialValue));
        result[columnName] = arr1;
    }
    return result;
}

Table Table::aggregateColumn( std::vector<std::string>& columnNames, std::function<std::string(std::string, std::string)> aggFunc, std::string initialValue){
    Table result;
    for ( auto& columnName : columnNames) {
        std::shared_ptr<ArrayString> arr1;
        arr1->append(aggregateColumn(columnName, aggFunc, initialValue));
        result[columnName] = arr1;
    }
    return result;
}

Table Table::aggregateColumn( std::vector<std::string>& columnNames, std::function<float(float, float)> aggFunc, float initialValue){
    Table result;
    for ( auto& columnName : columnNames) {
        std::shared_ptr<ArrayFloat> arr1;
        arr1->append(aggregateColumn(columnName, aggFunc, initialValue));
        result[columnName] = arr1;
    }
    return result;
}

std::unordered_map<std::string, std::shared_ptr<Array>> Table::getTable() const{
    return table;
}

size_t Table::getNumColumn(){
    return numColumn;
}

size_t Table::getNumRow(){
    return numRow;
}

std::shared_ptr<Array>& Table::operator[](const std::string& columnName) {
    
    if (table.find(columnName) == table.end()) {
        table[columnName] = std::make_shared<ArrayInt>();
        ++numColumn;
        // throw std::invalid_argument("the column is not in the table");
    }
    return table[columnName];
}

std::ostream& operator<<(std::ostream& os, const Table& table ) {
    
    for (const auto& [name, col] : table.getTable()) {
        os << name << ": ";
        if ("int" == col->typedef_()) {
            std::shared_ptr<ArrayInt> intCol = std::dynamic_pointer_cast<ArrayInt>(col);
            os << (*intCol) << std::endl;
        }
        else if ("string" == col->typedef_()) {
            std::shared_ptr<ArrayString> strCol = std::dynamic_pointer_cast<ArrayString>(col);
            os << (*strCol) << std::endl;
        }
        else if ("float" == col->typedef_()) {
            std::shared_ptr<ArrayFloat> flCol = std::dynamic_pointer_cast<ArrayFloat>(col);
            os << (*flCol) << std::endl;
        }
    }
    return os;
}

bool Table::operator==(Table& t){
    auto otherTable = t.getTable();

    for(auto& [name, col]: getTable()){
        if(otherTable.find(name) == otherTable.end()){
            return false;
        }
        if ("int" == col->typedef_()) {
            auto intCol = std::dynamic_pointer_cast<ArrayInt>(col);
            auto otherCol = std::dynamic_pointer_cast<ArrayInt>(otherTable[name]);
            if(*intCol != *otherCol){
                return false;
            }
        }
        else if ("string" == col->typedef_()) {
            auto strCol = std::dynamic_pointer_cast<ArrayString>(col);
            auto otherCol = std::dynamic_pointer_cast<ArrayString>(otherTable[name]);
            if(*strCol != *otherCol){
                return false;
            }
        }
        else if ("float" == col->typedef_()) {
            auto flCol = std::dynamic_pointer_cast<ArrayFloat>(col);
            auto otherCol = std::dynamic_pointer_cast<ArrayFloat>(otherTable[name]);
            if(*flCol != *otherCol){
                return false;
            }
        }
    }

    return true;
}

bool Table::operator!=(Table& t){
    return !(*this == t);
}