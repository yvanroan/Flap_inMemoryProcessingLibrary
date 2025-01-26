#include <iostream>
#include <typeinfo>
#include <stdexcept>
#include "ArrayInt.hpp"
// #include "../memory/memManager.cpp"

ArrayInt::ArrayInt(){
    // mem_id = memManager::getInstance().registerInstance(0);
}


ArrayInt::ArrayInt(ArrayInt& arr)
        : sequence(arr.getArray()), size(arr.getSize()){
    
}

ArrayInt::ArrayInt(const std::vector<int>& input) 
        : sequence(input.begin(), input.end()), size(input.size()){
            
    // mem_id = memManager::getInstance().registerInstance(size * sizeof(sequence[0]));
}


// ArrayInt::~ArrayInt(){
//     // memManager::getInstance().removeInstance(mem_id);
// }



size_t ArrayInt::memoryUsage() const{
    return size * sizeof(int);
} 

void ArrayInt::reportMemoryUsage() const{
    // memManager::getInstance().updateInstance(mem_id, memoryUsage());
} 

void ArrayInt::append(int data){
    sequence.emplace_back(data);
    size++;
    reportMemoryUsage();
}

void ArrayInt::extend( const ArrayInt& arr){

    for(auto e: arr.sequence){
        sequence.emplace_back(e);
    }
    size=+arr.getSize();

    reportMemoryUsage();
}

void ArrayInt::map(std::function<void(int&)> f){
    for (auto& elem : sequence) {
        f(elem);
    }
}

void ArrayInt::filter(std::function<bool(const int&)> f) {
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

size_t ArrayInt::getSize() const{
    return size;
}


std::vector<size_t> ArrayInt::filteredIndex(std::function<bool(const int&)> f) {
    std::vector<size_t> indexes;

    for (size_t it = 0; it < sequence.size(); ++it) {
        const auto& elem = sequence[it];
        if(!f(elem)){
            indexes.emplace_back(it);
        }
    }

    return indexes;
}

int ArrayInt::aggregate(std::function<int(int, int)> aggFunc, int initial) const{
    int result = initial;
    for (const auto& elem : sequence) {
        result = aggFunc(elem, result);
    }
    return result;
}

void ArrayInt::removeByIndex(int index){
    if(index<0 or index> size){
        throw std::out_of_range("The index is out of range. Make sure you start from idx zero");
    }

    sequence.erase(sequence.begin() + index);
    size--;
}

int ArrayInt::operator[](size_t idx) const{
    if (idx >= sequence.size()) {
        throw std::out_of_range("Index out of range");
    }
    return sequence[idx];
}

std::string ArrayInt::typedef_() const {
    return "int";
}

std::vector<int> ArrayInt::getArray() const{
    return sequence;
}


bool ArrayInt::operator==(ArrayInt& arr){
    if(getArray() == arr.getArray()){
        return true;
    }
    return false;
}

bool ArrayInt::operator!=(ArrayInt& arr){
    return !(*this == arr);
}

bool ArrayInt::operator==(Array& arr){
    if(arr.typedef_() != "int"){
        return false;
    }
    const ArrayInt* otherArr = dynamic_cast<const ArrayInt*>(&arr);
    if (!otherArr) return false;

    return  otherArr->getArray() == sequence;

}
    
bool ArrayInt::operator!=(Array& arr){
    return !(*this == arr);
}

std::ostream& operator<<(std::ostream& os,  ArrayInt& arr) {
    
    for (const auto& value : arr.getArray()) {
        os << value << ", ";
    }
    os << std::endl;

    return os;
}


std::shared_ptr<Array> ArrayInt::clone() const{
    return std::make_shared<ArrayInt>(sequence);
}
