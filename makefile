SRC_DIR = src
LIB_DIR = lib
OBJ_DIR = obj
BIN_DIR = bin
INC_DIR = include

LEX_SOURCE = $(SRC_DIR)/generated/lex.c
LEX_HEADER = $(INC_DIR)/generated/lex.h
TAB_SOURCE = $(SRC_DIR)/generated/tab.c
TAB_HEADER = $(INC_DIR)/generated/tab.h

_PARSE_DEPS = parse.o hashing.o builtins.o decorators.o list.o constructs.o language.o
PARSE_DEPS = $(patsubst %,$(OBJ_DIR)/%,$(_PARSE_DEPS))

CC = gcc
COMPILE_FLAGS = -std=gnu99 -I./$(INC_DIR) -I./$(INC_DIR)/generated -L./$(LIB_DIR) -lm -Wall -g -Wno-unused
WIN32_FLAGS =  -Wl,-subsystem,windows

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) -c -o $@ $< $(COMPILE_FLAGS)

$(OBJ_DIR)/lavendeux.res: $(SRC_DIR)/lavendeux.rc
	windres $(SRC_DIR)/lavendeux.rc -O coff -o $(OBJ_DIR)/lavendeux.res

$(LIB_DIR)/libinterface.a: $(SRC_DIR)/interface_win32.c
	gcc -c $(SRC_DIR)/interface_win32.c -o $(OBJ_DIR)/interface.o  $(COMPILE_FLAGS)
	gcc -c $(SRC_DIR)/language.c -o $(OBJ_DIR)/language.o  $(COMPILE_FLAGS)
	ar rcs $(LIB_DIR)/libinterface.a $(OBJ_DIR)/interface.o $(OBJ_DIR)/language.o

$(LIB_DIR)/libparse.a: grammar $(PARSE_DEPS)
	gcc -c $(LEX_SOURCE) -o $(OBJ_DIR)/lex.o $(COMPILE_FLAGS)
	gcc -c $(TAB_SOURCE) -o $(OBJ_DIR)/tab.o $(COMPILE_FLAGS)
	ar rcs $(LIB_DIR)/libparse.a $(PARSE_DEPS) $(OBJ_DIR)/lex.o $(OBJ_DIR)/tab.o

grammar: $(SRC_DIR)\grammar.y $(SRC_DIR)\grammar.lex
	bison $(SRC_DIR)\grammar.y --output=$(TAB_SOURCE) --defines=$(TAB_HEADER) --report-file=report.txt
	flex --outfile=$(LEX_SOURCE) --header-file=$(LEX_HEADER) -B $(SRC_DIR)\grammar.lex

win32: $(OBJ_DIR)/lavendeux.res grammar $(LIB_DIR)/libinterface.a $(LIB_DIR)/libparse.a $(SRC_DIR)/main.c
	$(CC) $(OBJ_DIR)/lavendeux.res $(SRC_DIR)/main.c -o $(BIN_DIR)/lavendeux.exe -linterface -lparse $(COMPILE_FLAGS) $(WIN32_FLAGS)

all: win32

clean:
	rm -f $(LIB_DIR)/*.a
	rm -f $(OBJ_DIR)/*.o
	rm -f $(OBJ_DIR)/*.res
	rm -f $(BIN_DIR)/*.exe