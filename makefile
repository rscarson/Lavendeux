ub3rparse.res:
	windres ub3rparse.rc -O coff -o ub3rparse.res

menu.o:
	gcc menu.c -o menu.o

all: ub3rparse.res menu.o
	ln ub3rparse.res menu.o