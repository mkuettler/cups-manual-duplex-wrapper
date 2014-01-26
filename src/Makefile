
DEFOPTS=
DEFOTS=-DDEFAULT_PRINTER=CLP-360_Series

CFLAGS=-Wall -Wextra -Werror
OPT=-O3

CMDWSRC=cmdw.c cmdw-options.c
CMDWHDR=cmdw.h cmdw-options.h
CMDWOBJ=cmdw.o cmdw-options.o


.PHONY: all
all: cmdw

cmdw: ${CMDWOBJ}
	${CC} ${CFLAGS} ${OPT} $^ -o $@

%.o: %.c ${CMDWHDR}
	${CC} ${CFLAGS} ${DEFOPTS} $< -c -o $@

.PHONY: debug
debug: ${CMDWSRC} ${CMDWHDR}
	${CC} ${CFLAGS} ${DEFOPTS} -DDEBUG -g cmdw.c cmdw-options.c -o cmdw

.PHONY: test
test:
	lp -o sides=two-sided-long-edge -d Duplex/ML-1640 ~/studium/diplom/arbeit.pdf

.PHONY: gdb
gdb: debug
	sudo gdb ./cmdw

#lpadmin -p Duplex -P manual-duplex-wrapper.ppd -E -v cmdw:/ -L Virtual Printer
#lpoptions -d Duplex/ML-1640 -o cmdw-target-printer=ML-1640-Series
