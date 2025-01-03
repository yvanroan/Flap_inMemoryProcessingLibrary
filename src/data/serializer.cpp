#include <string>
#include <sstream>
#include <vector>
#include <optional>
#include <unordered_map>
#include "schema.pb.h"


class Array;

class Table;


class Serializer {
public:

    
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

