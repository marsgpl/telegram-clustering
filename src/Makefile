NAME = tgnews

ROOT_DIR = /tgnews
LUA_ROOT_DIR = $(ROOT_DIR)/src/lua-5.3.5
LUA_SRC_DIR = $(LUA_ROOT_DIR)/src
LOGIC_DST_DIR = $(ROOT_DIR)/logic
LIBS_DST_DIR = $(ROOT_DIR)/libs

CC = gcc
LD = gcc
CCFLAGS = -c -m64 -std=gnu99 -O2 -Wall -Werror
LDFLAGS =
INCS = -I$(LUA_SRC_DIR)
LIBS = -L$(LUA_SRC_DIR) -llua -lm -Wl,-E -ldl

MAIN_T = $(NAME)
MAIN_O = $(NAME).o

all: lua main libs logic

clean: lua- main- libs- logic-

lua:
	cd $(LUA_ROOT_DIR) && make linux

lua-:
	cd $(LUA_ROOT_DIR) && make clean

main: $(MAIN_T)
	cp $(MAIN_T) $(ROOT_DIR)

main-:
	rm -f *.o $(MAIN_T)
	rm -f $(ROOT_DIR)/$(MAIN_T)

libs:
	bash libs.sh

libs-:
	bash libs.sh clean
	rm -rf $(LIBS_DST_DIR)

logic:
	bash logic.sh

logic-:
	rm -rf $(LOGIC_DST_DIR)

$(MAIN_T): $(MAIN_O)
	$(LD) $(LDFLAGS) -o $@ $^ $(LIBS)

.c.o:
	$(CC) $(CCFLAGS) -o $@ $< $(INCS)

$(MAIN_O): $(NAME).h shared/tgnews_lua.h

.PHONY: logic libs
