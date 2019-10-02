%code requires{
  #include "../AST/AST.h"
  using namespace ast;
}

%{
  #include <cstdio>
  #include <iostream>
  using namespace std;

  // stuff from flex that bison needs to know about:
  extern int yylex();
  extern int yyparse();
  extern FILE *yyin;
  extern int yylineno;

  void yyerror(const char *s);
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
  VariableList *varlist;
}

%token TYPE ARRAY OF VAR FUNCTION END_OF_FILE
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
%type <varlist> rec_fields
%type <grpdecs> decs
%type <var> l_value
%type <exp_list> args explist

%%
prog : exp END_OF_FILE

exp : INT					{ $$ = new IntExp($1,Position(yylineno)); }
	| PI PD					{ $$ = new UnitExp(Position(yylineno)); }
	| NIL					  { $$ = NilExp(Position(yylineno)); }
  | LITERAL				{ $$ = StringExp(Position(yylineno)); }
	| BREAK					{ $$ = BreakExp(Position(yylineno));}
	| l_value				{ $$ = VarExp($1,Position(yylineno)); }
	| l_value DOSPIG exp	{ $$ = AssignExp($1,$3,Position(yylineno)); }
	| PI exp PCOMA explist { $4 -> push_back($2);
                           $$ = SeqExp($4,Position(yylineno)); }
	| exp PIPE exp			{ $$ = IfExp($1,new IntExp(1),$3,Position(yylineno)); }
	| exp AMPER exp			{ $$ = IfExp($1,$3, new IntExp(0), Position(yylineno)); }
	| exp IGUAL exp			{  }
	| exp MENOR exp			{  }
	| exp MENIG exp			{  }
	| exp MAYOR exp			{  }
	| exp MAYIG exp			{  }
	| exp DIST exp			{  }
	| exp MAS exp			{  }
	| exp MENOS exp			{  }
	| exp POR exp			{  }
	| exp DIV exp			{  }
	| MENOS exp				{  }
	| PI exp PD				{  }
	| id PI args PD			{  }
	| IF exp THEN exp		{ }
	| IF exp THEN exp ELSE exp {  }
	| WHILE exp DO exp		{  }
	| FOR id DOSPIG exp TO exp DO exp  {  }
	| LET decs IN END		{ }
	| LET decs IN exp END	{ }
	| LET decs IN exp PCOMA explist END  { }
	| l_value CI exp CD OF exp { }
  | id LI rec_fields LD	{  }
	;
explist: exp PCOMA explist	{ $3 -> push_back($1); $$ = $3; }
	| exp					{ $$ = new ExpressionList($1); }
	;
rec_fields : id IGUAL exp COMA rec_fields { SimpleVar *svar = new SimpleVar($1);
                                            SubscriptVar *susvar = new SubscriptVar(svar,$3);
                                            $5 -> push_back(susvar);
                                            $$ = $5; }
	| id IGUAL exp			{ SimpleVar *svar = new SimpleVar($1);
                        SubscriptVar *susvar = new SubscriptVar(svar,$3);
                        $$ = new VariableList(susvar); }
	|						        { $$ = new VariableList(); }
	;
decs : dec decs		{ $2 -> appendElement($1); $$ = $2;  }
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
  tyflds : tyfield COMA tyflds { $3->push_back($1); $$ = $3; }
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
args : exp COMA args		{ $3 -> push_back($1); $$ = $3; }
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
}

void yyerror(const char *s) {
  extern int yylineno;
  extern int line_num;
  //cout << "Ori Pesha, parse error!  Message: " << s << endl;
  //cout << "Line: " << line_num << endl;
  // might as well halt now:
  exit(-1);
}
