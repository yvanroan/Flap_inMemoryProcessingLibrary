#include "QueryPlanner.hpp"


QueryPlanner QueryPlanner::instance;

static QueryPlanner QueryPlanner::getInstance(){
    return instance;
}

QueryNode addNode(const std::variant<Array*, Table*>& subject,
                  const std::string& operatorType,
                  const std::vector<ParamsType>& params = {}) {
    return QueryNode {
        .subject = subject,
        .operatorType = operatorType,
        .params = params
    };
}

