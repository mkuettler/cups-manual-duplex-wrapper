
ifeq (,$(DEFAULT_PRINTER))
  CFLAGS =
else
  CFLAGS = -DDEFAULT_PRINTER=\"$(DEFAULT_PRINTER)\"
endif

CFLAGS += -Wall -Wextra -Werror

CMDWSRC=cmdw.c cmdw-options.c
CMDWHDR=cmdw.h cmdw-options.h
CMDWOBJ=cmdw.o cmdw-options.o

.PHONY: all
all: CFLAGS += -O3
all: cmdw

cmdw: ${CMDWOBJ}
	${CC} ${CFLAGS} $^ -o $@

%.o: %.c ${CMDWHDR}
	${CC} ${CFLAGS} $< -c -o $@

.PHONY: debug
debug: CFLAGS += -DDEBUG -g
debug: all
