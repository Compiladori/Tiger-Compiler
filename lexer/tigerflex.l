%{
#include "tigerbison.tab.h"
%}
%option noyywrap
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
\:=				{ return DOSPIG; }
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
\<=				{ return MENIG; }
\>  				{ return MAYOR; }
\>=				{ return MAYIG; }
\<>				{ return DIST; }
\+  				{ return MAS; }
\-  				{ return MENOS; }
\*  				{ return POR; }
\/  				{ return DIV; }
[0-9]+         { yylval.ival = atoi(yytext); return INT; }
[a-zA-Z0-9]+   {
                yylval.sval = strdup(yytext);
                return LITERAL;
              }
%%
