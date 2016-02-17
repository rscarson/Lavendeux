SRC_DIR = src
LIB_DIR = lib
OBJ_DIR = obj
BIN_DIR = bin
INC_DIR = include
TEST_DIR = tests

LEX_SOURCE = $(SRC_DIR)/generated/lex.c
LEX_HEADER = $(INC_DIR)/generated/lex.h
TAB_SOURCE = $(SRC_DIR)/generated/tab.c
TAB_HEADER = $(INC_DIR)/generated/tab.h

_PARSE_DEPS = parse.o hashing.o builtins.o decorators.o list.o constructs.o language.o values.o
PARSE_DEPS = $(patsubst %,$(OBJ_DIR)/%,$(_PARSE_DEPS))

_TEST_HASHING_DEPS = test.o hashing.o
TEST_HASHING_DEPS = $(patsubst %,$(OBJ_DIR)/%,$(_TEST_HASHING_DEPS))

_TEST_BUILTINS_DEPS = test.o hashing.o constructs.o builtins.o
TEST_BUILTINS_DEPS = $(patsubst %,$(OBJ_DIR)/%,$(_TEST_BUILTINS_DEPS))

_TEST_CONSTRUCTS_DEPS = test.o constructs.o hashing.o
TEST_CONSTRUCTS_DEPS = $(patsubst %,$(OBJ_DIR)/%,$(_TEST_CONSTRUCTS_DEPS))

_TEST_DECORATORS_DEPS = test.o decorators.o constructs.o hashing.o builtins.o values.o
TEST_DECORATORS_DEPS = $(patsubst %,$(OBJ_DIR)/%,$(_TEST_DECORATORS_DEPS))

_TEST_PARSE_DEPS = test.o
TEST_PARSE_DEPS = $(patsubst %,$(OBJ_DIR)/%,$(_TEST_PARSE_DEPS))

CC = gcc
COMPILE_FLAGS = -std=c99 -I./$(INC_DIR) -I./$(INC_DIR)/generated -L./$(LIB_DIR) -lm -Wall -g -Wno-unused
WIN32_FLAGS =  -Wl,-subsystem,windows

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) -c -o $@ $< $(COMPILE_FLAGS)

$(OBJ_DIR)/lavendeux.res: $(SRC_DIR)/lavendeux.rc
	windres $(SRC_DIR)/lavendeux.rc -O coff -o $(OBJ_DIR)/lavendeux.res

$(LIB_DIR)/libinterface.a: $(SRC_DIR)/interface_win32.c
	$(CC) -c $(SRC_DIR)/interface_win32.c -o $(OBJ_DIR)/interface.o  $(COMPILE_FLAGS)
	$(CC) -c $(SRC_DIR)/language.c -o $(OBJ_DIR)/language.o  $(COMPILE_FLAGS)
	ar rcs $(LIB_DIR)/libinterface.a $(OBJ_DIR)/interface.o $(OBJ_DIR)/language.o

$(LIB_DIR)/libparse.a: grammar $(PARSE_DEPS)
	$(CC) -c $(LEX_SOURCE) -o $(OBJ_DIR)/lex.o $(COMPILE_FLAGS)
	$(CC) -c $(TAB_SOURCE) -o $(OBJ_DIR)/tab.o $(COMPILE_FLAGS)
	ar rcs $(LIB_DIR)/libparse.a $(PARSE_DEPS) $(OBJ_DIR)/lex.o $(OBJ_DIR)/tab.o

grammar:
	bison $(SRC_DIR)/grammar.y --output=$(TAB_SOURCE) --defines=$(TAB_HEADER) --report-file=report.txt
	flex --outfile=$(LEX_SOURCE) --header-file=$(LEX_HEADER) -B $(SRC_DIR)/grammar.lex

win32: $(OBJ_DIR)/lavendeux.res grammar $(LIB_DIR)/libinterface.a $(LIB_DIR)/libparse.a
	$(CC) $(OBJ_DIR)/lavendeux.res $(SRC_DIR)/main.c -o $(BIN_DIR)/lavendeux.exe -linterface -lparse $(COMPILE_FLAGS) $(WIN32_FLAGS)

test_hashing: $(TEST_HASHING_DEPS)
	@$(CC) $(TEST_DIR)/hashing.c $(TEST_HASHING_DEPS) -o $(BIN_DIR)/$@ $(COMPILE_FLAGS)
	@bin/$@

test_builtins: $(TEST_BUILTINS_DEPS)
	@$(CC) $(TEST_DIR)/builtins.c $(TEST_BUILTINS_DEPS) -o $(BIN_DIR)/$@ $(COMPILE_FLAGS)
	@bin/$@

test_constructs: $(TEST_CONSTRUCTS_DEPS)
	@$(CC) $(TEST_DIR)/constructs.c $(TEST_CONSTRUCTS_DEPS) -o $(BIN_DIR)/$@ $(COMPILE_FLAGS)
	@bin/$@

test_decorators: $(TEST_DECORATORS_DEPS)
	@$(CC) $(TEST_DIR)/decorators.c $(TEST_DECORATORS_DEPS) -o $(BIN_DIR)/$@ $(COMPILE_FLAGS)
	@bin/$@

test_parse: $(LIB_DIR)/libparse.a grammar
	@$(CC) $(TEST_DIR)/parse.c $(TEST_PARSE_DEPS) -o $(BIN_DIR)/$@ -lparse $(COMPILE_FLAGS)
	@bin/$@

test: test_hashing test_builtins test_constructs test_decorators test_parse

clean:
	rm -f $(LIB_DIR)/*.a
	rm -f $(OBJ_DIR)/*.o
	rm -f $(OBJ_DIR)/*.res
	rm -f $(BIN_DIR)/*.exe