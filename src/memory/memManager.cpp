
#include <unordered_map>
#include <map>
#include <stdexcept>
#include "memManager.hpp"
#include "../data/ArrayInt.hpp"
// #include "diskSpiller.hpp"

memManager memManager::instance;

memManager& memManager::getInstance(){
    return instance;
}

size_t memManager::registerInstance(const std::shared_ptr<Array>& array, size_t size){

    if (currentMemory + size > memoryThreshold) {
        evictLFU();
    }

    size_t id = ++cur_id;
    arrayMap[id] = array;
    memTracker[id] = size;
    accessFrequency[id] = 1;
    frequencyMap.insert({1, id});
    currentMemory += size;

    return id;
}

size_t memManager::registerInstance(const std::shared_ptr<Table>& table, size_t size){

    if (currentMemory + size > memoryThreshold) {
        evictLFU();
    }

    size_t id = ++cur_id;
    tableMap[id] = table;
    memTracker[id] = size;
    accessFrequency[id] = 1;
    frequencyMap.insert({1, id});
    currentMemory += size;

    return id;
}


void memManager::updateInstance(size_t id, size_t newsize ){
    if (!isInMemory(id)) {
        throw std::invalid_argument("Instance ID not found in Memory Manager.");
        
    }
    if (currentMemory - memTracker[id] + newsize > memoryThreshold) {
        evictLFU();
    }
 
    currentMemory += newsize - memTracker[id];

    memTracker[id] = newsize;
    
}

void memManager::removeInstance(size_t id){

    if (!isInMemory(id)) {
        throw std::invalid_argument("Instance ID not found in Memory Manager.");
    }

    size_t size = memTracker[id];
    memTracker.erase(id);

    int frequency = accessFrequency[id];
    accessFrequency.erase(id);

    std::shared_ptr<Array> arr = std::make_shared<ArrayInt>();
    std::shared_ptr<Table> tab = std::make_shared<Table>();

    auto range = frequencyMap.equal_range(frequency);
    for (auto it = range.first; it != range.second; ++it) {
        if (it->second == id) {
            frequencyMap.erase(it);
            break;
        }
    }

    currentMemory -= size;

    if (arrayMap.find(id) != arrayMap.end()) {
        arr = arrayMap[id];
        arrayMap.erase(id);
        spiller.collect(id, arr);

    }else{
        tab = tableMap[id];
        tableMap.erase(id);
        spiller.collect(id, tab);
    }

}

//still thinking if i am gonna need this
size_t memManager::getMemSizeById(size_t id){

    if (!isInMemory(id)) {
        throw std::invalid_argument("Instance ID not found.");
    }

    return memTracker[id];
}

size_t memManager::totalMemory(){
    return currentMemory;
}

void memManager::evictLFU() {
    while (currentMemory > memoryThreshold) {
        if (frequencyMap.empty()) {
            throw std::runtime_error("Cannot evict: No objects to remove.");
        }

        auto it = frequencyMap.begin();
        size_t id = it->second;
        removeInstance(id);
    }
}

void memManager::updateFrequency(size_t id) {
    int frequency = accessFrequency[id];
    accessFrequency[id]++;

    auto range = frequencyMap.equal_range(frequency);
    for (auto it = range.first; it != range.second; ++it) {
        if (it->second == id) {
            frequencyMap.erase(it);
            break;
        }
    }
    frequencyMap.emplace(frequency + 1, id);

}

bool memManager::isInMemory(size_t id){
    return !(memTracker.find(id)==memTracker.end());
}