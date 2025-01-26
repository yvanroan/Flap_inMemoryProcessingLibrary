#pragma once

#include <iostream>
#include <vector>
#include <functional>
#include "Array.hpp"

class ArrayInt: public Array{

protected:
    size_t size = 0;
    std::vector<int> sequence;

public:
    ArrayInt();
    ArrayInt(ArrayInt& arr);
    ArrayInt(const std::vector<int>& input);

    ~ArrayInt()= default;

    void append(int data);
    void extend(const ArrayInt& arr);
    size_t getSize() const override;
    std::shared_ptr<Array> clone() const override;

    int operator[](size_t idx) const;
    bool operator==(ArrayInt& arr);
    bool operator!=(ArrayInt& arr);
    bool operator==(Array& arr) override;
    bool operator!=(Array& arr) override;
    

    size_t memoryUsage() const override;
    std::string typedef_() const override; 

    void removeByIndex(const std::vector<size_t> indexes);
    void removeByIndex(int index);
    
    std::vector<size_t> filteredIndex(std::function<bool(const int&)> f);
    void map(std::function<void(int&)> f);
    void filter(std::function<bool(const int&)> f); 
    int aggregate(std::function<int(int, int)> aggFunc, int initial) const;

    std::vector<int> getArray() const;
    void reportMemoryUsage() const ; 
};
std::ostream& operator<<(std::ostream& os, ArrayInt& arr);
