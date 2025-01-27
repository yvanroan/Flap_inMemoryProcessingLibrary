#pragma once

#include <vector>
#include <string>
#include <variant>
#include <memory>
#include <any>
#include "Array.hpp"

class Series{

    private: 
        std::shared_ptr<Array> arr;


    protected:

    public:
        Series();
        Series(Series& s);
        Series(std::shared_ptr<Array> array);
        Series(std::vector<int> input);
        Series(std::vector<std::string> input);
        Series(std::vector<float> input);
        void append(int data);
        void append(std::string data);
        void append(float data);
        void extend( Series& col);

        size_t size();
        std::string typedef_() const; 

        std::vector<size_t> filteredIndex(std::function<bool(const int&)> f);
        void map(std::function<void(int&)> f);
        void filter(std::function<bool(const int&)> f); 
        int aggregate(std::function<int(int, int)> aggFunc, int initial) const;

        std::vector<size_t> filteredIndex(std::function<bool(const float&)> f);
        void map(std::function<void(float&)> f);
        void filter(std::function<bool(const float&)> f); 
        float aggregate(std::function<float(float, float)> aggFunc, float initial) const;
        
        std::vector<size_t> filteredIndex(std::function<bool(const std::string&)> f);
        void map(std::function<void(std::string&)> f);
        void filter(std::function<bool(const std::string&)> f); 
        std::string aggregate(std::function<std::string(std::string, std::string)> aggFunc, std::string initial) const;
        
        void removeByIndex(const std::vector<size_t> indexes);
        void removeByIndex(int index);


        std::shared_ptr<Array> getArray();
        // std::any collect();
        
};
