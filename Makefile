
SUBDIRS = src

.PHONY: subdirs ${SUBDIRS}

subdirs: ${SUBDIRS}

${SUBDIRS}:
	${MAKE} -C $@


.PHONY: install
install:
	cp src/cmdw /usr/lib/cups/backend
	chmod 0544 /usr/lib/cups/backend/cmdw

.PHONY: uninstall
uninstall:
	rm -f /usr/lib/cups/backend/cmdw
