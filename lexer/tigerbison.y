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

  void yyerror(const char *s);
%}

%union {
  int ival;
  char *sval;
  Expression *Exp;
  Symbol *symbol;
  TypeField *typefield;
  TypeFieldList *typefieldlist;
  Type *typ;
  FunDec *fundec;
  VarDec *vardec;
  Declaration *dec;
  RecordExp *recordexp;
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
%type <Exp> exp
%type <symbol> id
%type <typefield> tyfield
%type <typefieldlist> tyflds
%type <typ> ty
%type <fundec> fundec
%type <vardec> vardec
%type <dec> dec
%type <recordexp> rec_fields

%%
prog : exp END_OF_FILE

exp : INT					{ }
	| PI PD					{  }
	| NIL					{ }
  | LITERAL				{ }
	| BREAK					{ }
	| l_value				{ }
	| l_value DOSPIG exp	{ }
	| PI exp PCOMA explist {  }
	| exp PIPE exp			{  }
	| exp AMPER exp			{  }
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
	| l_value CI exp CD OF exp {}
  | id LI rec_fields LD	{  }
	;
explist: exp PCOMA explist	{ }
	| exp					{ }
	;
rec_fields : id IGUAL exp COMA rec_fields { $5->push_back(new RecordExp($3, $1, yylineno)); $$ = $5; }
	| id IGUAL exp			{  }
	|						{ }
	;
decs : dec decs		{ /*lista*/ }
	|						{ }
	;
dec : TYPE id IGUAL ty		{ $$ = new TypeDec($2, $4); }
	| vardec				{ $$ = $1; }
	| fundec				{ $$ = $1; }
	;
ty : id						{ $$ = new NameType($1); }
	| LI tyflds LD			{ $$ = new RecordType($2); }
	| ARRAY OF id			{ $$ = new ArrayType($3); }
	;
id : ID						{ $$ = new Symbol(string($1)); }
	;
  tyflds : tyfield COMA tyflds { $3->push_back($1); $$ = $3; }
	| tyfield				{ $$ = new TypeFieldList(1, $1); }
	|						{ $$ = new TypeFieldList(); }
	;
vardec : VAR id DOSPIG exp	{ $$ = new VarDec($2, $4); }
	| VAR id DOSP id DOSPIG exp { $$ = new VarDec($2, $4, $6); }
	;
fundec : FUNCTION id PI tyflds PD IGUAL exp { $$ = new FunDec($2, $4, $7); }
	| FUNCTION id PI tyflds PD DOSP id IGUAL exp
							{ $$ = new FunDec($2, $4, $7, $9); }
	;
tyfield : id DOSP id		{ $$ = new TypeField($1, $3); }
	;
args : exp COMA args		{ }
	| exp					{ }
	|						{  }
	;
l_value : id				{ }
	| l_value PTO id		{  }
	| l_value CI exp CD		{ }
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
