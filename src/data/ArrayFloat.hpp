#pragma once

#include <functional>
#include <any>
#include <iostream>
#include <vector>
#include "Array.hpp"

class ArrayFloat: public Array {
    size_t size = 0;
    std::vector<float> sequence;

public:
    ArrayFloat();
    ArrayFloat(ArrayFloat& arr);
    ArrayFloat(const std::vector<float>& input);

    ~ArrayFloat()= default;

    void append(float data);
    void extend(ArrayFloat& arr);
    size_t getSize() const override;
    std::shared_ptr<Array> clone() const override;

    float operator[](int idx) const;
    bool operator==(ArrayFloat& arr);
    bool operator!=(ArrayFloat& arr);
    bool operator==(Array& arr) override;
    bool operator!=(Array& arr) override;


    size_t memoryUsage() const override;
    std::string typedef_() const override; 

    void removeByIndex(const std::vector<size_t> indexes);
    void removeByIndex(int index);
    
    std::vector<size_t> filteredIndex(std::function<bool(const float&)> f);
    void map(std::function<void(float&)> f);
    void filter(std::function<bool(const float&)> f); 
    float aggregate(std::function<float(float, float)> aggFunc, float initial) const;
    // virtual std::shared_ptr<Array> clone() const = 0; // Support copying arrays

    std::ostream& print() const;
    std::vector<float> getArray() const;
    void reportMemoryUsage() const; 
};
std::ostream& operator<<(std::ostream& os,  ArrayFloat& arr);
