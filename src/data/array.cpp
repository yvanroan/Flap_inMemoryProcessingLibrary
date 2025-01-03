#include <iostream>
#include <vector>
#include <optional>
#include <typeinfo>
#include "array.hpp"

Array::Array(const std::vector<std::optional<ArrayType>>& input){

    for(auto e: input){
        sequence.emplace_back(e);
    }
    size += input.size();
}

Array::Array(std::optional<ArrayType> data){
    sequence.emplace_back(data);
    size++;
}

void Array::append(std::optional<ArrayType> data){
    sequence.emplace_back(data);
    size++;
}

void Array::appendNull(){
    sequence.emplace_back(std::nullopt);
    size++;
}

void Array::extend(const Array& arr){

    for(auto e: arr.sequence){
        sequence.emplace_back(e);
    }
    size += arr.size();
}

template <typename Func> 
void Array::map(Func f){
    
    for(auto& e: this.sequence){
        e=f(e.value()); 
    }
}

template <typename Func> 
void Array::filter(Func f) {
    for(size_t it= 0; it<sizeof(this.sequence); it++){
        if(!f(sequence[it])){
            sequence.erase(sequence.begin()+it);
        }
    }
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

size_t Array::memoryUsage() const{
    if(size == 0){
        return 0;
    }
    return size * sizeof(sequence[0])
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
    return !sequence[idx].has_value()

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

