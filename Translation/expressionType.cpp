#include "expressionType.h"

using namespace trans;

/**
 * Overriden ExpType == operator (particular equality cases)
 * **/
bool ArrayExpType::operator==(const ExpType& exp_type) const {
    if(this->kind != exp_type.kind)
        return false;
    
    return *this->type == *static_cast<const ArrayExpType*>(&exp_type)->type;
}

bool RecordExpType::operator==(const ExpType& exp_type) const {
    if(this->kind != exp_type.kind)
        return false;
    
    return this == &exp_type;
}

bool CustomExpType::operator==(const ExpType& exp_type) const {
    if(this->kind != exp_type.kind)
        return false;
    
    return this->name == static_cast<const CustomExpType*>(&exp_type)->name;
}
