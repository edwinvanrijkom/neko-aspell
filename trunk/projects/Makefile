PROJECT:=aspell

NDLL:=../bin/linux/$(PROJECT).ndll

SRC_PATHS:=../src
SRCS:=$(foreach PATH,$(SRC_PATHS), $(wildcard $(PATH)/*.c))

CC:=gcc -I../include
NEKO_FLAGS:=-fPIC -shared -L/usr/lib -lneko


$(NDLL): $(SRCS)
        @echo srcs: $(SRCS)
        $(CC) $(NEKO_FLAGS) $(GTK_FLAGS) $(GCONF_FLAGS) -o $@ $(SRCS)
