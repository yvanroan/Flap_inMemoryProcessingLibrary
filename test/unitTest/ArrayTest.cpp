#include <iostream>
#include <vector>
#include <string>
#include <functional>
#include <string>
#include <sstream>
#include "data/ArrayInt.hpp"
#include "data/ArrayString.hpp"
#include "data/ArrayFloat.hpp"

int main () {

    std::shared_ptr<ArrayInt> arr1 = std::make_shared<ArrayInt>();
    
    int base = 10;
    std::vector<int> vecs;
    vecs.emplace_back(base);

    std::shared_ptr<ArrayInt> arr2 = std::make_shared<ArrayInt>(vecs);
    arr1->append(base);


    if(arr1->getSize() != arr2->getSize()){
        
        printf("size doesnt work not same size\n");

        printf("arr1 size: %ld",arr1->getSize());
        std::cout << arr1;
        printf("arr2 size: %ld",arr2->getSize());
        std::cout << arr2;
        return 0;
    }

    if(arr1->getSize()!=1){
        printf("size doesnt work, size not 1");

        printf("arr1 size: %ld",arr1->getSize());
        std::cout << arr1;
        printf("arr2 size: %ld",arr2->getSize());
        std::cout << arr2;
        return 0;
    }

    arr1->removeByIndex(0);

    if(arr1->getSize() == arr2->getSize()){
        printf("removeByIndex didn't work");

        printf("arr1 size: %ld",arr1->getSize());
        std::cout << *arr1;
        printf("arr2 size: %ld",arr2->getSize());
        std::cout << *arr2;
        return 0;
    }

    arr1->extend(*arr2);

    if(arr1->getSize() != arr2->getSize()){
        printf("arr1 just got extended and should be equal");

        printf("arr1 size: %ld",arr1->getSize());
        std::cout << *arr1;
        printf("arr2 size: %ld",arr2->getSize());
        std::cout << *arr2;
        return 0;
    }

    arr1->removeByIndex(0);
    std::vector<int> vec;

    for(int i =0; i< 10; ++i){
        vec.push_back(i);
        arr1->append(i);
    }

    arr2 = std::make_shared<ArrayInt>(vec);

    if(arr1->getSize() != arr2->getSize()){
        printf("arr1 and arr2 should have the same values");

        printf("arr1 size: %ld",arr1->getSize());
        std::cout << *arr1;
        printf("arr2 size: %ld",arr2->getSize());
        std::cout << *arr2;
        return 0;
    }

    if(*arr1 != *arr2){
        printf("the arrays are not equal");
        return 0;
    }

    std::vector<int> arr1_vec = arr1->getArray();
    std::vector<int> arr2_vec = arr2->getArray();


    for (int i = 0; i < arr1->getSize(); ++i) {
        
        if((*arr1)[i] != arr1_vec[i]) {
            printf("the values in arr1 are not correct\n");
            return 0;
        } 
    }

    arr1->map(
        [](int& input) {  
            input*=2;
        }
    );

    
    arr2->map(
        [](int& value) { 
            value /= 2;
        }
    );


    for (int i = 0; i < arr1->getSize(); ++i) {
        if((*arr1)[i] != arr1_vec[i] * 2){
            printf("map didn't work properly in arr1\n");
            std::cout << *arr1;
            // std::cout << arr1_vec;
            return 0;
        }

        if((*arr2)[i] != arr2_vec[i] / 2){
            printf("map didn't work properly in arr2\n");
            std::cout << *arr1;
            // std::cout << arr1_vec;
            return 0;
        }
    }
   

    std::function<int(int)> filter1 = [](int a){ return a%3==1; };
    std::function<int(int)> filter2 = [](int a){ return a%3==2; };

    arr1->filter(filter1);
    arr2->filter(filter2);

    std::vector<size_t> vec1_idx =  arr1->filteredIndex(filter1);
    std::vector<size_t> vec2_idx =  arr2->filteredIndex(filter2);

    int j=0;

    for(size_t i: vec1_idx ){

        if(arr1_vec[i] != (*arr1)[j]){
            if(!filter1((*arr1)[j])){
                printf("filter didn't work in arr1");
                return 0;
            }
            j+=1;
            printf("filteredIndex didn't work properly in arr1");
            return 0;
        }
    }

    j=0;

    for(size_t i: vec2_idx ){

        if(arr2_vec[i] != (*arr2)[j]){
            std::cout << *arr2;
            if(!filter2((*arr2)[j])){
                printf("filter didn't work in arr2");
                return 0;
            }
            j+=1;
            printf("filteredIndex didn't work properly in arr2");
            return 0;
        }
    }

    std::shared_ptr<Array> arr = std::make_shared<ArrayInt>(*arr1);
    std::shared_ptr<ArrayInt> arr3 = std::dynamic_pointer_cast<ArrayInt>(arr);
    

    std::function<int(int, int)> aggFunc = [](int a, int b){ return a+b; };

    int aggsum3 = arr3->aggregate(aggFunc, 0);
    int aggsum2 = arr2->aggregate(aggFunc, 0);

    int sum1=0;
    int sum2=0;

    for(auto c: arr3->getArray()){
        sum1+=c;
    }

    for(auto c: arr2->getArray()){
        sum2+=c;
    }

    if(sum1 != aggsum3){
        printf("aggregate didn't work for arr1\n");
        std::cout << sum1<< ", arr sum: " << aggsum3;
        return 0;
    }

    if(sum2 != aggsum2){
        printf("aggregate didn't work for arr2");
        std::cout << sum2<< ", arr sum: " << aggsum2;

        return 0;
    }

    aggFunc = [](int a, int b){ return a*b; };

    int aggmul3 = arr3->aggregate(aggFunc, 1);
    int aggmul2 = arr2->aggregate(aggFunc, 1);
    int mult1=1;
    int mult2=1;

    for(auto c: arr3->getArray()){
        mult1*=c;
    }

    for(auto c: arr2->getArray()){
        mult2*=c;
    }

    if(mult1 != aggmul3){
        printf("aggregate mult didn't work for arr1");
        std::cout << mult1<< ", arr sum: " << aggmul3;
        return 0;
    }

    if(mult2 != aggmul2){
        printf("aggregate mult didn't work for arr2");
        std::cout << mult2<< ", arr sum: " << aggmul2;
        return 0;
    }


}