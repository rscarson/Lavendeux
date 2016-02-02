bin/ub3rparse.res:
	windres src/ub3rparse.rc -O coff -o bin/ub3rparse.res

all: bin/ub3rparse.res
	gcc bin/ub3rparse.res src/main.c src/menu.c -o bin/ub3rparse.exe -Wall -g

clean:
	rm bin/*