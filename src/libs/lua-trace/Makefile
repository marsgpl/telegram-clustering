NAME = trace
INSTALL_DIR = ../../../../libs
LUAC = ../../../lua-5.3.5/src/luac

build:
	$(LUAC) -o $(NAME).luac $(NAME).lua

clean:
	rm -f $(NAME).luac

install:
	mkdir -p $(INSTALL_DIR)
	cp $(NAME).luac $(INSTALL_DIR)
