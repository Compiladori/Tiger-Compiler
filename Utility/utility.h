#ifndef __UTILITY_H__
#define __UTILITY_H__

namespace util {

/**
 * Unique pointer list
 * 
 * Mainly to manage AST + Flex/Bison pointers, claims ownership over raw pointers
 * **/
template <class T>
class GenericList {
    std::deque<std::unique_ptr<T>> data;
public:
    GenericList()     : data() {}
    GenericList(T *e) : GenericList() { this->push_back(e); }
    
    using iterator       = typename std::deque<std::unique_ptr<T>>::iterator;
    using const_iterator = typename std::deque<std::unique_ptr<T>>::const_iterator;
    
    iterator       begin()        { return data.begin(); }
    iterator       end()          { return data.end(); }
    const_iterator begin()  const { return data.cbegin(); }
    const_iterator end()    const { return data.cend(); }
    
    auto  size()    const { return data.size(); } 
    auto  empty()   const { return data.empty(); }
    auto& back()    const { return data.back(); }
    auto& front()   const { return data.front(); }
    
    void push_back(T *e){ data.emplace_back(std::unique_ptr<T>(e)); }
    void push_front(T *e){ data.emplace_front(std::unique_ptr<T>(e)); }
    
    void push_back(std::unique_ptr<T> p){ data.emplace_back(p); }
    void push_front(std::unique_ptr<T> p){ data.emplace_front(p); }
    
    auto& operator[](int i){ return data[i]; }
    
    void print() const {
        std::cout << "List ";
        for(auto& p : data){
            std::cout << "(";
            p -> print(); 
            std::cout << ")";
        }
    }
};

/**
 * Derivated type test
 * **/
template <typename T, typename U>
bool testSameDerivatedTypes(T a, T b){
    // Check if both a and b are of the same type U derivated from T
    return dynamic_cast<U>(a) and dynamic_cast<U>(b);
}

};

#endif
