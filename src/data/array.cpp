#include <iostream>
#include <vector>
#include <optional>
#include <typeinfo>
#include <stdexcept>
#include "array.hpp"
#include "memManager.hpp"

Array::Array(){
    mem_id = memManager::getInstance().registerInstance(0);
}

Array::Array(const std::vector<std::optional<ArrayType>>& input){

    mem_id = memManager::getInstance().registerInstance();

    for(auto e: input){
        sequence.emplace_back(e);
    }
    size += input.size();


    mem_id = memManager::getInstance().registerInstance(size * sizeof(sequence[0]));
}

Array::~Array(){
    memManager::getInstance().removeInstance(mem_id);
}

Array::Array(std::optional<ArrayType> data){
    sequence.emplace_back(data);
    size++;
    mem_id = memManager::getInstance().registerInstance(size * sizeof(sequence[0]));
}

void Array::memoryUsage() const{
    return size * sizeof(sequence[0]);
} 

void Array::reportMemoryUsage() const{
    memManager::getInstance().updateInstance(mem_id, memoryUsage());
} 

void Array::append(std::optional<ArrayType> data){
    sequence.emplace_back(data);
    size++;
    reportMemoryUsage();
}

void Array::appendNull(){
    sequence.emplace_back(std::nullopt);
    size++;
    reportMemoryUsage();
}

void Array::extend(const Array& arr){

    for(auto e: arr.sequence){
        sequence.emplace_back(e);
    }
    size += arr.size();

    reportMemoryUsage();
}

template <typename Func> 
void Array::map(Func f){
    
    for(auto& e: this.sequence){
        e=f(e.value()); 
    }

    reportMemoryUsage();
}

template <typename Func> 
void Array::filter(Func f) {
    for(size_t it= 0; it<sizeof(this.sequence); it++){
        if(!f(sequence[it])){
            sequence.erase(sequence.begin()+it);
        }
    }
    reportMemoryUsage();
}

template <typename Func> 
std::vector<size_t> Array::filteredIndex(Func f) {
    vector<size_t> indexes;
    for( size_t it= 0; it<sizeof(this.sequence); it++){
        if(f(sequence[it])){
            indexes.emplace_back(it);
        }
    }
    return indexes;
}

size_t Array::size(){
    return size;
}

std::optional<ArrayType> Array::getByIndex(int idx) const{
    if (idx >= sequence.size()) {
        throw std::out_of_range("Index out of range");
    }
    return sequence[idx];
}

void Array::fillNulls( ArrayType val){
    for(size_t i=0; i<n; i++){
        if(!sequence[i].has_value()){
            sequence[i] = val;
        }
    }
}

bool Array::isNull(int idx){
    if (idx >= sequence.size()) {
        throw std::out_of_range("Index out of range");
    }
    return !sequence[idx].has_value();

}

void Array::memoryUsage() const{
    return size * sizeof(sequence[0]);
} 

std::string Array::typedef_() const {
    if (sequence.empty()) {
        return "empty";
    }

    for(auto& val : sequence){
        if (val.has_value()) {
            return typeid(val.value()).name();
        }
    }

    return "unknown";
}

template <typename Func> 
ArrayType Array::aggregate(Func aggFunc, ArrayType initialValue) const {
    ArrayType result = initialValue;
    for (const auto& value : data) {
        if (value.has_value()) {
            result = aggFunc(result, value.value());
        }
    }
    return result;
}

std::vector<std::optional<ArrayType>> Array::getArray(){
    return sequence;
}

