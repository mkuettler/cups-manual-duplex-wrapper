
CFLAGS=-Wall -Wextra -Werror -O3

CMDWSRC=cmdw.c cmdw-options.c
CMDWHDR=cmdw.h cmdw-options.h
CMDWOBJ=cmdw.o cmdw-options.o

all: cmdw

cmdw: ${CMDWOBJ}
	${CC} ${CFLAGS} $^ -o $@

%.o: %.c ${CMDWHDR}
	${CC} ${CFLAGS} $< -c -o $@

install: cmdw
	cp cmdw /usr/lib/cups/backend
	chmod 0544 /usr/lib/cups/backend/cmdw

uninstall:
	rm -f /usr/lib/cups/backend/cmdw
