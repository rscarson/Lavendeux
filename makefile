SRC_DIR = src
LIB_DIR = lib
OBJ_DIR = obj
BIN_DIR = bin
INC_DIR = include

LEX_SOURCE = $(SRC_DIR)/lex.c
LEX_HEADER = $(INC_DIR)/lex.h
TAB_SOURCE = $(SRC_DIR)/tab.c
TAB_HEADER = $(INC_DIR)/tab.h

CC = gcc
COMPILE_FLAGS = -I./$(INC_DIR) -L./lib -Wall -g
WIN32_FLAGS =  -Wl,-subsystem,windows

$(OBJ_DIR)/ub3rparse.res:
	windres $(SRC_DIR)/ub3rparse.rc -O coff -o $(OBJ_DIR)/ub3rparse.res

lib/libhashing.a:
	gcc -c $(SRC_DIR)/hashing.c -o $(OBJ_DIR)/hashing.o
	ar rcs lib/libhashing.a $(OBJ_DIR)/hashing.o

lib/libinterface.a:
	gcc -c $(SRC_DIR)/interface_win32.c -o $(OBJ_DIR)/interface.o
	ar rcs lib/libinterface.a $(OBJ_DIR)/interface.o

lib/libparse.a: grammar
	gcc -c $(SRC_DIR)/parse.c -o $(OBJ_DIR)/parse.o $(COMPILE_FLAGS)
	gcc -c $(LEX_SOURCE) -o $(OBJ_DIR)/lex.o $(COMPILE_FLAGS)
	gcc -c $(TAB_SOURCE) -o $(OBJ_DIR)/tab.o $(COMPILE_FLAGS)
	ar rcs lib/libparse.a $(OBJ_DIR)/parse.o $(OBJ_DIR)/lex.o $(OBJ_DIR)/tab.o

grammar:
	bison $(SRC_DIR)\grammar.y --output=$(TAB_SOURCE) --defines=$(TAB_HEADER)
	flex --outfile=$(LEX_SOURCE) --header-file=$(LEX_HEADER) -B $(SRC_DIR)\grammar.lex

win32: $(OBJ_DIR)/ub3rparse.res grammar
	$(CC) $(OBJ_DIR)/ub3rparse.res $(SRC_DIR)/main.c -o $(BIN_DIR)/ub3rparse.exe -linterface $(COMPILE_FLAGS) $(WIN32_FLAGS)