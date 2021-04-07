#include "assem.h"

using namespace assem;

/**
 * Instructions
 * **/
void Oper::output(std::ostream& os, temp::TempMap temp_map) const {
    std::string result;
    for ( auto it = assm.cbegin(); it != assm.cend(); ++it ) {
        if ( *it == '\'' ) {
            switch ( *(++it) ) {
                case 's': {
                    int n = std::atoi(&*(++it));
                    std::string s = temp_map.at(src[n]).name;
                    result.append(s);
                } break;
                case 'd': {
                    int n = std::atoi(&*(++it));
                    std::string s = temp_map.at(dst[n]).name;
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
    os <<"    " << result << "\n";
}

void Label::output(std::ostream& os, temp::TempMap temp_map) const {
    os << assm << "\n";
}

void Move::output(std::ostream& os, temp::TempMap temp_map) const {
    std::string result;
    for ( auto it = assm.cbegin(); it != assm.cend(); ++it ) {
        if ( *it == '\'' ) {
            switch ( *(++it) ) {
                case 's': {
                    int n = std::atoi(&*(++it));
                    std::string s = temp_map.at(src[n]).name;
                    result.append(s);
                } break;
                case 'd': {
                    int n = std::atoi(&*(++it));
                    std::string s = temp_map.at(dst[n]).name;
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
    os <<"    " << result << "\n";
}

void Oper::print() const { std::cout << "Oper( " + assm + " ) ( "; temp::print_templist(src);  temp::print_templist(dst);  temp::print_labellist(jumps); std::cout << " )"; }
void Label::print() const { std::cout << "Label( "+ label.name + " )"; }
void Move::print() const { std::cout << "Move( "; temp::print_templist(src);  temp::print_templist(dst); std::cout << " )"; }
