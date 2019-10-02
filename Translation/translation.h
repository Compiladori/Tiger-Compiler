#ifndef __TRANSLATION_H__
#define __TRANSLATION_H__

#include <map>
#include "../AST/AST.h"

namespace trans{


template <class T>
class BindingTable {
    std::map<ast::Symbol, T> table;
public:
    BindingTable();
    void insert(ast::Symbol&, T&);
    T get(ast::Symbol&);
};

typedef BindingTable<ast::Type> TypeEnvironment; // Symbol -> Type
// typedef BindingTable<> ValueEnvironment; // Symbol -> Value (?)



};

#endif
