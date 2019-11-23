ifndef TGNEWS
$(error "use libs.sh to build")
endif

NAME = thread

CC = gcc
LD = gcc
CCFLAGS = -c -fPIC -m64 -std=gnu99 -O2 -Wall -Werror
LDFLAGS = -shared
INCS = -I$(LUA_SRC_DIR) -I$(SHARED_DIR) -pthread
LIBS = -L$(LUA_SRC_DIR) -pthread

MAIN_T = $(NAME).so
MAIN_O = lua_$(NAME).o

build: $(MAIN_T)
	cp $(MAIN_T) $(LIBS_DST_DIR)

clean:
	rm -f $(MAIN_T) $(MAIN_O)

$(MAIN_T): $(MAIN_O)
	$(LD) $(LDFLAGS) -o $@ $^ $(LIBS)

.c.o:
	$(CC) $(CCFLAGS) -o $@ $< $(INCS)

$(MAIN_O): lua_$(NAME).h $(SHARED_DIR)/tgnews_lua.h
