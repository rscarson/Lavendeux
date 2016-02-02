bin/ub3rparse.res:
	windres src/ub3rparse.rc -O coff -o bin/ub3rparse.res

libmenu.a:
	gcc -c src/menu.c -o bin/menu.o
	ar rcs libmenu.a bin/menu.o

win32: bin/ub3rparse.res libmenu.a
	gcc bin/ub3rparse.res src/main.c -o bin/ub3rparse3.exe -L. -lmenu -Wall -g

clean:
	rm bin/*
	rm libmenu.a