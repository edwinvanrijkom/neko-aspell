PROJECT:=aspell

NDLL:=../bin/linux/$(PROJECT).ndll

INSTALL_PREFIX:=/usr/local

SRC_PATHS:=../src
SRCS:=$(foreach PATH,$(SRC_PATHS), $(wildcard $(PATH)/*.c))

CC:=gcc -I../include
NEKO_FLAGS:=-fPIC -shared -L/usr/lib -lneko


$(NDLL): $(SRCS)
	@echo srcs: $(SRCS)
	$(CC) $(NEKO_FLAGS) $(GTK_FLAGS) $(GCONF_FLAGS) -o $@ $(SRCS)

install:
	cp ${NDLL} ${INSTALL_PREFIX}/lib/neko

clean:
	rm ${NDLL}
