ifndef TGNEWS
$(error "use libs.sh to build")
endif

NAME = class

build:
	$(LUAC) -o $(LIBS_DST_DIR)/$(NAME).luac $(NAME).lua

clean:
	rm -f $(LIBS_DST_DIR)/$(NAME).luac
