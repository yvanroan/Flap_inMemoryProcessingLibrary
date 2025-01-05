#include <string>
#include <sstream>
#include <vector>
#include <optional>
#include <unordered_map>
#include "schema.pb.h"
#include "Array.hpp"
#include "Table.hpp"


class Serializer {
public:

    //helper
    Array createArray(vector<std::string> data){

        Array array;
        const std::string& type = data[0];
        for (size_t i = 1; i < data.size(); ++i) {
            if (data[i]=="null") {
                array.append(std::nullopt); 
            } else if (type == "int") {
                array.append(std::stoi(data[i]));
            } else if (type == "float") {
                array.append(std::stof(data[i]));
            } else if (type == "double") {
                array.append(std::stod(data[i]));
            } else if (type == "string") {
                array.append(data[i]);
            } else {
                throw std::invalid_argument("Unsupported type: " + type);
            }
        }
        return array;
    }


    std::vector<std::string> csvToVector(const std::string& csvLine) {
        std::vector<std::string> result;
        std::stringstream ss(csvLine);
        std::string cell;

        while (std::getline(ss, cell, ',')) {
            result.push_back(cell);
        }

        return result;
    }

    void populateColumns(
        const std::vector<std::string>& types,
        const std::vector<std::string>& columnNames,
        const std::vector<std::string>& rowValues,
        Table t) {
        
        for (size_t i = 0; i < columnNames.size(); ++i) {
            const auto& cell = rowValues[i];
            if (cell.empty()) {
                t[columnNames[i]].append(std::nullopt); // Null value
            } else if (types[i] == "int") {
                t[columnNames[i]].append(std::make_optional<std::int>(std::stoi(cell)));
            } else if (types[i] == "float") {
                t[columnNames[i]].append(std::make_optional<std::float>(std::stof(cell)));
            } else if (types[i] == "double") {
                t[columnNames[i]].append(std::make_optional<std::double>(std::stod(cell)));
            } else if (types[i] == "string") {
                t[columnNames[i]].append(std::make_optional<std::string>(cell));
            } else {
                throw std::invalid_argument("Unsupported type: " + types[i]);
            }
        }
    }

    std::vector<std::string> parseJsonArray(const std::string& jsonArray) {
    
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

    ////

    
    static Array binaryToArray(const std::string& binaryData) {
        ArrayProto arrayProto;
        arrayProto.ParseFromString(binaryData);

        Array result;
        for (const auto& protoValue : arrayProto.sequence()) {
            if (protoValue.is_null()) {
                result.sequence.push_back(std::nullopt);
            } else {
                if (protoValue.has_int_val()) {
                    result.sequence.emplace_back(protoValue.int_val());
                } else if (protoValue.has_float_val()) {
                    result.sequence.emplace_back(protoValue.float_val());
                } else if (protoValue.has_double_val()) {
                    result.sequence.emplace_back(protoValue.double_val());
                } else if (protoValue.has_string_val()) {
                    result.sequence.emplace_back(protoValue.string_val());
                }
            }
        }

        return result;
    }
    
    static Array csvToArray(const std::string& csvData) {

        vector<std::string> data = csvToVector(csvData);

        if (data.empty()) {
            throw std::invalid_argument("CSV line is empty.");
        }

        return createArray(data);
    }

    static Array jsonToArray(const std::string& jsonData) {
        
        vector<std::string> data = parseJsonArray(jsonData);
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
            auto rowValues = splitCsvLine(line);
            if (rowValues.size() != columnNames.size()) {
                throw std::runtime_error("Row size does not match column size.");
            }
            populateColumns(types, columnNames, rowValues, t);
        }

        return t;
    }
    

    static Table jsonToTable(const std::string& jsonString) {
        json parsedJson = json::parse(jsonString);

        if (!parsedJson.is_object()) {
            throw std::invalid_argument("Input JSON is not a valid object.");
        }
        
        Table t;

        for (const auto& [key, value] : parsedJson.items()) {
            if (!value.is_array()) {
                throw std::invalid_argument("JSON value for column is not an array.");
            }
            Array array = jsonToArray(value.dump());
            t[key] = array;
        }

        return t;
    }
    
    static std::string arrayToCsv(const Array& array) {
        std::ostringstream csv;
        csv << array.typedef_() << ",";
        for (const auto& item : array.getSequence()) {
            if (item.has_value()) {
                csv << item.value();
            }
            csv << ","; 
        }
        std::string result = csv.str();
        if (!result.empty() && result.back() == ',') {
            result.pop_back();
        }
        return result;
    }

    
    static std::string tableToCsv(const Table& table) {
        std::ostringstream csv;

        
        for (const auto& [name, _] : table.getTable()) {
            csv << name << ",";
        }
        csv << "\n";

        size_t numRows = table.getNumRows();

        for (size_t i = -1; i < numRows; ++i) {
            for (const auto& [_, array] : table.getTable()) {
                const auto& sequence = array.getSequence();
                if(i==-1){
                    csv << array.typedef_();
                }
                
                if (i < sequence.size() && sequence[i].has_value()) {
                    csv << sequence[i].value();
                }
                csv << ",";
            }
            csv << "\n";
        }

        return csv.str();
    }

    std::string tableToBinary(const Table& table) const {
        TableProto tableProto;
        tableProto.set_numrow(table.getNumRow());
        tableProto.set_numcolum(table.getNumColumn());

        for (const auto& [columnName, array] : table.getTable()) {
            ArrayProto arrayProto;
            std::string arrayBinary = array.toBinary();
            arrayProto.ParseFromString(arrayBinary);
            (*tableProto.mutable_columns())[columnName] = arrayProto;
        }

        std::string binaryData;
        tableProto.SerializeToString(&binaryData);
        return binaryData;
    }

    std::string arrayToBinary(const Array& array) const{
        ArrayProto arrayProto;
        for (const auto& element : array.getSequence()) {
            Value* value = arrayProto.add_sequence();
            if (!element.has_value()) {
                value->set_is_null(true); 
            } else {
                value->set_is_null(false); 
                std::visit([value](auto&& val) {
                    using ValueType = std::decay_t<decltype(val)>;
                    if constexpr (std::is_same_v<ValueType, int>) {
                        value->set_int_val(val);
                    } else if constexpr (std::is_same_v<ValueType, float>) {
                        value->set_float_val(val);
                    } else if constexpr (std::is_same_v<ValueType, double>) {
                        value->set_double_val(val);
                    } else if constexpr (std::is_same_v<ValueType, std::string>) {
                        value->set_string_val(val);
                    }
                }, element.value());
            }
        }
        std::string binaryData;
        arrayProto.SerializeToString(&binaryData);
        return binaryData;
    }

    std::string arrayToJson(const Array& array) const {
        std::ostringstream json;
        json << "[";
        
        json << array.typedef_();

        for (size_t i = 0; i < sequence.size(); ++i) {
            json << ",";
            if (sequence[i].has_value()) {
                std::visit([&json](auto&& value) {
                    using ValueType = std::decay_t<decltype(value)>;
                    if constexpr (std::is_arithmetic_v<ValueType>) {
                        json << value;
                    } else if constexpr (std::is_same_v<ValueType, std::string>) {
                        json << "\"" << value << "\"";
                    }
                }, sequence[i].value());
            } else {
                json << "null";
            }
        }

        json << "]";
        return json.str();
    }


    std::string tableToJson(const Table& table) const {
        std::ostringstream json;
        size_t i = 0;
        json << "{\n";
        

        for (const auto& [column, array]: table.getTable()) {
            json << "\"" << column << "\"";
            json << " : " << arrayToJson(array); 

            if ( i != table.getNumColumn() - 1) {
                json << ",\n";
            }
            i++;
        }

        json << "\n}";
        return json.str();
    }

};

