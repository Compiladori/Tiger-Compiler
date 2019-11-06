#ifndef TEMP_H
#define TEMP_H

#include <string>
#include "../AST/AST.h"

namespace temp {

class Temp {
  public:
    int num;
    static int total_num;
    Temp () : num(total_num++) {}
};

class Label : public ast::Symbol {
  public:
    static int labels;
    Label() : Label( "L" + std::to_string( labels++ ) ) {}
    Label(std::string s) : Symbol(s) {}
};


};

#endif
