%code requires{
  #include "../AST/AST.h"
  using namespace ast;
}
%define parse.error verbose
%{
  #include <cstdio>
  #include <iostream>
  #include "../AST/AST.h"
  #include "../Semantic/semantic.h"
  #include "../Escapes/escapes.h"
  #include "../Utility/error.h"
  using namespace std;

  // stuff from flex that bison needs to know about:
  extern int yylex();
  extern int yyparse();
  extern FILE *yyin;
  extern int yylineno;

  void yyerror(const char *s);

  ast::Expression* ast_raw_ptr;
%}

%union {
  int ival;
  char *sval;
  Expression *exp;
  Symbol *symbol;
  TypeField *typefield;
  TypeFieldList *typefieldlist;
  Type *typ;
  FunDec *fundec;
  VarDec *vardec;
  Declaration *dec;
  RecordExp *recordexp;
  GroupedDeclarations *grpdecs;
  Variable *var;
  ExpressionList *exp_list;
  RecordFieldList *recfieldlist;
}

%token TYPE ARRAY OF VAR FUNCTION
%token LET IN END IF THEN ELSE WHILE DO FOR TO BREAK
%token PTO DOSP DOSPIG COMA PCOMA IGUAL PI PD CI CD LI LD
%token AMPER PIPE MENOR MENIG MAYOR MAYIG DIST
%token MAS POR DIV MENOS NIL
%token<ival> INT
%token<sval> LITERAL ID

%nonassoc THEN
%left ELSE
%nonassoc DO
%nonassoc OF
%nonassoc DOSPIG
%left PIPE
%left AMPER
%nonassoc IGUAL MENOR MENIG MAYOR MAYIG DIST
%left MAS MENOS
%left POR DIV
%start prog
%type <exp> exp
%type <symbol> id
%type <typefield> tyfield
%type <typefieldlist> tyflds
%type <typ> ty
%type <fundec> fundec
%type <vardec> vardec
%type <dec> dec
%type <recfieldlist> rec_fields
%type <grpdecs> decs
%type <var> l_value
%type <exp_list> args explist

%%
prog : exp                  { ast_raw_ptr = $1; }

exp : INT					{ $$ = new IntExp($1, yylineno); }
	| PI PD					{ $$ = new UnitExp(yylineno); }
	| NIL					  { $$ = new NilExp(yylineno); }
    | LITERAL				{ $$ = new StringExp($1, yylineno); }
	| BREAK					{ $$ = new BreakExp(yylineno); }
	| l_value				{ $$ = new VarExp($1, yylineno); }
	| l_value DOSPIG exp	{ $$ = new AssignExp($1, $3, yylineno); }
	| PI exp PCOMA explist PD { $4 -> push_front($2);
                                $$ = new SeqExp($4, yylineno); }
	| exp PIPE exp			{ $$ = new IfExp($1, new IntExp(1, yylineno), $3, yylineno); }
	| exp AMPER exp			{ $$ = new IfExp($1, $3, new IntExp(0, yylineno), yylineno); }
	| exp IGUAL exp			{ $$ = new OpExp($1, Eq, $3, yylineno); }
	| exp MENOR exp			{ $$ = new OpExp($1, Lt, $3, yylineno); }
	| exp MENIG exp			{ $$ = new OpExp($1, Le, $3, yylineno); }
	| exp MAYOR exp			{ $$ = new OpExp($1, Gt, $3, yylineno); }
	| exp MAYIG exp			{ $$ = new OpExp($1, Ge, $3, yylineno); }
	| exp DIST exp			{ $$ = new OpExp($1, Neq, $3, yylineno); }
	| exp MAS exp			{ $$ = new OpExp($1, Plus, $3, yylineno); }
	| exp MENOS exp			{ $$ = new OpExp($1, Minus, $3, yylineno); }
	| exp POR exp			{ $$ = new OpExp($1, Times, $3, yylineno); }
	| exp DIV exp			{ $$ = new OpExp($1, Divide, $3, yylineno); }
	| MENOS exp				{ $$ = new OpExp(new IntExp(0, yylineno), Minus, $2, yylineno); }
	| PI exp PD				{ $$ = $2; }
	| id PI args PD			{ $$ = new CallExp($1, $3, yylineno); }
	| IF exp THEN exp		{ $$ = new IfExp($2, $4, yylineno); }
	| IF exp THEN exp ELSE exp { $$ = new IfExp($2, $4, $6, yylineno); }
	| WHILE exp DO exp		{ $$ = new WhileExp($2, $4, yylineno); }
	| FOR id DOSPIG exp TO exp DO exp  { $$ = new ForExp($2, $4, $6, $8, yylineno); }
	| LET decs IN END		{ $$ = new LetExp($2, new UnitExp(yylineno), yylineno); }
	| LET decs IN exp END	{ $$ = new LetExp($2, $4, yylineno); }
	| LET decs IN exp PCOMA explist END  { $6 -> push_front($4); $$ = new LetExp($2, new SeqExp($6, yylineno), yylineno); }
	| l_value CI exp CD OF exp { // Shift/reduce fix
                                 auto simple_var = dynamic_cast<SimpleVar*>($1);
                                 if(not simple_var){ yyerror("Expected simple var on array declaration"); }
                                 string name = simple_var->id->name;
                                 delete simple_var;
                                 $$ = new ArrayExp(new Symbol(name), $3, $6, yylineno);
                               }
    | id LI rec_fields LD	{ $$ = new RecordExp($3, $1, yylineno); }
	;
explist: exp PCOMA explist	{ $3 -> push_front($1); $$ = $3; }
	| exp					{ $$ = new ExpressionList($1); }
	;
rec_fields : id IGUAL exp COMA rec_fields { $5 -> push_front(new RecordField($1, $3));
                                            $$ = $5; }
	| id IGUAL exp			{ $$ = new RecordFieldList(new RecordField($1, $3)); }
	|						{ $$ = new RecordFieldList(); }
	;
decs : dec decs		{ $2 -> frontAppendDeclaration($1); $$ = $2;  }
	|						{ $$ = new GroupedDeclarations(); }
	;
dec : TYPE id IGUAL ty		{ $$ = new TypeDec($2, $4, yylineno); }
	| vardec				{ $$ = $1; }
	| fundec				{ $$ = $1; }
	;
ty : id						{ $$ = new NameType($1, yylineno); }
	| LI tyflds LD			{ $$ = new RecordType($2, yylineno); }
	| ARRAY OF id			{ $$ = new ArrayType($3, yylineno); }
	;
id : ID						{ $$ = new Symbol($1); }
	;
  tyflds : tyfield COMA tyflds { $3->push_front($1); $$ = $3; }
	| tyfield				   { $$ = new TypeFieldList($1); }
	|						 { $$ = new TypeFieldList();   }
	;
vardec : VAR id DOSPIG exp	{ $$ = new VarDec($2, $4, yylineno); }
	| VAR id DOSP id DOSPIG exp { $$ = new VarDec($2, $4, $6, yylineno); }
	;
fundec : FUNCTION id PI tyflds PD IGUAL exp { $$ = new FunDec($2, $4, $7, yylineno); }
	| FUNCTION id PI tyflds PD DOSP id IGUAL exp
							{ $$ = new FunDec($2, $4, $7, $9, yylineno); }
	;
tyfield : id DOSP id		{ $$ = new TypeField($1, $3); }
	;
args : exp COMA args		{ $3 -> push_front($1); $$ = $3; }
	| exp					{ $$ = new ExpressionList($1); }
	|						{ $$ = new ExpressionList(); }
	;
l_value : id				{ $$ = new SimpleVar($1, yylineno); }
	| l_value PTO id		{ $$ = new FieldVar($1, $3, yylineno); }
	| l_value CI exp CD		{ $$ = new SubscriptVar($1, $3, yylineno); }
	;

%%

int main(int, char**) {
  // open a file handle to a particular file:
  FILE *myfile = fopen("test.file", "r");
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
    auto result = SC.translate(final_ast.get());

    // ...
  } catch (error::semantic_error& se) {
    cout << "Semantic error: " << se.getMessage() << endl;
  } catch (error::internal_error& ie) { 
    cout << "Internal error: " << ie.getMessage() << endl;
  }
  
  return 0;
}

void yyerror(const char *s) {
  extern int yylineno;
  extern int line_num;
  cout << "Bison error: " << s << " in line " << yylineno << endl;
  // might as well halt now:
  exit(-1);
}
