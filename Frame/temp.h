#ifndef TEMP_H
#define TEMP_H

#include <string>
#include <vector>
#include <unordered_map>
#include "../AST/AST.h"

namespace temp {

/** Temporary **/
struct Temp {
    int num;
    static int total_num;
    
    bool operator == (const Temp& t) const { return num == t.num; }
    
    Temp () : num(total_num++) {}
    Temp (int n): num(n) {}
    void print() const { std::cout << num << " "; }
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

static void print_labellist(LabelList const &input)
{
    for (auto const& i: input) {
		i.print();
	}
}
/**
 * Map of temporaries
 * 
 * Mapping from a temporary to a label
 * **/
 
using TempMap = std::unordered_map<Temp, Label, TempHasher>;



};

#endif
