#include <stdexcept>
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

std::vector<ParamsType> validateInputsAndGetObjects(const std::string& operatorType, const std::vector<std::any>& params) {
    const auto& metadata = operatorMetadata.at(operatorType);

    if (params.size() != metadata.size()) {
        throw std::runtime_error("Incorrect number of parameters for operator: " + operatorType);
    }

    std::vector<ParamsType> objects;

    for (size_t i = 0; i < params.size(); ++i) {
        const auto& [paramName, paramType] = metadata[i];
        
        if (params[i].type() != paramType) {
            throw std::runtime_error("Invalid type for parameter '" + paramName +
                                     "'. Expected " + paramType.name() +
                                     " but got " + params[i].type().name());
        }

        if (paramType == typeid(Array*) || paramType == typeid(Table*)) {
            objects.emplace_back(params[i]);
        }
    }

    return objects;
}

void validateQuery() {
    for (const auto& node : queryPlan) {
        std::visit([](auto* obj) {
            if (!obj) throw std::runtime_error("Subject is null.");
            if (!MemoryManager::getInstance().isInMemory(obj->getId())) {
                DiskSpiller::getInstance().load(obj->getId()); // Load if spilled
            }
        }, node.subject);
        
        for (const auto& param : validateInputsAndGetObjects(node.operatorType, node.params)) {
            std::visit([](auto* obj) {
                if (obj && !MemoryManager::getInstance().isInMemory(obj->getId())) {
                    DiskSpiller::getInstance().load(obj->getId()); // Load if spilled
                }
            }, param);
        }
    }
}

void executePlan() {
    for (const auto& node : queryPlan) {
        std::visit([&node](auto* obj) {

            const auto& params = node.params;

            if (node.operatorType == "appendCol") {
                obj->appendCol(std::get<std::string>(params[0]), std::get<Array*>(params[1]));
            } else if (node.operatorType == "append") {
                obj->append(std::get<std::optional<ArrayType>>(params[0]));
            } else if (node.operatorType == "appendNull") {
                obj->appendNull();
            } else if (node.operatorType == "extend") {
                obj->extend(std::get<Array*>(params[0]));
            } else if (node.operatorType == "map") {
                obj->map(std::get<std::function<std::optional<ArrayType>(std::optional<ArrayType>)>>(params[0]));
            } else if (node.operatorType == "filter") {
                obj->filter(std::get<std::function<bool(std::optional<ArrayType>)>>(params[0]));
            } else if (node.operatorType == "fillNulls") {
                obj->fillNulls(std::get<ArrayType>(params[0]));
            } else if (node.operatorType == "isNullAt") {
                obj->isNullAt(std::get<int>(params[0]));
            } else if (node.operatorType == "filteredIndex") {
                obj->filteredIndex(std::get<std::function<bool(std::optional<ArrayType>)>>(params[0]));
            } else if (node.operatorType == "aggregate") {
                obj->aggregate(std::get<std::function<ArrayType(ArrayType, ArrayType)>>(params[0]),
                               std::get<ArrayType>(params[1]));
            } else if (node.operatorType == "rename") {
                obj->rename(std::get<std::string>(params[0]), std::get<std::string>(params[1]));
            } else if (node.operatorType == "dataAt") {
                obj->dataAt(std::get<std::string>(params[0]), std::get<int>(params[1]));
            } else if (node.operatorType == "innerJoin") {
                obj->innerJoin(std::get<Table*>(params[0]), std::get<Table*>(params[1]),
                               std::get<std::vector<std::string>>(params[2]));
            } else if (node.operatorType == "leftJoin") {
                obj->leftJoin(std::get<Table*>(params[0]), std::get<Table*>(params[1]),
                              std::get<std::vector<std::string>>(params[2]));
            } else if (node.operatorType == "rightJoin") {
                obj->rightJoin(std::get<Table*>(params[0]), std::get<Table*>(params[1]),
                               std::get<std::vector<std::string>>(params[2]));
            } else if (node.operatorType == "outerJoin") {
                obj->outerJoin(std::get<Table*>(params[0]), std::get<Table*>(params[1]),
                               std::get<std::vector<std::string>>(params[2]));
            } else {
                throw std::runtime_error("Unsupported operator: " + node.operatorType);
            }
        }, node.subject);
    }
}
