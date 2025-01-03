#include <string>
#include <sstream>
#include <vector>
#include <std::optional>
#include <unordered_map>
#include "schema.pb.h"


class Array;
class Table;

class Deserializer {
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
                result.sequence.push_back(std::nullopt); // Reconstruct null
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
    

    Table jsonToTable(const std::string& jsonString) {
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

}

