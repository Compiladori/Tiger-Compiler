#include <cstdio>
#include <iostream>

#include "AST/AST.h"
#include "Canon/canon.h"
#include "Escapes/escapes.h"
#include "FileHandler/file_handler.h"
#include "Munch/munch.h"
#include "RegAlloc/regalloc.h"
#include "Semantic/semantic.h"
#include "Syntax/tigerbison.tab.h"
#include "Utility/error.h"

extern FILE* yyin;
extern int yylineno;

extern void yyerror(const char* s);

extern ast::Expression* ast_raw_ptr;

using namespace std;

void doProc(file::Handler& out, shared_ptr<frame::Frame> frame, unique_ptr<irt::Statement> body) {
    cout << "Entered doProc!!!!" << endl;
    cout << endl;
    cout << endl;
    canon::Canonizator C;
    auto stmList = C.linearize(move(body));
    cout << endl;
    stmList->print();
    cout << endl;
    cout << "Linerize !!!!" << endl;
    cout << endl;
    cout << endl;
    auto basic_b = C.basicBlocks(move(stmList));
    basic_b->stmLists->print();
    cout << endl;
    cout << "BasicBLock List!!!!" << endl;
    cout << endl;
    cout << endl;
    auto stm_list = C.traceSchedule(move(basic_b));
    stm_list->print();
    cout << endl;
    cout << "traceSchedule!!!!" << endl;
    cout << endl;
    cout << endl;
    munch::Muncher MN(*frame);
    auto instr_list = MN.munchStatementList(move(*stm_list.get()));
    cout << endl;
    cout << "Muncher!!!!" << endl;
    cout << endl;
    cout << endl;
    assem::InstructionList shrd_list;
    for ( auto& inst : instr_list ) {
        shrd_list.push_back(move(inst));
    }
    shrd_list = frame::proc_entry_exit2(frame, shrd_list);
    regalloc::RegAllocator RA;
    auto ra_result = RA.regAllocate(*frame, shrd_list);
    cout << "Regallocate!!!!" << endl;
    cout << endl;
    cout << endl;
    auto coloring = ra_result.coloring;
    auto proc = frame::proc_entry_exit3(frame, ra_result.instruction_list);
    out.print_proc(move(proc), coloring);
    cout << "Print!!!!" << endl;
    cout << endl;
    cout << endl;
}

int main(int argc, char** argv) {
    file::Handler out("out.s");
    FILE* myfile = fopen("test.file", "r");
    // make sure it's valid:
    if ( !myfile ) {
        cout << "I can't open a.snazzle.file!" << endl;
        return -1;
    }
    // Set flex to read from it instead of defaulting to STDIN:
    yyin = myfile;

    // Parse through the input:
    yyparse();

    // TODO: Separate into a different main file
    try {
        unique_ptr<ast::Expression> final_ast(ast_raw_ptr);

        // Print the final built AST
        final_ast->print();
        cout << endl;

        // Set variable escapes
        esc::Escapator E;
        E.setEscapes(final_ast.get());

        // Semantic check
        seman::SemanticChecker SC;
        auto frags = SC.translate(final_ast.get());
        // Canonical conversion
        cout << "FragList size:" << frags->size() << endl;
        bool head_text = true, head_data = true;
        for ( const auto& frag : *frags ) {
            if ( auto proc_frag = dynamic_cast<frame::ProcFrag*>(frag.get()) ) {
                proc_frag->body->print();
                cout << endl;
                if ( head_text ) {
                    out.print_text_header();
                    head_text = false;
                }
                doProc(out, proc_frag->_frame, move(proc_frag->body));
            } else if ( auto string_frag = dynamic_cast<frame::StringFrag*>(frag.get()) ) {
                if ( head_data ) {
                    out.print_data_header();
                    head_data = false;
                }
                out.print_str(*string_frag);
            }
        }
    } catch ( error::semantic_error& se ) {
        cout << "Semantic error: " << se.getMessage() << endl;
    } catch ( error::internal_error& ie ) {
        cout << "Internal error: " << ie.getMessage() << endl;
    }
    fclose(myfile);
    return 0;
}
