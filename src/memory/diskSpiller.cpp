#include "diskSpiller.hpp"
#include "Serializer.cpp"
#include <string>
#include <fstream>


DiskSpiller DiskSpiller::instance;

DiskSpiller& DiskSpiller::getInstance() {
    return instance;
}

std::string DiskSpiller::generateFilePath(size_t id, bool isArray) const {
    std::string ans = "spill_";

    if(isArray){
        ans += "array";
    }else{
        ans += "table";
    }

    ans += std::to_string(id) + ".bin";
    return ans;
}

void DiskSpiller::collect(size_t id, std::shared_ptr<Array> obj) {

    evictedArrays[id] = std::move(obj);

    if (evictedArrays.size() >= spillThreshold) {
        for (const auto& [key, obj]: evictedArrays) {
            std::string filePath = generateFilePath(key, true);
            serializeToFile(obj, filePath);
            spilledArrays.emplace_back(key);
        }
        evictedArrays.clear();
    }
}

void DiskSpiller::collect(size_t id, std::shared_ptr<Table>  obj) {
    evictedTables[id] = std::move(obj);

    if (evictedTables.size() >= spillThreshold) {
        for (const auto& [key, obj]: evictedTables) {
            std::string filePath = generateFilePath(key, false);
            serializeToFile(obj, filePath);
            spilledTables.emplace_back(key);
        }
        evictedTables.clear();
    }
}

void DiskSpiller::serializeToFile(const std::shared_ptr<Array> obj, const std::string& filePath) {
    std::ofstream outFile(filePath);
    if (!outFile) {
        throw std::runtime_error("Failed to open file for writing: " + filePath);
    }

    std::string csv = Serializer::arrayToCsv(obj);
    outFile << csv;
    outFile.close();
}

void DiskSpiller::serializeToFile(std::shared_ptr<Table> obj, const std::string& filePath) {
    std::ofstream outFile(filePath);
    if (!outFile) {
        throw std::runtime_error("Failed to open file for writing: " + filePath);
    }

    std::string csv = Serializer::tableToCsv(*obj);
    outFile << csv;
    outFile.close();
}

std::shared_ptr<Array> DiskSpiller::deserializeArrayFromFile(const std::string& filePath) {
    std::ifstream inFile(filePath);
    if (!inFile) {
        throw std::runtime_error("Failed to open file for reading: " + filePath);
    }

    std::ostringstream buffer;
    buffer << inFile.rdbuf();

    return Serializer::csvToArray(buffer.str());
}

std::shared_ptr<Table> DiskSpiller::deserializeTableFromFile(const std::string& filePath) {
    std::ifstream inFile(filePath);
    if (!inFile) {
        throw std::runtime_error("Failed to open file for reading: " + filePath);
    }

    std::ostringstream buffer;
    buffer << inFile.rdbuf();

    return std::make_shared<Table>(Serializer::csvToTable(buffer.str()));
}


std::shared_ptr<Table> DiskSpiller::loadTable(size_t id) {
    

    if (evictedTables.find(id) != evictedTables.end()) {
        return evictedTables[id];
    } 

    std::string filePath = generateFilePath(id, false);
    return deserializeTableFromFile(filePath);
}

std::shared_ptr<Array> DiskSpiller::loadArray(size_t id) {

    if (evictedArrays.find(id) != evictedArrays.end()) {
        return evictedArrays[id];
    } 
    
    std::string filePath = generateFilePath(id, false);
    return deserializeArrayFromFile(filePath);
}

void DiskSpiller::cleanupTable() {
    for (const auto& id : spilledTables) {
        std::string filepath = generateFilePath(id, false);
        if (std::remove(filepath.c_str()) != 0) {
            std::cerr << "Failed to delete file: " + filepath << std::endl;
        }
    }
    spilledTables.clear();
}

void DiskSpiller::cleanupArray() {
    for (const auto& id : spilledArrays) {
        std::string filepath = generateFilePath(id, false);
        if (std::remove(filepath.c_str()) != 0) {
            std::cerr << "Failed to delete file: " + filepath << std::endl;
        }
    }
    spilledArrays.clear();
}