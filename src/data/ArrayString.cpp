#include <iostream>
#include <typeinfo>
#include <stdexcept>
#include "ArrayString.hpp"
// #include "../memory/memManager.cpp"

ArrayString::ArrayString(){
    // mem_id = memManager::getInstance().registerInstance(0);
}


ArrayString::ArrayString(ArrayString& arr)
        : sequence(arr.getArray()), size(arr.getSize()){

}

ArrayString::ArrayString(const std::vector<std::string>& input) 
        : sequence(input.begin(), input.end()), size(input.size()) {
    ;
    // mem_id = memManager::getInstance().registerInstance(size * sizeof(sequence[0]));
}


// ArrayString::~ArrayString(){
//     // memManager::getInstance().removeInstance(mem_id);
// }



size_t ArrayString::memoryUsage() const{
    return size * sizeof(sequence[0]);
} 

void ArrayString::reportMemoryUsage() const{
    // memManager::getInstance().updateInstance(mem_id, memoryUsage());
} 

void ArrayString::append(std::string data){
    sequence.emplace_back(data);
    size++;
    reportMemoryUsage();
}

void ArrayString::extend(ArrayString& arr){

    for(auto e: arr.sequence){
        sequence.emplace_back(e);
    }
    size+=arr.getSize();

    reportMemoryUsage();
}

void ArrayString::map(std::function<void(std::string&)> f){
    for (auto& elem : sequence) {
        f(elem);
    }
}

void ArrayString::filter(std::function<bool(const std::string&)> f) {
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

    size =sequence.size();
}

std::vector<size_t> ArrayString::filteredIndex(std::function<bool(const std::string&)> f) {
    std::vector<size_t> indexes;

    for (size_t it = 0; it < sequence.size(); ++it) {
        const auto& elem = sequence[it];
        if(f(elem)){
            indexes.emplace_back(it);
        }
    }

    return indexes;
}

std::string ArrayString::aggregate(std::function<std::string(std::string, std::string)> aggFunc, std::string initial) const{
    std::string result= initial;
    for (const auto& elem : sequence) {
        result = aggFunc(elem, result);
    }
    return result;
}

void ArrayString::removeByIndex(int index){
    if(index<0 or index> size){
        throw std::out_of_range("The index is out of range. Make sure you start from idx zero");
    }

    sequence.erase(sequence.begin() + index);
    size--;
}

size_t ArrayString::getSize() const{
    
    return size;
}

std::string ArrayString::operator[](int idx) const{
    if (idx >= sequence.size()) {
        throw std::out_of_range("Index out of range");
    }
    return sequence[idx];
}

std::string ArrayString::typedef_() const {
    return "string";
}

std::vector<std::string> ArrayString::getArray() const{
    return this->sequence;
}

bool ArrayString::operator==(ArrayString& arr){
    if(getArray() == arr.getArray()){
        return true;
    }
    return false;
}

bool ArrayString::operator!=(ArrayString& arr){
    return !(*this == arr);
}


bool ArrayString::operator==(Array& arr){
    if(arr.typedef_() != "string"){
        return false;
    }


    const ArrayString* otherInt = dynamic_cast<const ArrayString*>(&arr);
    if (!otherInt) return false;

    return otherInt->getArray() == sequence;

}
    
bool ArrayString::operator!=(Array& arr){

    return !(*this == arr);
}

std::ostream& operator<<(std::ostream& os,  ArrayString& arr) {
    
    for (const auto& value : arr.getArray()) {
        os << value << ", ";
    }
    os << std::endl;

    return os;
}

std::shared_ptr<Array> ArrayString::clone() const{
    return std::make_shared<ArrayString>(sequence);
}
