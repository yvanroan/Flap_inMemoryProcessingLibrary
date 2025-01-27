#pragma once

#include <unordered_map>
#include <map>
#include <memory>
#include <variant>
#include "diskSpiller.hpp"
#include "../data/Array.hpp"
#include "../data/Table.hpp"


class memManager{

    private:
    std::unordered_map<size_t, size_t> memTracker; // Track memory usage by ID
    std::unordered_map<size_t, int> accessFrequency; // Track access frequency
    std::unordered_map<size_t, std::shared_ptr<Array>> arrayMap; // Manage Array objects
    std::unordered_map<size_t, std::shared_ptr<Table>> tableMap; // Manage Table objects
    std::multimap<int, size_t> frequencyMap; // Map for LFU eviction

    size_t currentMemory = 0; 
    const size_t memoryThreshold = 1024 * 1024 * 20; // 20MB limit
    static memManager instance; // Singleton instance
    size_t cur_id=0; // Unique identifier for objects

    DiskSpiller& spiller;  // Reference to the DiskSpiller instance

    memManager() : spiller(DiskSpiller::getInstance()) {}

    void evictLFU(); // Handle least-frequently-used eviction

public:
    static memManager& getInstance(); 

    size_t registerInstance(const std::shared_ptr<Array>& array, size_t size);
    size_t registerInstance(const std::shared_ptr<Table>& table, size_t size);

    void updateInstance(size_t id, size_t newSize);
    void removeInstance(size_t id);

    size_t getMemSizeById(size_t id);
    size_t totalMemory();
    void updateFrequency(size_t id);

    bool isInMemory(size_t id);
};



