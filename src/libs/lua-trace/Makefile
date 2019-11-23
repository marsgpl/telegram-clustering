ifndef TGNEWS
$(error "use libs.sh to build")
endif

NAME = trace

build:
	$(LUAC) -o $(LIBS_DST_DIR)/$(NAME).luac $(NAME).lua

clean:
	rm -f $(LIBS_DST_DIR)/$(NAME).luac
