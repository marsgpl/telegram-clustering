ifndef TGNEWS
$(error "use libs.sh to build")
endif

NAME = text

CC = gcc
LD = gcc
CCFLAGS = -c -fPIC -m64 -std=gnu99 -O2 -Wall -Werror
LDFLAGS = -shared
INCS = -I$(LUA_SRC_DIR) -I$(SHARED_DIR)
LIBS = -L$(LUA_SRC_DIR) -lrt -Lutf8proc/build -lutf8proc

MAIN_T = $(NAME).so
MAIN_O = lua_$(NAME).o

build: utf8proc $(MAIN_T)
	cp $(MAIN_T) $(LIBS_DST_DIR)

clean: clean-utf8proc
	rm -f $(MAIN_T) $(MAIN_O)

utf8proc:
	cd utf8proc && mkdir -p build && cd build && cmake .. && make

clean-utf8proc:
	cd utf8proc && rm -rf build && make clean

$(MAIN_T): $(MAIN_O)
	$(LD) $(LDFLAGS) -o $@ $^ $(LIBS)

.c.o:
	$(CC) $(CCFLAGS) -o $@ $< $(INCS)

$(MAIN_O): lua_$(NAME).h $(SHARED_DIR)/tgnews_lua.h utf8proc/utf8proc.h

.PHONY: utf8proc
