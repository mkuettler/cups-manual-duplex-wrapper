
CFLAGS=-Wall -Wextra -Werror
OPT=-O3

CMDWSRC=cmdw.c cmdw-options.c
CMDWHDR=cmdw.h cmdw-options.h
CMDWOBJ=cmdw.o cmdw-options.o


all: cmdw

cmdw: ${CMDWOBJ}
	${CC} ${CFLAGS} ${OPT} $^ -o $@

%.o: %.c ${CMDWHDR}
	${CC} ${CFLAGS} $< -c -o $@

debug: ${CMDWSRC} ${CMDWHDR}
	${CC} ${CFLAGS} -DDEBUG -g cmdw.c cmdw-options.c -o cmdw

test:
	lp -o sides=two-sided-long-edge -d Duplex/ML-1640 ~/studium/diplom/arbeit.pdf

gdb: debug
	sudo gdb ./cmdw

install:
	cp cmdw /usr/lib/cups/backend
	chmod 0544 /usr/lib/cups/backend/cmdw

uninstall:
	rm -f /usr/lib/cups/backend/cmdw
#lpadmin -p Duplex -P manual-duplex-wrapper.ppd -E -v cmdw:/ -L Virtual Printer
#lpoptions -d Duplex/ML-1640 -o cmdw-target-printer=ML-1640-Series
