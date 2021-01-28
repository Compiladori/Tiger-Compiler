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
    }
    // void print_proc(tipo proc, temp::TempMap coloring) {
    //     _file << "#BEGIN function\n";
    //     print_instr_list(ra.coloring);
    //     _file << proc->prolog;
    //     _file << proc->epilog;
    //     _file << "#END function\n\n";
    // }

    // void print_instr_list (temp::TempMap coloring) {
    //     AS_printInstrList(
    //         out, proc->body,
    //         Temp_layerMap(F_tempMap, coloring));

    // }
};

};    // namespace file

#endif
