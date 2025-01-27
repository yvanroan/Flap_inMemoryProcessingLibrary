
#include <stdexcept>
#include "Series.hpp"

#include "ArrayInt.hpp"
#include "ArrayFloat.hpp"
#include "ArrayString.hpp"
#include "QueryPlanner.hpp"

Series::Series(){
    arr = std::make_shared<ArrayInt>();
}

Series::Series(Series& s){
    arr = s.getArray();
}

Series::Series(std::shared_ptr<Array> array){
    arr = array;
}

Series::Series(std::vector<int> input){
    arr= std::make_shared<ArrayInt>(input);
}

Series::Series(std::vector<std::string> input){
    arr= std::make_shared<ArrayString>(input);
}

Series::Series(std::vector<float> input){
    arr= std::make_shared<ArrayFloat>(input);
}


void Series::append(int data){
    
    // QueryPlanner::getInstance().addNode(
    //     arr,
    //     "append",
    //     0,
    //     {data}
    // );

}

void Series::append(int data){
    if (auto intCol = std::dynamic_pointer_cast<ArrayInt>(arr)) {
        intCol->append(data);
    }else{
        throw std::invalid_argument("the input type should be a "+ arr->typedef_());
    }
}
void Series::append(std::string data){
    if (auto col = std::dynamic_pointer_cast<ArrayString>(arr)) {
        col->append(data);
    }else{
        throw std::invalid_argument("the input type should be a "+ arr->typedef_());
    }
}
void Series::append(float data){
    if (auto col = std::dynamic_pointer_cast<ArrayFloat>(arr)) {
        col->append(data);
    }else{
        throw std::invalid_argument("the input type should be a "+ arr->typedef_());
    }
}

void Series::extend( Series& col){
    if (auto intCol = std::dynamic_pointer_cast<ArrayInt>(arr)) {
        auto otherCol = std::dynamic_pointer_cast<ArrayInt>(col.getArray());
        intCol->extend(*otherCol);
    } else if (auto stringCol = std::dynamic_pointer_cast<ArrayString>(arr)) {
        auto otherCol = std::dynamic_pointer_cast<ArrayString>(col.getArray());
        stringCol->extend(*otherCol);
    } else if (auto floatCol = std::dynamic_pointer_cast<ArrayFloat>(arr)) {
        auto otherCol = std::dynamic_pointer_cast<ArrayFloat>(col.getArray());
        floatCol->extend(*otherCol);
    } else{
        throw std::invalid_argument("Expected object of base type " + typedef_()+", but got " +col.typedef_());
    }
}

std::vector<size_t> Series::filteredIndex(std::function<bool(const int&)> f){
    if (auto intCol = std::dynamic_pointer_cast<ArrayInt>(arr)) {
        return intCol->filteredIndex(f);
    }else{
        throw std::invalid_argument("the input type should be a "+ arr->typedef_());
    }
}
void Series::map(std::function<void(int&)> f){
    if (auto intCol = std::dynamic_pointer_cast<ArrayInt>(arr)) {
        intCol->map(f);
    }else{
        throw std::invalid_argument("the input type should be a "+ arr->typedef_());
    }
}

void Series::filter(std::function<bool(const int&)> f){
    if (auto intCol = std::dynamic_pointer_cast<ArrayInt>(arr)) {
        intCol->filter(f);
    }else{
        throw std::invalid_argument("the input type should be a "+ arr->typedef_());
    }
}

int Series::aggregate(std::function<int(int, int)> aggFunc, int initial) const{
    if (auto intCol = std::dynamic_pointer_cast<ArrayInt>(arr)) {
        return intCol->aggregate(aggFunc, initial);
    }else{
        throw std::invalid_argument("the input type should be a "+ arr->typedef_());
    }
}

std::vector<size_t> Series::filteredIndex(std::function<bool(const float&)> f){
    if (auto col = std::dynamic_pointer_cast<ArrayFloat>(arr)) {
        return col->filteredIndex(f);
    }else{
        throw std::invalid_argument("the input type should be a "+ arr->typedef_());
    }
}
void Series::map(std::function<void(float&)> f){
    if (auto col = std::dynamic_pointer_cast<ArrayFloat>(arr)) {
        col->map(f);
    }else{
        throw std::invalid_argument("the input type should be a "+ arr->typedef_());
    }
}

void Series::filter(std::function<bool(const float&)> f){
    if (auto intCol = std::dynamic_pointer_cast<ArrayFloat>(arr)) {
        intCol->filter(f);
    }else{
        throw std::invalid_argument("the input type should be a "+ arr->typedef_());
    }
}

float Series::aggregate(std::function<float(float, float)> aggFunc, float initial) const{
    if (auto col = std::dynamic_pointer_cast<ArrayFloat>(arr)) {
        return col->aggregate(aggFunc, initial);
    }else{
        throw std::invalid_argument("the input type should be a "+ arr->typedef_());
    }
}

std::vector<size_t> Series::filteredIndex(std::function<bool(const std::string&)> f){
    if (auto col = std::dynamic_pointer_cast<ArrayString>(arr)) {
        return col->filteredIndex(f);
    }else{
        throw std::invalid_argument("the input type should be a "+ arr->typedef_());
    }
}
void Series::map(std::function<void(std::string&)> f){
    if (auto intCol = std::dynamic_pointer_cast<ArrayString>(arr)) {
        intCol->map(f);
    }else{
        throw std::invalid_argument("the input type should be a "+ arr->typedef_());
    }
}

void Series::filter(std::function<bool(const std::string&)> f){
    if (auto intCol = std::dynamic_pointer_cast<ArrayString>(arr)) {
        intCol->filter(f);
    }else{
        throw std::invalid_argument("the input type should be a "+ arr->typedef_());
    }
}

std::string Series::aggregate(std::function<std::string(std::string, std::string)> aggFunc, std::string initial) const{
    if (auto col = std::dynamic_pointer_cast<ArrayString>(arr)) {
        return col->aggregate(aggFunc, initial);
    }else{
        throw std::invalid_argument("the input type should be a "+ arr->typedef_());
    }
}

std::shared_ptr<Array> Series::getArray(){
    return arr;
}

size_t Series::size(){
    return arr->getSize(); 
}

std::string Series::typedef_() const{
    return arr->typedef_(); 
}

void Series::removeByIndex(const std::vector<size_t> indexes){
    arr->removeByIndex(indexes);
}

void Series::removeByIndex(int index){
    arr->removeByIndex(index);
}

// std::any collect() {
//     auto result = executePlan();
//     switch (queryPlan.back().resultType) { // Dispatch based on last operation
//         case Type::VECTOR_SIZE: return std::get<std::vector<size_t>>(result);
//         case Type::VOID: arr = make_shared<*Array>(std::get<Array*>(result));
//         case Type::SCALAR: return std::get<SeriesType>(result);
//         default: throw std::runtime_error("Unsupported result type.");
//     }
//     return 0;
// }
