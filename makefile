bin/ub3rparse.res:
	windres src/ub3rparse.rc -O coff -o bin/ub3rparse.res

bin/libinterface.a:
	gcc -c src/interface_win32.c -o bin/interface.o
	ar rcs lib/libinterface.a bin/interface.o

win32: bin/ub3rparse.res bin/libinterface.a
	gcc bin/ub3rparse.res src/main.c -o bin/ub3rparse3.exe -L./lib -linterface -Wall -g

clean:
	rm bin/*
	rm libmenu.a