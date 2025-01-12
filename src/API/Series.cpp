
#include <stdexcept>
#include "Series.hpp"
#include "QueryPlanner.hpp"

Series::Series(){
    arr = std::make_shared<Array>();
}

Series::Series(Array arr){
    arr = std::make_shared<Array>(arr);
}

Series::Series(const std::vector<SeriesType>& input){
    std::vector<std::optional<int>> optionalValues;

    std::transform(input.begin(), input.end(), std::back_inserter(optionalValues),
                [](int value) { return std::optional<int>(value); });
    
    arr = std::make_shared<Array>(optionalValues);
}

Series::Series(SeriesType data){
    arr = std::make_shared<Array>(std::make_optional(data));
}

void Series::append(SeriesType data){
    
    QueryPlanner::getInstance().addNode(
        arr,
        "append",
        0,
        {data}
    );

}

void Series::appendNull(){
    QueryPlanner::getInstance().addNode(
        arr,
        "appendNull",
        0,
        {}
    );
}

std::shared_ptr<Array> Series::getArray(){
    return arr;
}

void Series::extend(const Series& s){

    QueryPlanner::getInstance().addNode(
        arr,
        "extend",
        0,
        {s.getArray()}
    );
}

template <typename Func> 
void Series::map(Func f){
    
    QueryPlanner::getInstance().addNode(
        arr,
        "map",
        0,
        {f}
    );
}

template <typename Func> 
void Series::filter(Func f) {

    QueryPlanner::getInstance().addNode(
        arr,
        "filter",
        0,
        {f}
    );
}

template <typename Func> 
std::vector<size_t> Series::filteredIndex(Func f) {

    QueryPlanner::getInstance().addNode(
        arr,
        "filteredIndex",
        2,
        {f}
    );
}

void Series::removeByIndex(const std::vector<size_t> indexes) {

    QueryPlanner::getInstance().addNode(
        arr,
        "removeByIndex",
        0,
        {indexes}
    );
}

size_t Series::size(){
    arr->size();
}

SeriesType Series::operator[](int idx) const{
    if((*arrayPtr)[idx] == std::nullopt){
        throw std::invalid_argument("the value you selected is null");
    }
    return (*arrayPtr)[idx];
}

void Series::fillNulls( SeriesType val){
    QueryPlanner::getInstance().addNode(
        arr,
        "fillNulls",
        0,
        {val}
    );
}

bool Series::isNull(int idx){
    QueryPlanner::getInstance().addNode(
        arr,
        "isNull",
        1,
        {idx}
    );

}

template <typename Func> 
SeriesType Series::aggregate(Func aggFunc, SeriesType initialValue) const {
    QueryPlanner::getInstance().addNode(
        arr,
        "aggregate",
        1,
        {aggFunc, initialValue}
    );
}

std::any collect() {
    auto result = executePlan();
    switch (queryPlan.back().resultType) { // Dispatch based on last operation
        case Type::VECTOR_SIZE: return std::get<std::vector<size_t>>(result);
        case Type::VOID: arr = make_shared<*Array>(std::get<Array*>(result));
        case Type::SCALAR: return std::get<SeriesType>(result);
        default: throw std::runtime_error("Unsupported result type.");
    }
    return 0;
}
