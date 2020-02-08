#include <cstdio>
#include <iostream>
#include "../AST/AST.h"
#include "../Canon/canon.h"
#include "../Escapes/escapes.h"
#include "../Semantic/semantic.h"
#include "../Utility/error.h"
#include "tigerbison.tab.h"
extern int yylex();
extern int yyparse();
extern FILE* yyin;
extern int yylineno;

extern void yyerror(const char* s);

extern ast::Expression* ast_raw_ptr;

using namespace std;
void doProc(shared_ptr<frame::Frame> frame, unique_ptr<irt::Statement> body) {
  canon::Canonizator C;
  auto stmList = C.linearize(move(body));
  cout<<endl;
  stmList -> print();
}
int main(int, char**) {
  // open a file handle to a particular file:
  FILE* myfile = fopen("test.file", "r");
  // make sure it's valid:
  if (!myfile) {
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
    cout << "FragList size:"<<frags -> size()<<endl;
    for (const auto& frag : *frags) {
      if (auto proc_frag = dynamic_cast<frame::ProcFrag*>(frag.get())) {
        proc_frag -> body -> print();
        cout << endl;
        doProc(proc_frag->_frame, move(proc_frag->body));
      } else if (auto string_frag = dynamic_cast<frame::StringFrag*>(frag.get())) {
      }
    }
  } catch (error::semantic_error& se) {
    cout << "Semantic error: " << se.getMessage() << endl;
  } catch (error::internal_error& ie) {
    cout << "Internal error: " << ie.getMessage() << endl;
  }

  return 0;
}
