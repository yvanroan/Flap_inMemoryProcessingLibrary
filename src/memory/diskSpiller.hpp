#ifndef DISK_SPILLER_HPP
#define DISK_SPILLER_HPP

#include <unordered_map>
#include <variant>
#include <vector>
#include <string>
#include "../data/Array.hpp"
#include "../data/Table.hpp"

using ObjectType = std::variant<Array, Table>;

class DiskSpiller {
private:
    std::vector<size_t> spilled;
    const size_t spillThreshold= 30; 
    static DiskSpiller instance;
    std::unordered_map<size_t, ObjectType> evictedObjects;                        

    DiskSpiller() = default;

    std::string generateFilePath(size_t id) const;
    
    void serializeToFile(const ObjectType& obj, const std::string& filePath);

    ObjectType deserializeFromFile(const std::string& filePath);

public:
    static DiskSpiller& getInstance();

    void collect(size_t id, ObjectType obj);

    ObjectType load(size_t id);

    void cleanup();
};

#endif