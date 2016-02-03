SHELL=C:/Windows/System32/cmd.exe

bin/ub3rparse.res:
	windres src/ub3rparse.rc -O coff -o bin/ub3rparse.res

bin/libinterface.a:
	gcc -c src/interface_win32.c -o bin/interface.o
	ar rcs lib/libinterface.a bin/interface.o

src/ub3rparse.tab.c:
	bison --defines=src/y.tab.h -o src/ub3rparse.tab.c src/ub3rparse.y

src/lex.yy.c:
	flex -o src/lex.yy.c src/ub3rparse.lex

win32: bin/ub3rparse.res bin/libinterface.a src/ub3rparse.tab.c src/lex.yy.c
	gcc bin/ub3rparse.res src/main.c src/lex.yy.c src/ub3rparse.tab.c -o bin/ub3rparse3.exe -L./lib -linterface -Wall -g

clean:
	rm bin/* -f
	rm lib/* -f 
	rm src/lex.yy.c -f 
	rm src/y.tab.h -f 
	rm src/ub3rparse.tab.c -f
	touch bin/.gitkeep
	touch lib/.gitkeep