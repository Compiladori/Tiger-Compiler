%{
#include "tigerbison.tab.h"
int num_comments = 0;
int line_num = 0;
using namespace std;
%}
%option noyywrap
%option yylineno
%x COMMENT
%%
type		{ return TYPE; }
array		{ return ARRAY; }
of			{ return OF; }
var			{ return VAR; }
function	{ return FUNCTION; }
let			{ return LET; }
in			{ return IN; }
end			{ return END; }
if			{ return IF; }
then		{ return THEN; }
else		{ return ELSE; }
while		{ return WHILE; }
do			{ return DO; }
for			{ return FOR; }
to			{ return TO; }
break		{ return BREAK; }
nil			{ return NIL; }

\.  				{ return PTO; }
\:  				{ return DOSP; }
\:=				  { return DOSPIG; }
\,  				{ return COMA; }
\;  				{ return PCOMA; }
\=  				{ return IGUAL; }
\(  				{ return PI; }
\)  				{ return PD; }
\[  				{ return CI; }
\]  				{ return CD; }
\{  				{ return LI; }
\}  				{ return LD; }
\&  				{ return AMPER; }
\|  				{ return PIPE; }
\<  				{ return MENOR; }
\<=				  { return MENIG; }
\>  				{ return MAYOR; }
\>=				  { return MAYIG; }
\<>				  {  return DIST; }
\+  				{ return MAS; }
\-  				{ return MENOS; }
\*  				{ return POR; }
\/  				{ return DIV; }
\n          { line_num++; }
[0-9]+         { yylval.ival = atoi(yytext); return INT; }
\"([^\\\"]|\\.)*\"  { yylval.sval = strdup(yytext); return LITERAL; }
[A-Za-z][A-Za-z0-9_]* { yylval.sval = strdup(yytext); return ID; }
"/*"            {num_comments = 1; BEGIN(COMMENT); }
<COMMENT>"*/" { num_comments--;
                if(!num_comments)
                  BEGIN(INITIAL); }
<COMMENT>"/*" { num_comments++; }
<COMMENT>.    { }
<COMMENT>\n    { line_num++; }
[` ``\t``\r``\^L`]      {}
.              { cout << "Lexer syntax error in line " << yylineno<<endl; exit(-1);}
<<EOF>>                         { yyterminate(); return 0; }
%%
