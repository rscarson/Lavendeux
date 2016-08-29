# Options
PYTHON_DIR = C:\\Python27
EXTENSIONS = 1
DEBUG = 1

# Directories
SRC_DIR = src
LIB_DIR = lib
OBJ_DIR = obj
BIN_DIR = bin
INC_DIR = include
ICON_DIR = icon
TEST_DIR = tests

# Base options
CC = gcc
BUILD_ARGS = $(SRC_DIR)/main.c -o $(BIN_DIR)/lavendeux -linterface -lparse
_COMPILE_FLAGS = -std=gnu99 -I./$(INC_DIR) -I./$(INC_DIR)/generated -L./$(LIB_DIR) -lm -Wall -Wno-unused
EXTENSIONS_FLAGS = -DEXTENSIONS_INCLUDED

# Dependencies for final build
BUILD_DEPS = grammar $(LIB_DIR)/libinterface.a $(LIB_DIR)/libparse.a

# Lexer / Parser output locations
LEX_SOURCE = $(SRC_DIR)/generated/lex.c
LEX_HEADER = $(INC_DIR)/generated/lex.h
TAB_SOURCE = $(SRC_DIR)/generated/tab.c
TAB_HEADER = $(INC_DIR)/generated/tab.h

# Versions
MAJOR_VERSION = 0
MINOR_VERSION = 7
RELEASE_NUMBER = 1

# Platform specific flags
ifeq ($(shell echo $$OSTYPE),cygwin)
	BUILD_DEPS += $(OBJ_DIR)/lavendeux.res
	BUILD_ARGS += $(OBJ_DIR)/lavendeux.res
	PLATFORM_FLAGS = $(OBJ_DIR)/lavendeux.res -Wl,-subsystem,windows
	EXTENSIONS_FLAGS += -I/usr/include/python2.7 -L/usr/lib/python2.7 -lpython2.7.dll
	_SETUP = winstall
	_SETUP_NOEXT = winstall_noext
	UNINSTALL = win32_uninstall
else ifeq ($(OS),Windows_NT)
	BUILD_DEPS += $(OBJ_DIR)/lavendeux.res
	BUILD_ARGS += $(OBJ_DIR)/lavendeux.res
	PLATFORM_FLAGS = -Wl,-subsystem,windows
	EXTENSIONS_FLAGS += -I$(PYTHON_DIR)\\include -L$(PYTHON_DIR)\\lib -lpython27
	_SETUP = winstall
	_SETUP_NOEXT = winstall_noext
	UNINSTALL = win32_uninstall
else ifeq ($(shell uname -s),Linux)
	PLATFORM_FLAGS = `pkg-config --cflags gtk+-3.0` `pkg-config --libs gtk+-3.0` `pkg-config --libs keybinder-3.0` `pkg-config appindicator3-0.1 --cflags` `pkg-config appindicator3-0.1 --libs` -lX11
	EXTENSIONS_FLAGS += `pkg-config --cflags python2` `pkg-config --libs python2`
	_SETUP = linstall
	_SETUP_NOEXT = linstall_noext
	UNINSTALL = linux_uninstall
endif

# Debug setup
ifeq ($(DEBUG), 1)
	_COMPILE_FLAGS += -g
endif

# Setup make extensions support
_COMPILE_FLAGS += $(PLATFORM_FLAGS)
COMPILE_FLAGS = $(_COMPILE_FLAGS)
SETUP = $(_SETUP_NOEXT)
SUFFIX = -no-extensions
ifeq ($(EXTENSIONS),1)
	OBJ_FLAGS = -static -static-libgcc
	COMPILE_FLAGS += $(EXTENSIONS_FLAGS)
	SETUP = $(_SETUP)
	SUFFIX = 
endif

# And filename
FILENAME = $(MAJOR_VERSION).$(MINOR_VERSION).$(RELEASE_NUMBER)$(SUFFIX)

###############
# Build Stuff #
###############

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) -c -o $@ $< $(COMPILE_FLAGS) $(OBJ_FLAGS)

# Parser library
_PARSE_DEPS = parse.o hashing.o builtins.o decorators.o list.o constructs.o language.o values.o settings.o extensions.o
PARSE_DEPS = $(patsubst %,$(OBJ_DIR)/%,$(_PARSE_DEPS))
$(LIB_DIR)/libparse.a: grammar $(PARSE_DEPS)
	$(CC) -c $(LEX_SOURCE) -o $(OBJ_DIR)/lex.o $(COMPILE_FLAGS)
	$(CC) -c $(TAB_SOURCE) -o $(OBJ_DIR)/tab.o $(COMPILE_FLAGS)
	ar rcs $(LIB_DIR)/libparse.a $(PARSE_DEPS) $(OBJ_DIR)/lex.o $(OBJ_DIR)/tab.o

# Interface library
_INTERFACE_DEPS = language.o cmdflags.o settings.o interface_win32.o interface_linux.o
INTERFACE_DEPS = $(patsubst %,$(OBJ_DIR)/%,$(_INTERFACE_DEPS))
$(LIB_DIR)/libinterface.a: $(INTERFACE_DEPS)
	ar rcs $(LIB_DIR)/libinterface.a $(INTERFACE_DEPS)

# Grammar build
grammar:
	bison $(SRC_DIR)/grammar.y --output=$(TAB_SOURCE) --defines=$(TAB_HEADER)
	flex --outfile=$(LEX_SOURCE) --header-file=$(LEX_HEADER) -B $(SRC_DIR)/grammar.lex

# Version build
version:
	echo '#ifndef VERSION_H' > include/generated/version.h
	echo '	#define VERSION_H' >> include/generated/version.h
	echo '	#define MAJOR_VERSION "$(MAJOR_VERSION)"' >> include/generated/version.h
	echo '	#define MINOR_VERSION "$(MINOR_VERSION)"' >> include/generated/version.h
	echo '	#define RELEASE_NUMBER "$(RELEASE_NUMBER)"' >> include/generated/version.h
	echo '#endif' >> include/generated/version.h

# Cleanup
clean:
	rm -f $(LIB_DIR)/*.a
	rm -f $(OBJ_DIR)/*.o
	rm -f $(OBJ_DIR)/*.res
	rm -f $(BIN_DIR)/lavendeux*

# Binary build
all: version $(BUILD_DEPS)
	$(CC) $(BUILD_ARGS) $(COMPILE_FLAGS) $(PLATFORM_FLAGS)

# Setup stuff
install: all $(SETUP)
uninstall: $(UNINSTALL)

##################
# Linux Specific #
##################

tar: clean
	tar --exclude='./.git' -zcvf ../lavendeux-$(FILENAME).tar.gz *
	mv ../lavendeux-$(FILENAME).tar.gz $(BIN_DIR)/lavendeux-$(FILENAME).tar.gz

linstall: linstall_noext
	cp -r $(BIN_DIR)/extensions/ /etc/lavendeux/
	cp $(BIN_DIR)/python27.zip /etc/lavendeux/

linstall_noext:
	cp $(BIN_DIR)/lavendeux /usr/bin/
	cp $(SRC_DIR)/lavendeux.desktop /usr/share/applications/
	chmod +x /usr/share/applications/lavendeux.desktop
	cp $(ICON_DIR)/lavendeux-128.png /usr/share/pixmaps/
	cp $(ICON_DIR)/lavendeux.png /usr/share/pixmaps/
	chmod 777 /usr/share/pixmaps/lavendeux.png
	mkdir -p /etc/lavendeux/
	cp $(BIN_DIR)/.lavendeuxsettings /etc/lavendeux/
	cp CHANGELOG /etc/lavendeux/
	cp LICENSE /etc/lavendeux/
	cp readme.md /etc/lavendeux/

linux_uninstall:
	rm /usr/bin/lavendeux
	rm /usr/share/applications/lavendeux.desktop
	rm /usr/share/pixmaps/lavendeux.png
	rm -r /etc/lavendeux/

####################
# Windows Specific #
####################

$(OBJ_DIR)/lavendeux.res: $(SRC_DIR)/lavendeux.rc
	windres $(SRC_DIR)/lavendeux.rc -O coff -o $(OBJ_DIR)/lavendeux.res

winstall:
	makensis $(SRC_DIR)/setup.nsi
	mv $(BIN_DIR)/lavendeux-setup.exe $(BIN_DIR)/lavendeux-setup-$(FILENAME).exe
	zip bin/lavendeux-$(FILENAME).zip $(BIN_DIR)/python27.dll $(BIN_DIR)/python27.zip CHANGELOG LICENSE readme.md $(BIN_DIR)/lavendeux.exe $(BIN_DIR)/.lavendeuxsettings -j
	cd  $(BIN_DIR); zip -r lavendeux-$(FILENAME).zip extensions
	cd  $(BIN_DIR); zip -r lavendeux-$(FILENAME).zip lib

winstall_noext:
	makensis $(SRC_DIR)/setup-noextensions.nsi
	mv $(BIN_DIR)/lavendeux-setup.exe $(BIN_DIR)/lavendeux-setup-$(FILENAME).exe
	zip bin/lavendeux-$(FILENAME).zip CHANGELOG LICENSE readme.md $(BIN_DIR)/lavendeux.exe $(BIN_DIR)/.lavendeuxsettings -j

win32_uninstall:
	@echo "Please run the uninstaller in the installation directory"

#########
# Tests #
#########

test : COMPILE_FLAGS = $(_COMPILE_FLAGS)

# Test hash tables
_TEST_HASHING_DEPS = test.o hashing.o
TEST_HASHING_DEPS = $(patsubst %,$(OBJ_DIR)/%,$(_TEST_HASHING_DEPS))
test_hashing: $(TEST_HASHING_DEPS)
	@$(CC) $(TEST_DIR)/hashing.c $(TEST_HASHING_DEPS) -o $(BIN_DIR)/$@ $(COMPILE_FLAGS)
	@bin/$@

# Test the builtin functions
_TEST_BUILTINS_DEPS = test.o hashing.o builtins.o
TEST_BUILTINS_DEPS = $(patsubst %,$(OBJ_DIR)/%,$(_TEST_BUILTINS_DEPS))
test_builtins: $(TEST_BUILTINS_DEPS)
	@$(CC) $(TEST_DIR)/builtins.c $(TEST_BUILTINS_DEPS) -o $(BIN_DIR)/$@ $(COMPILE_FLAGS)
	@bin/$@

# Test variable & function constructs
_TEST_CONSTRUCTS_DEPS = test.o constructs.o hashing.o
TEST_CONSTRUCTS_DEPS = $(patsubst %,$(OBJ_DIR)/%,$(_TEST_CONSTRUCTS_DEPS))
test_constructs: $(TEST_CONSTRUCTS_DEPS)
	@$(CC) $(TEST_DIR)/constructs.c $(TEST_CONSTRUCTS_DEPS) -o $(BIN_DIR)/$@ $(COMPILE_FLAGS)
	@bin/$@

# Test decorators
_TEST_DECORATORS_DEPS = test.o decorators.o hashing.o
TEST_DECORATORS_DEPS = $(patsubst %,$(OBJ_DIR)/%,$(_TEST_DECORATORS_DEPS))
test_decorators: $(TEST_DECORATORS_DEPS)
	@$(CC) $(TEST_DIR)/decorators.c $(TEST_DECORATORS_DEPS) -o $(BIN_DIR)/$@ $(COMPILE_FLAGS)
	@bin/$@

# Test the grammar
_TEST_PARSE_DEPS = test.o
TEST_PARSE_DEPS = $(patsubst %,$(OBJ_DIR)/%,$(_TEST_PARSE_DEPS))
test_parse: $(LIB_DIR)/libparse.a grammar
	@$(CC) $(TEST_DIR)/parse.c $(TEST_PARSE_DEPS) -o $(BIN_DIR)/$@ -lparse $(COMPILE_FLAGS) $(PYTHON_FLAGS)
	@bin/$@

test: test_hashing test_builtins test_constructs test_decorators test_parse