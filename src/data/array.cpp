#include <iostream>
#include <vector>
#include <optional>
#include <typeinfo>
#include "array.hpp"



template <typename T>

Array::Array(const std::vector<std::optional<T>>& input){

    for(auto e: input){
        sequence.emplace_back(e);
    }
    size += input.size();
}

void Array<std::optional<T>>::append(std::optional<T> data){
    sequence.emplace_back(data);
    size++;
}

void Array<std::optional<T>>::appendNull(){
    sequence.emplace_back(std::nullopt);
    size++;
}
void Array<std::optional<T>>::append(const Array<std::optional<T>>& arr){

    for(auto e: arr.sequence){
        sequence.emplace_back(e);
    }
    size += arr.size();
}
template <typename Func> void Array<std::optional<T>>::map(Func f){
    
    for(auto& e: this.sequence){
        e=f(e.value()); 
    }
}
template <typename Func> void Array<std::optional<T>>::filter(Func f) {
    for(size_t it= 0; it<sizeof(this.sequence); it++){
        if(!f(sequence[it])){
            sequence.erase(sequence.begin()+it);
        }
    }
}
template <typename Func> std::vector<size_t> Array<std::optional<T>>::filteredIndex(Func f) {
    vector<size_t> indexes;
    for( size_t it= 0; it<sizeof(this.sequence); it++){
        if(f(sequence[it])){
            indexes.emplace_back(it);
        }
    }
    return indexes;
}


size_t Arrat<std::optional<T>>::size(){
    return size;
}

std::optional<T> Array<std::optional<T>>::getByIndex(int idx) const{
    if (idx >= sequence.size()) {
        throw std::out_of_range("Index out of range");
    }
    return sequence[idx];
}

size_t Array<std::optional<T>>::memoryUsage() const{
    if(size == 0){
        return 0;
    }
    return size * sizeof(sequence[0])
} 

void Array<std::optional<T>>::fillNulls( T val){
    for(size_t i=0; i<n; i++){
        if(!sequence[i].has_value()){
            sequence[i] = val;
        }
    }
}

bool Array<std::optional<T>>::isNull(int idx){
    if (idx >= sequence.size()) {
        throw std::out_of_range("Index out of range");
    }
    return !sequence[idx].has_value()

}

std::string Array<std::optional<T>>::typedef_() const{
    return typeid(T).name;
}

template <typename Func> T Array<std::optional<T>>::aggregate(Func aggFunc, T initialValue) const {
    T result = initialValue;
    for (const auto& value : data) {
        if (value.has_value()) {
            result = aggFunc(result, value.value());
        }
    }
    return result;
}

std::vector<std::optional<T>> Array<std::optional<T>>::getArray(){
    return sequence;
}

