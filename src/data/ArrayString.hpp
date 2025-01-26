#pragma once
#include <iostream>
#include <functional>
#include <any>
#include <vector>
#include <string>
#include "Array.hpp"

class ArrayString : public Array {
    size_t size = 0;
    std::vector<std::string> sequence;

public:
    ArrayString();
    ArrayString(ArrayString& arr);
    ArrayString(const std::vector<std::string>& input);

    ~ArrayString() = default;

    void append(std::string data);
    void extend(ArrayString& arr);
    size_t getSize() const override;
    std::shared_ptr<Array> clone() const override;

    std::string operator[](int idx) const;
    bool operator==(ArrayString& arr) ;
    bool operator!=(ArrayString& arr) ;
    bool operator==(Array& arr) override;
    bool operator!=(Array& arr) override;

    size_t memoryUsage() const override;
    std::string typedef_() const override; 

    void removeByIndex(const std::vector<size_t> indexes);
    void removeByIndex(int index);
    
    std::vector<size_t> filteredIndex(std::function<bool(const std::string&)> f);
    void map(std::function<void(std::string&)> f);
    void filter(std::function<bool(const std::string&)> f); 
    std::string aggregate(std::function<std::string(std::string, std::string)> aggFunc, std::string initial) const;
    // virtual std::shared_ptr<Array> clone() const = 0; // Support copying arrays
    
    std::ostream& print() const;
    std::vector<std::string> getArray() const;
    void reportMemoryUsage() const;

};

std::ostream& operator<<(std::ostream& os,  ArrayString& arr);