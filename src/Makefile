NAME = tgnews
ROOT_DIR = ../..
LUA_ROOT_DIR = ../lua-5.3.5
CC = gcc
LD = gcc
CCFLAGS = -c -m64 -std=gnu99 -O2 -Wall -Werror
LDFLAGS =
INCS = -I$(LUA_ROOT_DIR)/src
LIBS = -L$(LUA_ROOT_DIR)/src -llua -lm -Wl,-E -ldl

MAIN_T = $(NAME)
MAIN_O = $(NAME).o

build: $(MAIN_T)

clean:
	rm -f $(MAIN_T) $(MAIN_O)

build_lua:
	cd $(LUA_ROOT_DIR) && make linux

clean_lua:
	cd $(LUA_ROOT_DIR) && make clean

build_libs:
	sh libs.sh

clean_libs:
	sh libs.sh clean

build_logic:
	sh logic.sh

clean_logic:
	sh logic.sh clean

install:
	cp $(MAIN_T) $(ROOT_DIR)

uninstall:
	rm $(ROOT_DIR)/$(MAIN_T)
	rm -rf $(ROOT_DIR)/libs
	rm -rf $(ROOT_DIR)/logic

$(MAIN_T): $(MAIN_O)
	$(LD) $(LDFLAGS) -o $@ $^ $(LIBS)

.c.o:
	$(CC) $(CCFLAGS) -o $@ $< $(INCS)

$(MAIN_O): $(NAME).h

.PHONY: logic libs
