all: parser
	g++ -std=c++17 -c -g tigerbison.tab.c lex.yy.c

parser: lexer
	bison -d tigerbison.y

lexer:
	flex --debug tigerflex.l

clean:
	rm -f tigerbison.tab.c lex.yy.c tigerbison.tab.h tigerbison.tab.o lex.yy.o 
