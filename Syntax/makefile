tiger.exe: lexer parser
	g++  --std=c++14 tigerbison.tab.c lex.yy.c -lfl -o tiger.exe

lexer:
	flex --debug tigerflex.l

parser:
	bison -d tigerbison.y

clean:
	rm -f tigerbison.tab.c lex.yy.c tiger.exe tigerbison.tab.h
