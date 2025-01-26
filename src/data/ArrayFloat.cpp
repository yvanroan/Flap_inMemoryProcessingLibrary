#include <iostream>
#include <typeinfo>
#include <stdexcept>
#include "ArrayFloat.hpp"

// #include "../memory/memManager.cpp"

ArrayFloat::ArrayFloat(){
    // mem_id = memManager::getInstance().registerInstance(0);
}


ArrayFloat::ArrayFloat(ArrayFloat& arr)
        : sequence(arr.getArray()), size(arr.getSize()){
    
}

ArrayFloat::ArrayFloat( const std::vector<float>& input) 
        : sequence(input.begin(), input.end()), size(input.size()){
            
    // mem_id = memManager::getInstance().registerInstance(size * sizeof(sequence[0]));
}


// ArrayFloat::~ArrayFloat(){
//     // memManager::getInstance().removeInstance(mem_id);
// }



size_t ArrayFloat::memoryUsage() const{
    return size * sizeof(sequence[0]);
} 

void ArrayFloat::reportMemoryUsage() const{
    // memManager::getInstance().updateInstance(mem_id, memoryUsage());
} 

void ArrayFloat::append(float data){
    sequence.emplace_back(data);
    size++;
    reportMemoryUsage();
}

void ArrayFloat::extend(ArrayFloat& arr){

    for(auto e: arr.sequence){
        sequence.emplace_back(e);
    }
    size += arr.getSize();

    reportMemoryUsage();
}

void ArrayFloat::map(std::function<void(float&)> f){
    for (auto& elem : sequence) {
        f(elem);
    }
}

void ArrayFloat::filter(std::function<bool(const float&)> f) {
    sequence.erase(
        std::remove_if(
                sequence.begin(), 
                sequence.end(),
                [&f](const auto& elem) {
                    return !f(elem);
                }
            ),
        sequence.end()
    );
    size = sequence.size();
}

size_t ArrayFloat::getSize() const{
    return size;
}


std::vector<size_t> ArrayFloat::filteredIndex(std::function<bool(const float&)> f) {
    std::vector<size_t> indexes;

    for (size_t it = 0; it < sequence.size(); ++it) {
        const auto& elem = sequence[it];
        if(f(elem)){
            indexes.emplace_back(it);
        }
    }

    return indexes;
}

float ArrayFloat::aggregate(std::function<float(float, float)> aggFunc, float initial) const{
    float result= initial;
    for (const auto& elem : sequence) {
        result = aggFunc(elem, result);
    }
    return result;
}

void ArrayFloat::removeByIndex(int index){
    if(index<0 or index> size){
        throw std::out_of_range("The index is out of range. Make sure you start from idx zero");
    }

    sequence.erase(sequence.begin() + index);
    size--;
}

float ArrayFloat::operator[](int idx) const{
    if (idx >= sequence.size()) {
        throw std::out_of_range("Index out of range");
    }
    return sequence[idx];
}

std::string ArrayFloat::typedef_() const {
    return "float";
}

bool ArrayFloat::operator==(const ArrayFloat& arr) const{
    if(getArray() == arr.getArray()){
        return true;
    }
    return false;
}

bool ArrayFloat::operator!=(const ArrayFloat& arr) const{
    return !(*this == arr);
}


bool ArrayFloat::operator==(const Array& arr) const{
    if(arr.typedef_() != "float"){
        return false;
    }

    const ArrayFloat* arrfloat = dynamic_cast<const ArrayFloat*>(&arr);
    if (!arrfloat) return false;

    return arrfloat->getArray() == sequence;

}
    
bool ArrayFloat::operator!=(const Array& arr) const{

    return !(*this == arr);
}


std::vector<float> ArrayFloat::getArray() const{
    return sequence;
}

std::ostream& operator<<(std::ostream& os,  ArrayFloat& arr) {
    
    for (const auto& value : arr.getArray()) {
        os << value << ", ";
    }
    os << std::endl;

    return os;
}


std::shared_ptr<Array> ArrayFloat::clone() const{
    return std::make_shared<ArrayFloat>(sequence);
}
