#ifndef TEMP_H
#define TEMP_H

#include <string>
#include <unordered_map>
#include <vector>
#include "../AST/AST.h"

namespace temp {

/** Temporary **/
struct Temp {
  int num;
  static int total_num;

  bool operator==(const Temp &t) const { return num == t.num; }

  Temp() : num(total_num++) {}
  Temp(int n) : num(n) {}

  // Define custom copy constructor and copy assignment just in case by default the static total_num is changed
  Temp(const Temp &t) : num(t.num) {} // Copy constructor
  Temp& operator=(const Temp &t) { num = t.num; return *this; } // Copy assignment

  void print() const { std::cout << num << " "; }
};

struct TempHasher {
  std::size_t operator()(const Temp &s) const {
    return std::hash<int>()(s.num);
  }
};

using TempList = std::vector<Temp>;

/** Label **/
struct Label : public ast::Symbol {
  static int labels;
  bool operator <(const Label& rhs) const{
        return name < rhs.name;
    }
  Label() : Label("L" + std::to_string(labels++)) {}
  Label(std::string s) : Symbol(s) {}

  // Define custom copy constructor and copy assignment just in case by default the static labels is changed
  Label(const Label &l) : Label(l.name) {} // Copy constructor
  Label& operator=(const Label &l) { name = l.name; return *this; } // Copy assignment
};

using LabelList = std::vector<Label>;

static void print_labellist(LabelList const &input) {
  for (auto const &i : input) {
    i.print();
  }
}
/**
 * Map of temporaries
 * 
 * Mapping from a temporary to a label
 * **/

using TempMap = std::unordered_map<Temp, Label, TempHasher>;

};  // namespace temp

#endif
