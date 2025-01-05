
#include <unordered_map>
#include <map>
#include <stdexcept>
#include "memManager.hpp"
#include "diskSpiller.hpp"

memManager memManager::instance;
size_t memManager::cur_id=0;

static memManager& memManager::getInstance(){
    return instance;
}

size_t memManager::registerInstance(size_t size){

    size_t id = ++cur_id; 
    memTracker[id] = size;

    accessFrequency[id] = 1;
    frequencyMap.emplace(1, id);
    currentMemory += size;

    // mem check
    if (currentMemory > memoryThreshold) {
        evictLFU();
    }

    return id;
}

void memManager::updateInstance(size_t id, size_t newsize ){
    if (memTracker.find(id) == memTracker.end()) {
        throw std::invalid_argument("Instance ID not found.");
        
    }

    currentMemory -= memTracker[id];
    currentMemory += newsize;

    memTracker[id] = newsize;

    // mem check
    if (currentMemory > memoryThreshold) {
        evictLFU();
    }
}

void memManager::removeInstance(size_t id){

    if (memTracker.find(id) == memTracker.end()) {
        throw std::invalid_argument("Instance ID not found.");
    }

    size_t size = memTracker[id];
    memTracker.erase(id);

    int frequency = accessFrequency[id];
    accessFrequency.erase(id);

    objectMap.erase(id);


    auto range = frequencyMap.equal_range(frequency);
    for (auto it = range.first; it != range.second; ++it) {
        if (it->second == id) {
            frequencyMap.erase(it);
            break;
        }
    }

    currentMemory -= size;
}

//still thinking if i am gonna need this
size_t getMemSizeById(size_t id) const{
    if (memTracker.find(id) == memTracker.end()) {
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
        ObjectType obj = getObject(id);

        removeInstance(id);

        diskSpiller::getInstance().collect(id, obj);
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

ObjectType memManager::getObject(size_t id) {
    auto it = objectMap.find(id);
    
    if (it == objectMap.end()) {
        throw std::runtime_error("Object not found");
    }

    ObjectType result = *(it->second); 

    return result;
}
