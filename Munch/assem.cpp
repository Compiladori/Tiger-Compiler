#include "assem.h"

using namespace assem;

/**
 * Instructions
 * **/
void Oper::print(std::ostream& os, temp::TempMap& temp_map) const {
    std::string result;
    for ( auto it = assm.cbegin(); it != assm.cend(); ++it ) {
        if ( *it == '\'' ) {
            switch ( *(++it) ) {
                case 's': {
                    int n = std::atoi(&*(++it));
                    std::string s = temp_map[src[n]].name;
                    result.append(s);
                } break;
                case 'd': {
                    int n = std::atoi(&*(++it));
                    std::string s = temp_map[dst[n]].name;
                    result.append(s);
                } break;
                case 'j': {
                    int n = std::atoi(&*(++it));
                    std::string s = jumps[n].name;
                    result.append(s);
                } break;
                case '\'': {
                    result.append(1, *it);
                } break;
            }
        } else {
            result.append(1, *it);
        }
    }
    os << result;
}

void Label::print(std::ostream& os, temp::TempMap& temp_map) const {
    os << assm;
}

void Move::print(std::ostream& os, temp::TempMap& temp_map) const {
    std::string result;
    for ( auto it = assm.cbegin(); it != assm.cend(); ++it ) {
        if ( *it == '\'' ) {
            switch ( *(++it) ) {
                case 's': {
                    int n = std::atoi(&*(++it));
                    std::string s = temp_map[src[n]].name;
                    result.append(s);
                } break;
                case 'd': {
                    int n = std::atoi(&*(++it));
                    std::string s = temp_map[dst[n]].name;
                    result.append(s);
                } break;
                case '\'': {
                    result.append(1, *it);
                } break;
            }
        } else {
            result.append(1, *it);
        }
    }
    os << result;
}