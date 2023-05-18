CC = gcc
VERSION=$(shell cat VERSION)
LOADABLE_EXTENSION=so
SQLITE_BIN=sqlite3

CMACROS= -DSQLITE_EXT_TICK_VERSION="\"$(VERSION)\""
CFLAGS= -fPIC -shared -Ilibs -O3 $(CMACROS)

DIST=out
$(DIST):
	mkdir -p $(DIST)

TARGET_LOADABLE_TICK=$(DIST)/tick.$(LOADABLE_EXTENSION)
TARGET_LOADABLE=$(TARGET_LOADABLE_TICK)

all: $(TARGET_LOADABLE)

$(TARGET_LOADABLE_TICK): tick.c $(DIST)
	$(CC) $(CFLAGS) \
	$< -o $@

clean:
	rm -rf $(DIST)

test:
	$(SQLITE_BIN) :memory: '.read tick_test.sql'

.PHONY: all clean test all
