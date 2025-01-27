#pragma once

#include <unordered_map>
#include <variant>
#include <vector>
#include <string>
#include "../data/Array.hpp"
#include "../data/Table.hpp"

class DiskSpiller {
private:
    std::vector<size_t> spilledTables;
    std::vector<size_t> spilledArrays;
    const size_t spillThreshold= 30; 
    static DiskSpiller instance;
    std::unordered_map<size_t, std::shared_ptr<Table>> evictedTables;    
    std::unordered_map<size_t, std::shared_ptr<Array>> evictedArrays;                    

    DiskSpiller() = default;

    std::string generateFilePath(size_t id, bool isArray) const;
    
    void serializeToFile( std::shared_ptr<Table> obj, const std::string& filePath);
    void serializeToFile(const std::shared_ptr<Array> obj, const std::string& filePath);

    std::shared_ptr<Array> deserializeArrayFromFile(const std::string& filePath);

    std::shared_ptr<Table> deserializeTableFromFile(const std::string& filePath);

public:
    static DiskSpiller& getInstance();

    void collect(size_t id, std::shared_ptr<Table> obj);

    void collect(size_t id, std::shared_ptr<Array> obj);

    std::shared_ptr<Array> loadArray(size_t id);

    std::shared_ptr<Table>  loadTable(size_t id);

    void cleanupTable();
    void cleanupArray();
};
