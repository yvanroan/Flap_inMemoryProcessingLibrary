
#ifndef QUERY_HPP
#define qUERY_HPP

#include <vector>
#include <string>
#include <variant>
#include <optional>
#include <functional>

class Array;
class Table;

using ArrayType = std::variant< size_t, int, float, double, std::string>;

using ParamsType = std::variant<size_t, int, std::string, float, double, Table*, Array*, 
                                std::function<std::optional<ArrayType>(std::optional<ArrayType>)>,
                                std::function<bool(std::optional<ArrayType>)>,
                                std::function<ArrayType(ArrayType, ArrayType)>>;

using FuncReturnType = std::variant<std::vector<std::optional<ArrayType>>, std::vector<std::vector<std::optional<ArrayType>>>, Array*, Table* ,std::vector<size_t>, ArrayType, std::optional<ArrayType>>;

struct QueryNode {
        std::variant<Array*,Table*> subject;
        std::string operatorType;   
        int return_type;
        std::vector<ParmasType>;
};

class QueryPlanner{

    private:
        std::vector<QueryNode> queryPlan;
        static QueryPlanner instance;

        static const std::unordered_map<std::string, std::vector<std::pair<std::string, std::type_index>>> operatorTableMetadata = {
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

            {"column", {
                {"columnName", typeid(std::string)}
            }},

            {"row", {
                {"rowNumber", typeid(int)},
            }},

            {"rows_vector", {
                {"indexes", typeid(std::vector<std::int>)}
            }},

            {"rows_idx", {
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

            {"aggregateColumn", {
                {"columnName", typeid(std::string)},
                {"aggFunc", typeid(std::function<ArrayType(ArrayType, ArrayType)>)},
                {"initialValue", typeid(ArrayType)}
            }},

            {"aggregateColumns", {
                {"columnNames", typeid(std::vector<std::string>)},
                {"aggFunc", typeid(std::function<ArrayType(ArrayType, ArrayType)>)},
                {"initialValue", typeid(ArrayType)}
            }}

        };

        static const std::unordered_map<std::string, std::vector<std::pair<std::string, std::type_index>>> operatorArrayMetadata ={
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

            {"fillNulls", {
                {"val", typeid(ArrayType)}
            } },

            {"isNull", {
                {"idx", typeid(int)}
            }},

            {"filteredIndex", {
                {"filterFunc", typeid(std::function<bool(std::optional<ArrayType>)>)}
            }},

            {"removeByIndex", {
                {"indexes", typeid(std::vector<size_t>)}
            }},

            {"aggregate", {
                {"aggFunc", typeid(std::function<ArrayType(ArrayType, ArrayType)>)},
                {"initialValue", typeid(ArrayType)}
            }}
        };

        QueryPlanner() = default;

    public: 
        static QueryPlanner getInstance();

        QueryNode addNode(const std::variant<Array*,Table*>& subject
             const std::string& operatorType,
             int return_type,
             const std::vector<ParamsType>& params = {});

        std::vector<std::variant<Array*, Table*>> validateInputsAndGetObjects(const std::string& operatorType, const std::vector<std::any>& params, bool isArray); 
        void validateNode(QueryNode node, bool isArray);
        FuncReturnType executeNode(QueryNode node, bool isArray);

        FuncReturnType executePlan();

}

#endif