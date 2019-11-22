NAME = thread
INSTALL_DIR = ../../../../libs
LUA_ROOT_DIR = ../../../lua-5.3.5
CC = gcc
LD = gcc
CCFLAGS = -c -fPIC -m64 -std=gnu99 -O2 -Wall -Werror
LDFLAGS = -shared
INCS = -I$(LUA_ROOT_DIR)/src -pthread
LIBS = -pthread

MAIN_T = $(NAME).so
MAIN_O = lua_$(NAME).o

build: $(MAIN_T)

clean:
	rm -f $(MAIN_T) $(MAIN_O)

install:
	mkdir -p $(INSTALL_DIR)
	cp $(MAIN_T) $(INSTALL_DIR)

$(MAIN_T): $(MAIN_O)
	$(LD) $(LDFLAGS) -o $@ $^ $(LIBS)

.c.o:
	$(CC) $(CCFLAGS) -o $@ $< $(INCS)

$(MAIN_O): lua_$(NAME).h
