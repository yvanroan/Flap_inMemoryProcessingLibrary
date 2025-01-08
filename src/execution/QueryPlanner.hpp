
#ifndef QUERY_HPP
#define qUERY_HPP

#include <vector>
#include <string>
#include <variant>
#include <optional>
#include <functional>

class Array;
class Table;

using ArrayType = std::variant<int, float, double, std::string>;
using ParamsType = std::variant<int, std::string, float, double, Table*, Array* >;

struct QueryNode {
        std::variant<Array*,Table*> subject;
        std::string operatorType;   
        std::vector<ParmasType>;
};

class QueryPlanner{

    private:
        std::vector<QueryNode> queryPlan;
        static QueryPlanner instance;

        static const std::unordered_map<std::string, std::vector<std::pair<std::string, std::type_index>>> operatorMetadata = {
            //array
            {"append", {
                {"additional_data", typeid(std::optional<ArrayType>)}
            }},

            {"extend", {
                {"extraArray", typeid(Array*)}
            }},

            {"appendNull", {} },

            {"map", {
                {"mapFunc", typeid(std::function<std::optional<ArrayType>(std::optional<ArrayType>)>)}
            }},
            
            {"filter", {
                {"filterFunc", typeid(std::function<bool(std::optional<ArrayType>)>)}
            }},

            {"size", {} },

            {"fillNulls", {
                {"val", typeid(ArrayType)}
            } },

            {"isNullAt", {
                {"idx", typeid(int)}
            }},

            {"filteredIndex", {
                {"filterFunc", typeid(std::function<bool(std::optional<ArrayType>)>)}
            }},

            {"aggregate", {
                {"aggFunc", typeid(std::function<ArrayType(ArrayType, ArrayType)>)},
                {"initialValue", typeid(ArrayType)}
            }},

            //table
            {"appendCol", {
                {"columnName", typeid(std::string)},
                {"inputArray", typeid(Array*)}
            }},

            {"appendRow", {
                {"newEntry", typeid(std::vector<>)},
            }},

            {"RENAME", {
                {"oldName", typeid(std::string)},
                {"newName", typeid(std::string)}
            }},

            {"dataAt", {
                {"columnName", typeid(std::string)},
                {"rowNumber", typeid(int)},
            }},

            {"column", {
                {"columnName", typeid(std::string)}
            }},

            {"row", {
                {"rowNumber", typeid(int)},
            }},

            {"rows", {
                {"indexes", typeid(std::vector<std::int>)}
            }},

            {"rows", {
                {"startRowNumber", typeid(int)},
                {"endRowNumber", typeid(int)},
            }},

            {"filterRow", {
                {"filterRowFunc", typeid(std::function<bool(ArrayType)>)}
            }},

            {"filterCol", {
                {"columnNames", typeid(std::vector<std::string>)},
                {"remove", typeid(bool)},
            }},

            {"map", {
                {"mapFunc", typeid(std::function<ArrayType(ArrayType)>)},
                {"columnName", typeid(std::string)},
            }},

            {"aggregateColumn", {
                {"columnName", typeid(std::string)},
                {"aggFunc", typeid(std::function<ArrayType(ArrayType, ArrayType)>)},
                {"initialValue", typeid(ArrayType)}
            }},

            {"aggregateColumns", {
                {"columnNames", typeid(std::vector<std::string>)},
                {"aggFunc", typeid(std::function<ArrayType(ArrayType, ArrayType)>)},
                {"initialValue", typeid(ArrayType)}
            }},

            {"innerJoin", {
                {"left", typeid(Table*)},
                {"right", typeid(Table*)},
                {"keyColumns", typeid(std::vector<std::string>)}
            }},

            {"outerJoin", {
                {"left", typeid(Table*)},
                {"right", typeid(Table*)},
                {"keyColumns", typeid(std::vector<std::string>)}
            }},

            {"leftJoin", {
                {"left", typeid(Table*)},
                {"right", typeid(Table*)},
                {"keyColumns", typeid(std::vector<std::string>)}
            }},

            {"rightJoin", {
                {"left", typeid(Table*)},
                {"right", typeid(Table*)},
                {"keyColumns", typeid(std::vector<std::string>)}
            }},

        };

        QueryPlanner() = default;

    public: 
        static QueryPlanner getInstance();

        QueryNode addNode(const std::variant<Array*,Table*>& subject
             const std::string& operatorType,
             const std::vector<ParamsType>& columns = {});

        std::vector<ParamsType> validateInputsAndGetObjects(const std::string& operatorType, const std::vector<std::any>& params); 

        void validatePlan();
        void executePlan();

}

#endif