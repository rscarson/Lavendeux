# Configuration options
NO_EXTENSIONS=1 will disable extensions
PYTHON_INCLUDE_DIR = C:\\Python27\\include
PYTHON_LIB_DIR = C:\\Python27\\libs

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

_TEST_HASHING_DEPS = test.o hashing.o
_TEST_BUILTINS_DEPS = test.o hashing.o builtins.o
_TEST_CONSTRUCTS_DEPS = test.o constructs.o hashing.o
_TEST_DECORATORS_DEPS = test.o decorators.o hashing.o
_TEST_PARSE_DEPS = test.o

CC = gcc
COMPILE_FLAGS = -std=gnu99 -I./$(INC_DIR) -I./$(INC_DIR)/generated -L./$(LIB_DIR) -lm -Wall -g -Wno-unused
WIN32_FLAGS = -Wl,-subsystem,windows
LINUX_FLAGS = `pkg-config --cflags gtk+-3.0` `pkg-config --libs gtk+-3.0`
PYTHON_FLAGS = 

ifndef NO_EXTENSIONS
	PYTHON_FLAGS = -lpython27 -static -static-libgcc
	COMPILE_FLAGS += -DEXTENSIONS_INCLUDED -I$(PYTHON_INCLUDE_DIR) -L$(PYTHON_LIB_DIR) 
	_PARSE_DEPS += extensions.o
endif

PARSE_DEPS = $(patsubst %,$(OBJ_DIR)/%,$(_PARSE_DEPS))

TEST_HASHING_DEPS = $(patsubst %,$(OBJ_DIR)/%,$(_TEST_HASHING_DEPS))
TEST_BUILTINS_DEPS = $(patsubst %,$(OBJ_DIR)/%,$(_TEST_BUILTINS_DEPS))
TEST_CONSTRUCTS_DEPS = $(patsubst %,$(OBJ_DIR)/%,$(_TEST_CONSTRUCTS_DEPS))
TEST_DECORATORS_DEPS = $(patsubst %,$(OBJ_DIR)/%,$(_TEST_DECORATORS_DEPS))
TEST_PARSE_DEPS = $(patsubst %,$(OBJ_DIR)/%,$(_TEST_PARSE_DEPS))

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) -c -o $@ $< $(COMPILE_FLAGS) $(PYTHON_FLAGS)

$(LIB_DIR)/libinterface.a: $(SRC_DIR)/interface_win32.c
	$(CC) -c $(SRC_DIR)/interface_win32.c -o $(OBJ_DIR)/interface.o  $(COMPILE_FLAGS)
	$(CC) -c $(SRC_DIR)/language.c -o $(OBJ_DIR)/language.o  $(COMPILE_FLAGS)
	$(CC) -c $(SRC_DIR)/cmdflags.c -o $(OBJ_DIR)/cmdflags.o  $(COMPILE_FLAGS)
	ar rcs $(LIB_DIR)/libinterface.a $(OBJ_DIR)/interface.o $(OBJ_DIR)/language.o $(OBJ_DIR)/cmdflags.o

$(LIB_DIR)/libparse.a: grammar $(PARSE_DEPS)
	$(CC) -c $(LEX_SOURCE) -o $(OBJ_DIR)/lex.o $(COMPILE_FLAGS)
	$(CC) -c $(TAB_SOURCE) -o $(OBJ_DIR)/tab.o $(COMPILE_FLAGS)
	ar rcs $(LIB_DIR)/libparse.a $(PARSE_DEPS) $(OBJ_DIR)/lex.o $(OBJ_DIR)/tab.o

grammar:
	bison $(SRC_DIR)/grammar.y --output=$(TAB_SOURCE) --defines=$(TAB_HEADER) --report-file=report.txt
	flex --outfile=$(LEX_SOURCE) --header-file=$(LEX_HEADER) -B $(SRC_DIR)/grammar.lex

clean:
	rm -f $(LIB_DIR)/*.a
	rm -f $(OBJ_DIR)/*.o
	rm -f $(OBJ_DIR)/*.res
	rm -f $(BIN_DIR)/*.exe

##################
# Linux Platform #
##################

linux: grammar $(LIB_DIR)/libinterface.a $(LIB_DIR)/libparse.a
	$(CC) $(SRC_DIR)/main.c -o $(BIN_DIR)/lavendeux.exe -linterface -lparse $(COMPILE_FLAGS) $(LINUX_FLAGS)

####################
# Windows Platform #
####################

$(OBJ_DIR)/lavendeux.res: $(SRC_DIR)/lavendeux.rc
	windres $(SRC_DIR)/lavendeux.rc -O coff -o $(OBJ_DIR)/lavendeux.res

win32: $(OBJ_DIR)/lavendeux.res grammar $(LIB_DIR)/libinterface.a $(LIB_DIR)/libparse.a
	$(CC) $(OBJ_DIR)/lavendeux.res $(SRC_DIR)/main.c -o $(BIN_DIR)/lavendeux.exe -linterface -lparse $(COMPILE_FLAGS) $(WIN32_FLAGS) $(PYTHON_FLAGS)

windows_binaries: win32
	zip bin/lavendeux.zip python27.dll python27.zip CHANGELOG LICENSE README $(BIN_DIR)/lavendeux.exe .lavendeuxsettings -j
	zip -r bin/lavendeux.zip extensions
	makensis src/setup.nsi

###############
# Tests Begin #
###############

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
	@$(CC) $(TEST_DIR)/parse.c $(TEST_PARSE_DEPS) -o $(BIN_DIR)/$@ -lparse $(COMPILE_FLAGS) $(PYTHON_FLAGS)
	@bin/$@

test: test_hashing test_builtins test_constructs test_decorators test_parse