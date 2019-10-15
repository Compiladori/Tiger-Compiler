tiger.exe: lexer parser
	g++  --std=c++17 tigerbison.tab.c lex.yy.c ../AST/AST.cpp ../Escapes/escapes.cpp ../Translation/translation.cpp -lfl -o tiger.exe

lexer:
	flex --debug tigerflex.l

parser:
	bison -d tigerbison.y

clean:
	rm -f tigerbison.tab.c lex.yy.c tiger.exe tigerbison.tab.h ../AST/AST ../Translation/translation ../Escapes/escapes
