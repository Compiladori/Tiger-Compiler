#ifndef TEMP_H
#define TEMP_H

#include <string>
#include <vector>
#include "../AST/AST.h"

namespace temp {

/** Temporary **/
struct Temp {
    int num;
    static int total_num;
    
    bool operator == (const Temp& t) const { return num == t.num; }
    
    Temp () : num(total_num++) {}
};

struct TempHasher{
    std::size_t operator()(const Temp& s) const {
        return std::hash<int>()(s.num);
    }
};

using TempList = std::vector<Temp>;


/** Label **/
struct Label : public ast::Symbol {
    static int labels;
    
    Label() : Label( "L" + std::to_string( labels++ ) ) {}
    Label(std::string s) : Symbol(s) {}
};

using LabelList = std::vector<Label>;

/**
 * Map of temporaries
 * 
 * Mapping from a temporary to a label
 * **/
 
using TempMap = std::unordered_map<Temp, Label, TempHasher>;



};

#endif
