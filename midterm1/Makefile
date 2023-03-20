.PHONY: build test

CC ?= gcc
CFLAGS = $(shell echo $$(cat compile_flags.txt))
CFILES = $(shell find . -name "*.c" -and -not -name "*test.c")
CTESTS = $(shell find . -name "*.c" -and -not -name "main.c")
LOCALES = locales/vi/LC_MESSAGES/messages.mo

build: main.out

test: test.out
	./test.out

main.out: $(CFILES) $(LOCALES)
	$(CC) $(CFLAGS) -o main.out $(CFILES)

test.out: $(CTESTS)
	$(CC) $(CFLAGS) -o test.out $(CTESTS)

locales/messages.po: $(CFILES)
	mkdir -p $(dir $@)
	xgettext -o locales/messages.po --no-location --omit-header $(CFILES)

$(LOCALES): locales/%/LC_MESSAGES/messages.mo: locales/%/messages.po

locales/%/messages.po: locales/messages.po
	mkdir -p $(dir $@)
	if [ ! -f $@ ]; then \
		msginit -o $@ -i $< -l $* --no-translator; \
	else \
		msgmerge -o $@ $@ $<; \
	fi

locales/%/LC_MESSAGES/messages.mo: locales/%/messages.po
	mkdir -p $(dir $@)
	msgfmt -o $@ $<

#
# locales/en/LC_MESSAGES/messages.po: locales
# 	msgfmt --input=
# 	msgfmt -o locales/en/LC_MESSAGES/messages.mo locales/en/LC_MESSAGES/messages.po
#
# locales/en/LC_MESSAGES/messages.mo: locales/en/LC_MESSAGES/messages.po
# 	msgfmt -o locales/en/LC_MESSAGES/messages.mo locales/en/LC_MESSAGES/messages.po
