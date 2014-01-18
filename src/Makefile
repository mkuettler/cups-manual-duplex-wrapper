
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

test: debug
	sudo ./cmdw 42 martin Test 1 "noCollate finishings=3 fit-to-page media=A4 number-up=1 outputorder=normal page-bottom=10 page-left=10 page-right=10 page-top=10 portrait sides=one-sided job-uuid=urn:uuid:e9194df3-dfcf-3928-5504-36b0b68037a5 job-originating-host-name=localhost time-at-creation=1390068972 time-at-processing=1390068972 PageSize=A4" cmdw-test

gdb: debug
	sudo gdb ./cmdw

install:
	cp cmdw /usr/lib/cups/backend
	chmod 0544 /usr/lib/cups/backend/cmdw

uninstall:
	rm -f /usr/lib/cups/backend/cmdw
#lpadmin -p ManualDuplexWrapper -P manual-duplex-wrapper.ppd -E -v cmdw:/ -L Virtual Printer
