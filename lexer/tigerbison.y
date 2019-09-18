%code requires{
  #include "AST.h"
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
	;
explist: exp PCOMA explist	{ }
	| exp					{ }
	;
rec_fields : id IGUAL exp COMA rec_fields {  }
	| id IGUAL exp			{  }
	|						{ }
	;
decs : dec decs		{ }
	|						{ }
	;
dec : TYPE id IGUAL ty		{ }
	| vardec				{ }
	| fundec				{ }
	;
ty : id						{ }
	| LI tyflds LD			{  }
	| ARRAY OF id			{  }
	;
id : ID						{ string S($1); $$ = new Symbol(S); }
	;
tyflds : tyfield COMA tyflds { }
	| tyfield				{ }
	|						{ }
	;
vardec : VAR id DOSPIG exp	{ }
	| VAR id DOSP id DOSPIG exp { }
	;
fundec : FUNCTION id PI tyflds PD IGUAL exp { }
	| FUNCTION id PI tyflds PD DOSP id IGUAL exp
							{ }
	;
tyfield : id DOSP id		{
              Symbol *a = $1;
              Symbol *b = $3;
              $$ = new TypeField(*a,*b);}
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
