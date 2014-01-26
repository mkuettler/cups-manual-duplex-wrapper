
ifeq ($(wildcard Makefile.conf),)
$(warning Makefile.conf not found.)
$(warning Create Makefile.conf from Makefile.conf.template to override the defaults.)
endif

-include Makefile.conf

DEFAULT_PRINTER ?=
WRAPPER_NAME ?= DuplexWrapper

SUBDIRS = src

.PHONY: subdirs ${SUBDIRS}

subdirs: ${SUBDIRS}

${SUBDIRS}:
	${MAKE} -C $@ DEFAULT_PRINTER=${DEFAULT_PRINTER}


.PHONY: clean
clean:
	rm -f src/*.o
	rm -f src/cmdw

.PHONY: install
install:
	cp src/cmdw /usr/lib/cups/backend
	chmod 0544 /usr/lib/cups/backend/cmdw
	lpadmin -p $(WRAPPER_NAME) -P extra/manual-duplex-wrapper.ppd -E -v cmdw:/ -L "Virtual Printer"

.PHONY: uninstall
uninstall:
	rm -f /usr/lib/cups/backend/cmdw
