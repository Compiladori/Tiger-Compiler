#ifndef __UTILITY_H__
#define __UTILITY_H__

#include <deque>
#include <iostream>

namespace util {

/**
 * Unique pointer generic list
 *
 * Mainly to manage AST + Flex/Bison pointers, claims ownership over raw pointers
 * **/
template <class T>
class GenericList {
  std::deque<std::unique_ptr<T>> data;

 public:
  GenericList() = default;
  GenericList(T* e) : GenericList() { this->push_back(e); }

  using iterator = typename std::deque<std::unique_ptr<T>>::iterator;
  using reverse_iterator = typename std::deque<std::unique_ptr<T>>::reverse_iterator;
  using const_iterator = typename std::deque<std::unique_ptr<T>>::const_iterator;

  iterator begin() { return data.begin(); }
  iterator end() { return data.end(); }
  reverse_iterator rbegin() { return data.rbegin(); }
  reverse_iterator rend() { return data.rend(); }
  const_iterator begin() const { return data.cbegin(); }
  const_iterator end() const { return data.cend(); }

  auto size() const { return data.size(); }
  auto empty() const { return data.empty(); }
  auto& back() const { return data.back(); }
  auto& front() const { return data.front(); }
  auto clear() { return data.clear(); }

  void push_back(T* e) { data.emplace_back(std::unique_ptr<T>(e)); }
  void push_front(T* e) { data.emplace_front(std::unique_ptr<T>(e)); }

  void push_back(std::unique_ptr<T> p) { data.emplace_back(std::move(p)); }
  void push_front(std::unique_ptr<T> p) { data.emplace_front(std::move(p)); }

  auto pop_front() {
    std::unique_ptr<T> temp_ptr(move(data.front()));
    data.pop_front();
    return move(temp_ptr);
  }
  auto pop_back() {
    std::unique_ptr<T> temp_ptr(move(data.back()));
    data.pop_back();
    return move(temp_ptr);
  }
  auto& operator[](int i) { return data[i]; }

  auto erase(iterator i) { data.erase(i); };

  void print() const {
    std::cout << "List ";
    for (auto& p : data) {
      std::cout << "(";
      p->print();
      std::cout << "), ";
    }
  }
};

/**
 * Derivated type test
 * **/
template <typename T, typename U>
bool testSameDerivatedTypes(T a, T b) {
  // Check if both a and b are of the same type U derivated from T
  return dynamic_cast<U>(a) and dynamic_cast<U>(b);
}

};  // namespace util

#endif
