%code requires{
  #include "../AST/AST.h"
  using namespace ast;
}
%define parse.error verbose
%{
  #include <cstdio>
  #include <iostream>
  #include "../AST/AST.h"
  #include "../Translation/translation.h"
  using namespace std;

  // stuff from flex that bison needs to know about:
  extern int yylex();
  extern int yyparse();
  extern FILE *yyin;
  extern int yylineno;

  void yyerror(const char *s);
  
  ast::Expression* final_ast;
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
prog : exp                  { final_ast = $1; }

exp : INT					{ $$ = new IntExp($1, Position(yylineno)); }
	| PI PD					{ $$ = new UnitExp(Position(yylineno)); }
	| NIL					  { $$ = new NilExp(Position(yylineno)); }
    | LITERAL				{ $$ = new StringExp($1, Position(yylineno)); }
	| BREAK					{ $$ = new BreakExp(Position(yylineno)); }
	| l_value				{ $$ = new VarExp($1, Position(yylineno)); }
	| l_value DOSPIG exp	{ $$ = new AssignExp($1, $3, Position(yylineno)); }
	| PI exp PCOMA explist PD { $4 -> push_front($2);
                                $$ = new SeqExp($4, Position(yylineno)); }
	| exp PIPE exp			{ $$ = new IfExp($1, new IntExp(1, Position(yylineno)), $3, Position(yylineno)); }
	| exp AMPER exp			{ $$ = new IfExp($1, $3, new IntExp(0, Position(yylineno)), Position(yylineno)); }
	| exp IGUAL exp			{ $$ = new OpExp($1, Eq, $3, Position(yylineno)); }
	| exp MENOR exp			{ $$ = new OpExp($1, Lt, $3, Position(yylineno)); }
	| exp MENIG exp			{ $$ = new OpExp($1, Le, $3, Position(yylineno)); }
	| exp MAYOR exp			{ $$ = new OpExp($1, Gt, $3, Position(yylineno)); }
	| exp MAYIG exp			{ $$ = new OpExp($1, Ge, $3, Position(yylineno)); }
	| exp DIST exp			{ $$ = new OpExp($1, Neq, $3, Position(yylineno)); }
	| exp MAS exp			{ $$ = new OpExp($1, Plus, $3, Position(yylineno)); }
	| exp MENOS exp			{ $$ = new OpExp($1, Minus, $3, Position(yylineno)); }
	| exp POR exp			{ $$ = new OpExp($1, Times, $3, Position(yylineno)); }
	| exp DIV exp			{ $$ = new OpExp($1, Divide, $3, Position(yylineno)); }
	| MENOS exp				{ $$ = new OpExp(new IntExp(0, Position(yylineno)), Minus, $2, Position(yylineno)); }
	| PI exp PD				{ $$ = $2; }
	| id PI args PD			{ $$ = new CallExp($1, $3, Position(yylineno)); }
	| IF exp THEN exp		{ $$ = new IfExp($2, $4, Position(yylineno)); }
	| IF exp THEN exp ELSE exp { $$ = new IfExp($2, $4, $6, Position(yylineno)); }
	| WHILE exp DO exp		{ $$ = new WhileExp($2, $4, Position(yylineno)); }
	| FOR id DOSPIG exp TO exp DO exp  { $$ = new ForExp(new SimpleVar($2), false, $4, $6, $8, Position(yylineno)); }
	| LET decs IN END		{ $$ = new LetExp($2, new UnitExp(Position(yylineno)), Position(yylineno)); }
	| LET decs IN exp END	{ $$ = new LetExp($2, $4, Position(yylineno)); }
	| LET decs IN exp PCOMA explist END  { $6 -> push_front($4); $$ = new LetExp($2, new SeqExp($6, Position(yylineno)), Position(yylineno)); }
	| id CI exp CD OF exp { $$ = new ArrayExp($1, $3, $6, Position(yylineno)); }
    | id LI rec_fields LD	{ $$ = new RecordExp($3, $1, Position(yylineno)); }
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
dec : TYPE id IGUAL ty		{ $$ = new TypeDec($2, $4); }
	| vardec				{ $$ = $1; }
	| fundec				{ $$ = $1; }
	;
ty : id						{ $$ = new NameType($1); }
	| LI tyflds LD			{ $$ = new RecordType($2); }
	| ARRAY OF id			{ $$ = new ArrayType($3); }
	;
id : ID						{ $$ = new Symbol($1); }
	;
  tyflds : tyfield COMA tyflds { $3->push_front($1); $$ = $3; }
	| tyfield				   { $$ = new TypeFieldList($1); }
	|						 { $$ = new TypeFieldList();   }
	;
vardec : VAR id DOSPIG exp	{ $$ = new VarDec($2, false, $4); }
	| VAR id DOSP id DOSPIG exp { $$ = new VarDec($2, false, $4, $6); }
	;
fundec : FUNCTION id PI tyflds PD IGUAL exp { $$ = new FunDec($2, $4, $7); }
	| FUNCTION id PI tyflds PD DOSP id IGUAL exp
							{ $$ = new FunDec($2, $4, $7, $9); }
	;
tyfield : id DOSP id		{ $$ = new TypeField($1, $3); }
	;
args : exp COMA args		{ $3 -> push_front($1); $$ = $3; }
	| exp					{ $$ = new ExpressionList($1); }
	|						{ $$ = new ExpressionList(); }
	;
l_value : id				{ $$ = new SimpleVar($1); }
	| l_value PTO id		{ $$ = new FieldVar($1,$3); }
	| l_value CI exp CD		{ $$ = new SubscriptVar($1,$3); }
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
  
  try {
      // Print the final built AST
      final_ast->print();
      
      // Semantic check
      trans::Translator T;
      auto res = T.transExpression(final_ast);
  } catch (exception& e) {
      cout << "Catched an exception: " << e.what() << endl;
  }
}

void yyerror(const char *s) {
  extern int yylineno;
  extern int line_num;
  cout << s << " in line " << yylineno << endl;
  // might as well halt now:
  exit(-1);
}
