#pragma once

#include <memory>
#include <string>
#include <iostream>

class Array {
protected:
    
    size_t mem_id;

public:
    virtual ~Array() = default;
    
    virtual size_t getSize() const{return 0;};
    virtual size_t memoryUsage() const {return 0;};
    virtual std::string typedef_() const {return "null";};
    virtual bool operator==(const Array& arr) const =0;
    virtual bool operator!=(const Array& arr) const =0;
    virtual std::shared_ptr<Array> clone() const = 0;
    virtual void removeByIndex(const std::vector<size_t> indexes) =0;
    virtual void removeByIndex(int index) =0;
    // virtual void reportMemoryUsage() const =0;
};

// std::ostream& operator<<(std::ostream& os, Array& arr ){
//     os<<"Null Array\n";
//     return os;
// }