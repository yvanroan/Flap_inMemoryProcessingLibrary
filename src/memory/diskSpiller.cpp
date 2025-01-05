#include "diskSpiller.hpp"
#include "serializer.hpp"
#include <string>
#include <fstream>


DiskSpiller DiskSpiller::instance;

static DiskSpiller& getInstance() {
    return instance;
}

std::string DiskSpiller::generateFilePath(size_t id) const {
    return "spill_" + std::to_string(id) + ".bin";
}

void DiskSpiller::collect(size_t id, ObjectType obj) {
    evictedObjects[id] = std::move(obj);

    if (evictedObjects.size() >= spillThreshold) {
        for (const auto& [key, obj]: evictedObjects) {
            std::string filePath = generateFilePath(key);
            serializeToFile(batch[i], filePath);
            spilled.emplace_back(key);
        }
        evictedObjects.clear();
    }
}

void DiskSpiller::serializeToFile(const ObjectType& obj, const std::string& filePath) {
    std::ofstream outFile(filePath, std::ios::binary);
    if (!outFile.is_open()) {
        throw std::runtime_error("Failed to open file for writing: " + filePath);
    }

    std::visit([&outFile](auto&& value) {
        using T = std::decay_t<decltype(value)>;
        if constexpr (std::is_same_v<T, Array>) {
            outFile << "Array\n"; 
            outFile << serializer::arrayToBinary(value); 
        } else if constexpr (std::is_same_v<T, Table>) {
            outFile << "Table\n";
            outFile << serializer::arrayToBinary(value);  
        }
    }, obj);

    outFile.close();
}

ObjectType DiskSpiller::deserializeFromFile(const std::string& filePath) {
    std::ifstream inFile(filePath, std::ios::binary);
    if (!inFile.is_open()) {
        throw std::runtime_error("Failed to open file for reading: " + filePath);
    }
    std::string schema;
    std::getline(inFile, schema);
    std::string binaryData((std::istreambuf_iterator<char>(inFile)),
                            std::istreambuf_iterator<char>());

    inFile.close();

    ObjectType obj;
    if (schema == "Array") {
        obj = serializer::binaryToArray(binaryData);
    } else if (schema == "Table") {
        obj = serializer::binaryToTable(binaryData);
    } else {
        throw std::runtime_error("Unknown schema: " + schema);
    }

    return obj;
}

ObjectType DiskSpiller::accessObject(size_t id) {
    

    if (evictedObjects.find(id) != evictedObjects.end()) {
        return evictedObjects[id];
    } else {
        std::string filePath = generateFilePath(id);
        ObjectType obj = deserializeFromFile(filePath);
        return obj;
    }
}

void DiskSpiller::cleanup() {
    for (const auto& id : spilled) {
        std::string filepath = generateFilePath(id);
        if (std::remove(filePath.c_str()) != 0) {
            std::cerr << "Failed to delete file: " + filePath << std::endl;
        }
    }
    spilled.clear();
}