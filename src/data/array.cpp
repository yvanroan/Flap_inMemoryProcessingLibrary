#include <iostream>
#include <vector>


template <typename T>
template <typename Func>

Array::Array(const std::vector<std::optional<T>>& input){

    for(auto e: input){
        sequence.emplace_back(e);
    }
    size += input.size();
}

void Array<std::optional<T>>::append(std::optional<T> data){
    sequence.emplace_back(data);
    size++;
}

void Array<std::optional<T>>::appendNull(){
    sequence.emplace_back(std::nullopt);
    size++;
}
void Array<std::optional<T>>::append(const Array<std::optional<T>>& arr){

    for(auto e: arr.sequence){
        sequence.emplace_back(e);
    }
    size += arr.size();
}
Array<std::optional<T>> Array<std::optional<T>>::map(Func f){
    Array<std::optional<T>> copy;
    for(const auto e: this.sequence){
        if(e.has_value()){
            copy.append(f(e.value()));
        }
    }

    return copy;
}
Array<std::optional<T>> Array<std::optional<T>>::filter(Func f) {
    Array<std::optional<T>> copy;
    for(const auto e: this.sequence){
        if(f(e)){
            copy.append(e);
        }
    }
    return copy;
}

//will implement aggregate later

size_t Arrat<std::optional<T>>::size(){
    return size;
}

std::optional<T> Array<std::optional<T>>::getByIndex(int idx) const{
    if (idx >= sequence.size()) {
        throw std::out_of_range("Index out of range");
    }
    return sequence[idx];
}

size_t Array<std::optional<T>>::memory_usage() const{
    if(size == 0){
        return 0;
    }
    return size * sizeof(sequence[0])
} 

void Array<std::optional<T>>::fill_nulls( T val){
    for(size_t i=0; i<n; i++){
        if(!sequence[i].has_value()){
            sequence[i] = val;
        }
    }
}

bool Array<std::optional<T>>::is_null(int idx){
    if (idx >= sequence.size()) {
        throw std::out_of_range("Index out of range");
    }
    return !sequence[idx].has_value()

}