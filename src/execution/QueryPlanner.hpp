
#ifndef QUERY_HPP
#define qUERY_HPP

#include <vector>
#include <string>
#include <variant>

class Array;
class Table;

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

        QueryPlanner() = default;

    public: 
        static QueryPlanner getInstance();

        QueryNode addNode(const std::variant<Array*,Table*>& subject
             const std::string& operatorType,
             const std::vector<ParamsType>& columns = {});

        

}

#endif