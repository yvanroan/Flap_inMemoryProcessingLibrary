#include <stdexcept>
#include "QueryPlanner.hpp"


QueryPlanner QueryPlanner::instance;

static QueryPlanner QueryPlanner::getInstance(){
    return instance;
}

QueryNode addNode(const std::variant<Array*, Table*>& subject,
                  const std::string& operatorType,
                  int return_type,
                  const std::vector<ParamsType>& params = {}) {
    return QueryNode {
        .subject = subject,
        .operatorType = operatorType,
        .return_type = return_type,
        .params = params
    };
}

std::vector<std::variant<Array*, Table*>> validateInputsAndGetObjects(const std::string& operatorType, const std::vector<ParmasType>& params, bool isArray) {
    std::vector<std::pair<std::string, std::type_index>> metadata;
    if(isArray){
        metadata = operatorArrayMetadata.at(operatorType);
    }
    else{
        metadata = operatorTableMetadata.at(operatorType);
    }

    if (params.size() != metadata.size()) {
        throw std::runtime_error("Incorrect number of parameters for operator: " + operatorType + " in the " +  (isArray? "Series" : "Dataframe") + " class");
    }

    std::vector<std::variant<Array*, Table*>> objects;

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

void validateNode(QueryNode node, bool isArray) {
    std::visit([](auto* obj) {
        if (!obj) throw std::runtime_error("Subject is null.");
        if (!MemoryManager::getInstance().isInMemory(obj->getId())) {
            DiskSpiller::getInstance().load(obj->getId()); // Load if spilled
        }
    }, node.subject);
    
    for (const auto& param : validateInputsAndGetObjects(node.operatorType, node.params, isArray)) {
        std::visit([](auto* obj) {
            if (!obj) throw std::runtime_error("One or more paramters are null.");
            if (!MemoryManager::getInstance().isInMemory(obj->getId())) {
                DiskSpiller::getInstance().load(obj->getId()); // Load if spilled
            }
        }, param);
    }
}

FuncReturnType executeNode(QueryNode node, bool isArray) {

    validateNode(QueryNode node, isArray);

    const auto& params = node.params;

    if (isArray){

        if (node.operatorType == "append") {
            node.subject->append(std::get<std::optional<ArrayType>>(params[0]));
        } else if (node.operatorType == "appendNull") {
            node.subject->appendNull();
        } else if (node.operatorType == "extend") {
            node.subject->extend(std::get<Array*>(params[0]));
        } else if (node.operatorType == "map") {
            node.subject->map(std::get<std::function<std::optional<ArrayType>(std::optional<ArrayType>)>>(params[0]));
        } else if (node.operatorType == "filter") {
            node.subject->filter(std::get<std::function<bool(std::optional<ArrayType>)>>(params[0]));
        } else if (node.operatorType == "fillNulls") {
            node.subject->fillNulls(std::get<ArrayType>(params[0]));
        } else if (node.operatorType == "isNull") {
            return node.subject->isNull(std::get<int>(params[0]));
        } else if (node.operatorType == "filteredIndex") {
            return node.subject->filteredIndex(std::get<std::function<bool(std::optional<ArrayType>)>>(params[0]));
        } else if (node.operatorType == "aggregate") {
            return node.subject->aggregate(std::get<std::function<ArrayType(ArrayType, ArrayType)>>(params[0]),
                            std::get<ArrayType>(params[1]));
        } else if (node.operatorType == "removeByIndex") {
            return node.subject->removeByIndex(std::get<std::vector<size_t>>(params[0]));
        } else {
            throw std::runtime_error("Unsupported operator: " + node.operatorType + "for Dataframe object");
        }
    }
    else{
        if (node.operatorType == "appendCol") {
            node.subject->appendCol(std::get<std::string>(params[0]), std::get<Array*>(params[1]));
        } else if (node.operatorType == "appendRow") {
            node.subject->appendRow(std::get<std::vector<std::optional<ArrayType>>>(params[0]));
        } else if (node.operatorType == "rename") {
            node.subject->rename(std::get<std::string>(params[0]), std::get<std::string>(params[1]));
        } else if (node.operatorType == "column") {
            node.subject->column(std::get<std::string>(params[0]));
        } else if (node.operatorType == "row") {
            return node.subject->row(std::get<int>(params[0]));
        } else if (node.operatorType == "rows_vector") {
            return node.subject->rows(std::get<std::vector<size_t>(params[0]));
        } else if (node.operatorType == "rows_idx") {
            return node.subject->rows(std::get<int>(params[0]), std::get<int>(params[1]));
        } else if (node.operatorType == "removeCol") {
            node.subject->removeCol(std::get<std::string>(params[0]));
        } else if (node.operatorType == "removeRows") {
            node.subject->removeRows(std::get<std::vector<size_t>>(params[0]));
        } else if (node.operatorType == "filterRow") {
            node.subject->filterRow(std::get<std::function<bool(std::optional<ArrayType>)>>(params[0]), std::get<std::string>(params[1]));
        } else if (node.operatorType == "filterCol") {
            node.subject->removeCol(std::get<std::vector<std::string>>(params[0]), std::get<bool>(params[1]));
        } else if (node.operatorType == "map") {
            node.subject->map(std::get<std::function<std::optional<ArrayType>(std::optional<ArrayType>)>>(params[0]), std::get<std::string>(params[1]));
        } else if (node.operatorType == "innerJoin") {
            return node.subject->innerJoin(std::get<Table*>(params[0]), std::get<Table*>(params[1]),
                            std::get<std::vector<std::string>>(params[2]));
        } else if (node.operatorType == "leftJoin") {
            return node.subject->leftJoin(std::get<Table*>(params[0]), std::get<Table*>(params[1]),
                            std::get<std::vector<std::string>>(params[2]));
        } else if (node.operatorType == "rightJoin") {
            return node.subject->rightJoin(std::get<Table*>(params[0]), std::get<Table*>(params[1]),
                            std::get<std::vector<std::string>>(params[2]));
        } else if (node.operatorType == "outerJoin") {
            return node.subject->outerJoin(std::get<Table*>(params[0]), std::get<Table*>(params[1]),
                            std::get<std::vector<std::string>>(params[2]));
        } else if (node.operatorType == "aggregateColumn") {
            return node.subject->aggregateColumn(std::get<std::string>(params[0]), std::get<std::function<ArrayType(ArrayType, ArrayType)>>(params[1]),
                            std::get<ArrayType>(params[2]));
        } else if (node.operatorType == "aggregateColumns") {
            return node.subject->aggregateColumns(std::get<std::vector<std::string>>(params[0]), std::get<std::function<ArrayType(ArrayType, ArrayType)>>(params[1]),
                            std::get<ArrayType>(params[2]));
        } else {
            throw std::runtime_error("Unsupported operator: " + node.operatorType + "for Dataframe object");
        }
    }    
}

FuncReturnType executePlan() {
    bool isArray= true;
    std::variant<Array*, Table*> currentResult = queryPlan[0].subject;

    if(std::get_if<Table*>(&currentResult) ){
        isArray = false;
    }

    for (auto& node : queryPlan) {

        node.subject = currentResult;

        FuncReturnType result = executeNode(node, isArray);

        if (auto obj = std::get_if<Array*>(&result)) {
            currentResult = *obj;
            isArray = true;
        } else if (!isArray and auto obj = std::get_if<Table*>(&result)) {
            currentResult = *obj;
        } else{
            if (&node != &queryPlan.back()) { 
                throw std::runtime_error("Cannot perform operations after final result.");
            }
            return result;
        }
        //a case we skipped here is if the intermediate result is a row/list of rows
        // eventually the user should be able to navigate these vectors using operator[]
        // within the same line but for now theyll have to do it on a diff line.
    }

    return currentResult;
        
}
