#ifndef TEMP_H
#define TEMP_H

#include <string>
#include <vector>
#include "../AST/AST.h"

namespace temp {

class Temp {
public:
    int num;
    static int total_num;
    Temp () : num(total_num++) {}
};

using TempList = std::vector<Temp>;

class Label : public ast::Symbol {
public:
    static int labels;
    Label() : Label( "L" + std::to_string( labels++ ) ) {}
    Label(std::string s) : Symbol(s) {}
};

using LabelList = std::vector<Label>;


};

#endif
