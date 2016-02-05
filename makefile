obj/ub3rparse.res:
	windres src/ub3rparse.rc -O coff -o obj/ub3rparse.res

lib/libhashing.a:
	gcc -c src/hashing.c -o obj/hashing.o
	ar rcs lib/libhashing.a obj/hashing.o

lib/libinterface.a:
	gcc -c src/interface_win32.c -o obj/interface.o
	ar rcs lib/libinterface.a obj/interface.o

lib/libparse.a:
	gcc -c src/parse.c -o obj/parse.o
	ar rcs lib/libparse.a obj/parse.o

obj/ub3rparse.tab.c:
	bison --defines=obj/y.tab.h -o obj/ub3rparse.tab.c src/ub3rparse.y

obj/lex.yy.c:
	flex -o obj/lex.yy.c src/ub3rparse.lex

win32: obj/ub3rparse.res lib/libinterface.a
	gcc obj/ub3rparse.res src/main.c -o bin/ub3rparse.exe -L./lib -linterface -Wall -g -Wl,-subsystem,windows

clean:
	rm bin/* -f
	rm lib/* -f 
	rm obj/* -f 
	touch bin/.gitkeep
	touch lib/.gitkeep
	touch obj/.gitkeep