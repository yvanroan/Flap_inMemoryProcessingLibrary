#include <string>
#include <sstream>
#include <vector>
#include <optional>
#include <unordered_map>
// #include "schema.pb.h"
#include "Table.hpp"
#include "ArrayFloat.hpp"
#include "ArrayInt.hpp"
#include "ArrayString.hpp"

class Serializer {
public:

    //helper
    static std::shared_ptr<Array> createArray(std::vector<std::string> data){

        const std::string& type = data[0];
        std::shared_ptr<Array> arr;
        
        if (type == "int") {
            std::shared_ptr<ArrayInt> array= std::make_shared<ArrayInt>();
            for (size_t i = 1; i < data.size(); ++i) {
                array->append(std::stoi(data[i]));
            }
            arr = array;
        } else if (type == "float") {
            std::shared_ptr<ArrayFloat> array= std::make_shared<ArrayFloat>();
            for (size_t i = 1; i < data.size(); ++i) {
                array->append(std::stof(data[i]));
            }
            arr = array;
        } else if (type == "string") {
            std::shared_ptr<ArrayString> array= std::make_shared<ArrayString>();
            for (size_t i = 1; i < data.size(); ++i) {
                array->append(data[i]);
            }
            arr = array;
        } else {
            throw std::invalid_argument("Unsupported type: " + type);
        }
        return arr;
    }


    static std::vector<std::string> csvToVector(const std::string& csvLine) {
        std::vector<std::string> result;
        std::stringstream ss(csvLine);
        std::string cell;

        while (std::getline(ss, cell, ',')) {
            result.push_back(cell);
        }

        return result;
    }

    static void populateColumns(
        const std::vector<std::string>& types,
        const std::vector<std::string>& columnNames,
        const std::vector<std::string>& rowValues,
        Table t) {

        if(types.size() != columnNames.size() or columnNames.size() != rowValues.size()){
            throw std::invalid_argument("Internal error during csv to Table conversion. The parameters do not have the same size");
        }
        
        for (size_t i = 0; i < columnNames.size(); ++i) {
            const auto& cell = rowValues[i];
            if (types[i] == "int") {
                if(t.getNumRow()==0){
                    auto arr = std::make_shared<ArrayInt>();
                    arr->append(std::stoi(cell));
                    t[columnNames[i]]=arr;
                }
                else{
                    std::shared_ptr<ArrayInt> arr = std::dynamic_pointer_cast<ArrayInt>(t[columnNames[i]]);
                    arr->append(std::stoi(cell));
                    t[columnNames[i]]=arr;
                }
                
                
            } else if (types[i] == "float") {
                if(t.getNumRow()==0){
                    auto arr = std::make_shared<ArrayFloat>();
                    arr->append(std::stof(cell));
                    t[columnNames[i]]=arr;
                }
                else{
                    std::shared_ptr<ArrayFloat> arr = std::dynamic_pointer_cast<ArrayFloat>(t[columnNames[i]]);
                    arr->append(std::stof(cell));
                    t[columnNames[i]]=arr;
                }
            } else if (types[i] == "string") {
                if(t.getNumRow()==0){
                    auto arr = std::make_shared<ArrayString>();
                    arr->append(cell);
                    t[columnNames[i]]=arr;
                }
                else{
                    std::shared_ptr<ArrayString> arr = std::dynamic_pointer_cast<ArrayString>(t[columnNames[i]]);
                    arr->append(cell);
                    t[columnNames[i]]=arr;
                }
            } else {
                throw std::invalid_argument("Unsupported type: " + types[i]);
            }
        }
    }

    static std::vector<std::string> parseJsonArray(const std::string& jsonArray) {
    
        std::string content = jsonArray;
        content.erase(std::remove(content.begin(), content.end(), '['), content.end());
        content.erase(std::remove(content.begin(), content.end(), ']'), content.end());

        
        std::vector<std::string> result;
        std::stringstream ss(content);
        std::string value;

        while (std::getline(ss, value, ',')) {
            
            value.erase(0, value.find_first_not_of(" \t\n\r"));
            value.erase(value.find_last_not_of(" \t\n\r") + 1);
            result.push_back(value);
        }

        return result;
    }

    static std::pair<std::vector<std::string>, std::vector<std::string>> jsonToVectors(const std::string& json) {
        std::vector<std::string> columnNames;
        std::vector<std::string> columnData;

        std::istringstream ss(json);
        std::string line;

        while (std::getline(ss, line, '\n')) {
            size_t colonPos = line.find(':');
            if (colonPos != std::string::npos) {
                std::string columnName = line.substr(1, colonPos - 3); // Extract column name (without quotes)
                std::string arrayData = line.substr(colonPos + 1); // Extract array data
                arrayData.erase(arrayData.find_last_of(",\n")); // Remove trailing comma or newline

                columnNames.push_back(columnName);
                columnData.push_back(arrayData);
            }
        }

        return {columnNames, columnData};
    }
    ////

    static std::shared_ptr<Array> csvToArray(const std::string& csvData) {

        std::vector<std::string> data = csvToVector(csvData);

        if (data.empty()) {
            throw std::invalid_argument("CSV line is empty.");
        }

        return createArray(data);
    }

    static std::shared_ptr<Array> jsonToArray(const std::string& jsonData) {
        
        std::vector<std::string> data = parseJsonArray(jsonData);
        if (data.empty()) {
            throw std::invalid_argument("CSV line is empty.");
        }
        return createArray(data);
    }


    static Table csvToTable(const std::string& csvData) {

        std::istringstream csv(csvData);
        std::string line;

        if (!std::getline(csv, line)) {
            throw std::runtime_error("CSV is empty or missing a header.");
        }
        auto columnNames = csvToVector(line);

        if (!std::getline(csv, line)) {
            throw std::runtime_error("CSV is missing type definitions.");
        }
        auto types = csvToVector(line);

        if (columnNames.size() != types.size()) {
            throw std::runtime_error("Mismatch between column names and type definitions.");
        }

        Table t;

        while (std::getline(csv, line)) {
            auto rowValues = csvToVector(line);
            if (rowValues.size() != columnNames.size()) {
                throw std::runtime_error("Row size does not match column size.");
            }
            populateColumns(types, columnNames, rowValues, t);
        }

        return t;
    }

    static Table jsonToTable(const std::string& jsonData) {
        auto data =  jsonToVectors(jsonData);
        std::vector<std::string> colNames = data.first;
        std::vector<std::string> arrays = data.second;
        Table t;
        for(int i =0; i<colNames.size(); i++){
            t.appendCol(colNames[i], jsonToArray(arrays[i]));
        }

        return t;
    } 
    
    static std::string arrayToCsv(std::shared_ptr<Array> col){
        std::ostringstream csv;
        std::string type = col->typedef_();
        csv << type<< ",";
        
        if(type == "int"){
            std::shared_ptr<ArrayInt> arr = std::dynamic_pointer_cast<ArrayInt>(col);
            for (const auto& item : arr->getArray()) {
                csv << item << ",";
            }
        }
        else if(type == "string"){
            std::shared_ptr<ArrayString> arr = std::dynamic_pointer_cast<ArrayString>(col);
            for (const auto& item : arr->getArray()) {
                csv << item << ",";
            }
        }
        else if(type == "float"){
            std::shared_ptr<ArrayFloat> arr = std::dynamic_pointer_cast<ArrayFloat>(col);
            for (const auto& item : arr->getArray()) {
                csv << item << ",";
            }
        }
        else{
            throw std::invalid_argument("the array is null");
        }

        std::string result = csv.str();
        if (!result.empty() && result.back() == ',') {
            result.pop_back();
        }
        return result;
    }

    
    static std::string tableToCsv(Table& table) {
        std::ostringstream csv;

        auto table_map = table.getTable();
        for(const auto& [name, _] : table_map) {
            csv << name << ",";
        }
        csv <<"\n";

        for(const auto& [_, col] : table_map) {
            csv << col->typedef_() << ",";
        }

        size_t numRows = table.getNumRow();

        for (size_t i = 0; i < numRows; ++i) {
            std::vector<std::pair<std::string, std::any>> tableRow = table.row(i);
            
            csv << "\n";
            for(auto& [columnName, inputValue]: tableRow){
                auto type = table_map[columnName]->typedef_();

                if (type == "int" and inputValue.type() == typeid(int)){
                    csv << std::any_cast<int>(inputValue)<< "," ;

                } else if (type == "string" and inputValue.type() == typeid(std::string)) {
                    csv << std::any_cast<std::string>(inputValue)<< "," ;
                } else if (type == "float" and  inputValue.type() == typeid(float)) {
                    csv << std::any_cast<float>(inputValue)<< ",";
                } else {
                    throw std::runtime_error("Unsupported data type for new column " + columnName);
                }
            }
            
        }

        return csv.str();
    }

    
    static std::string arrayToJson( std::shared_ptr<Array> col) {
        std::ostringstream json;
        json << "[";
        
        std::string type = col->typedef_();

        json << type;

        if(type == "int"){
            std::shared_ptr<ArrayInt> arr = std::dynamic_pointer_cast<ArrayInt>(col);
            for (const auto& item : arr->getArray()) {
                json << "," << item ;
            }
        }
        else if(type == "string"){
            std::shared_ptr<ArrayString> arr = std::dynamic_pointer_cast<ArrayString>(col);
            for (const auto& item : arr->getArray()) {
                json << "," << item;
            }
        }
        else if(type == "float"){
            std::shared_ptr<ArrayFloat> arr = std::dynamic_pointer_cast<ArrayFloat>(col);
            for (const auto& item : arr->getArray()) {
                json << "," << item;
            }
        }
        else{
            throw std::invalid_argument("the array is null");
        }

        json << "]";
        return json.str();
    }


    static std::string tableToJson( Table& table) {
        std::ostringstream json;
        size_t i = 0;
        json << "{\n";
        

        for ( auto& [column, array]: table.getTable()) {
            json << "\"" << column << "\"";
            json << " : " << arrayToJson(array) << ",\n"; 
        }

        json << "}";
        return json.str();
    }

    // static Array binaryToArray(const std::string& binaryData) {
    //     ArrayProto arrayProto;
    //     arrayProto.ParseFromString(binaryData);

    //     Array result;
    //     for (const auto& protoValue : arrayProto.sequence()) {
    //         if (protoValue.is_null()) {
    //             result.appendNull();
    //         } else {
    //             if (protoValue.has_int_val()) {
    //                 result.append(protoValue.int_val());
    //             } else if (protoValue.has_float_val()) {
    //                 result.append(protoValue.float_val());
    //             } else if (protoValue.has_double_val()) {
    //                 result.append(protoValue.double_val());
    //             } else if (protoValue.has_string_val()) {
    //                 result.append(protoValue.string_val());
    //             }
    //         }
    //     }

    //     return result;
    // }

    // std::string tableToBinary(Table& table) const {
    //     TableProto tableProto;
    //     tableProto.set_numrow(table.getNumRow());
    //     tableProto.set_numcolum(table.getNumColumn());

    //     for (auto& [columnName, array] : table.getTable()) {
    //         ArrayProto arrayProto;
    //         std::string arrayBinary = arrayToBinary(array);
    //         arrayProto.ParseFromString(arrayBinary);
    //         (*tableProto.mutable_columns())[columnName] = arrayProto;
    //     }

    //     std::string binaryData;
    //     tableProto.SerializeToString(&binaryData);
    //     return binaryData;
    // }

    // std::string arrayToBinary( std::shared_ptr<Array> col) const{
    //     ArrayProto arrayProto;
    //     for( auto& element : array.getArray()) {
    //         Value* value = arrayProto.add_sequence();
    //         if (!element.has_value()) {
    //             value->set_is_null(true); 
    //         } else {
    //             value->set_is_null(false); 
    //             std::visit([value](auto&& val) {
    //                 using ValueType = std::decay_t<decltype(val)>;
    //                 if constexpr (std::is_same_v<ValueType, int>) {
    //                     value->set_int_val(val);
    //                 } else if constexpr (std::is_same_v<ValueType, float>) {
    //                     value->set_float_val(val);
    //                 } else if constexpr (std::is_same_v<ValueType, double>) {
    //                     value->set_double_val(val);
    //                 } else if constexpr (std::is_same_v<ValueType, std::string>) {
    //                     value->set_string_val(val);
    //                 }
    //             }, element.value());
    //         }
    //     }
    //     std::string binaryData;
    //     arrayProto.SerializeToString(&binaryData);
    //     return binaryData;
    // }

};

