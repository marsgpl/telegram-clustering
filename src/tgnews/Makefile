NAME = tgnews

ROOT_DIR = ../..
LUA_ROOT_DIR = ../lua-5.3.5/src

CC = gcc
LD = gcc
CCFLAGS = -c -m64 -std=gnu99 -O2 -Wall -Werror
LDFLAGS = -s
INCS = -I$(LUA_ROOT_DIR)
LIBS = -L$(LUA_ROOT_DIR) -llua -lm -Wl,-E -ldl

MAIN_T = $(NAME)
MAIN_O = $(NAME).o

build: $(MAIN_T)

logic:
	sh logic.sh

libs:
	sh libs.sh

clean:
	rm -f $(MAIN_T) $(MAIN_O)
	rm -rf $(ROOT_DIR)/libs
	rm -rf $(ROOT_DIR)/logic

install:
	cp $(MAIN_T) $(ROOT_DIR)

$(MAIN_T): $(MAIN_O)
	$(LD) $(LDFLAGS) -o $@ $^ $(LIBS)

.c.o:
	$(CC) $(CCFLAGS) -o $@ $< $(INCS)

$(MAIN_O): $(NAME).h

.PHONY: logic libs
