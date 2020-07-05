allD: parser
	g++ -std=c++17 -c -g tigerbison.tab.c lex.yy.c

allC: parser
	g++ -std=c++17 -c -g tigerbison.tab.c lex.yy.c app.cpp

parser: lexer
	bison -d tigerbison.y

lexer:
	flex --debug tigerflex.l

cleanD:
	rm -f tigerbison.tab.c lex.yy.c tigerbison.tab.h tigerbison.tab.o lex.yy.o

cleanC:
	rm -f tigerbison.tab.c lex.yy.c tigerbison.tab.h tigerbison.tab.o lex.yy.o app.o
