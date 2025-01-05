#ifndef MEM_HPP
#define MEM_HPP

#include <unordered_map>
#include <map>
#include <memory>
#include <variant>
#include "Array.hpp"
#include "Table.hpp"

using ObjectType = std::variant<Array, Table>;

class memManager{

    private:
        std::unordered_map<size_t, size_t> memTracker;

        std::unordered_map<size_t, int> accessFrequency;

        std::unordered_map<size_t, std::unique_ptr<ObjectType>> objectMap;

        std::multimap<int, size_t> frequencyMap;

        size_t currentMemory = 0;

        const size_t memoryThreshold = 1024 * 1024 * 20;  //20MB

        static memManager instance;

        static size_t cur_id;

        memManager() = default;

        void evictLFU();

    public:

        static memManager& getInstance();
        size_t registerInstance(size_t size);
        void updateInstance(size_t id, size_t newsize );
        void removeInstance(size_t id);
        //might remove this
        size_t getMemSizeById(size_t id);
        size_t totalMemory();
        void updateFrequency(size_t id);
        void memManager::updateFrequency(size_t id);
        ObjectType getObject(size_t id);

}

#endif


