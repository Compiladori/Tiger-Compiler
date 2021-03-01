#ifndef __FILE_HANDLER_H__
#define __FILE_HANDLER_H__
#include <fstream>

#include "../RegAlloc/regalloc.h"

/**
 * Output and Input file handler
 * */

namespace file {

struct Handler {
    std::ofstream _file;
    Handler(char* file_name) {
        _file.open(file_name);
    };
    ~Handler() {
        _file.close();
    };
    void print_proc(std::unique_ptr<assem::Procedure> proc, temp::TempMap coloring) {
        _file << "#BEGIN function\n";
        _file << proc -> prolog;
        for ( auto i = proc -> body.begin(); proc -> body.end() != i; i++ ) {
            (*i)->print(_file, coloring);    // add Temp_layerMap
        }
        _file << proc -> epilog;
        _file << "#END function\n\n";
    };
    void print_str(frame::StringFrag string_frag) {
        _file << string_frag._label.name + ": .ascii " + "'\'" + string_frag.str + "'\'" + "\n";
    };

    void print_text_header() {
        _file << ".text\n";
        _file << ".global tigermain\n";
        _file << ".type tigermain, @function\n";
    }
    void print_data_header() {
        _file << ".section .rodata\n";
    }

};
}    // namespace file
#endif
