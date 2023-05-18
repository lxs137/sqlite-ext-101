CC = gcc
VERSION=$(shell cat VERSION)
LOADABLE_EXTENSION=so
SQLITE_BIN=sqlite3

CMACROS= -DSQLITE_EXT_VERSION="\"$(VERSION)\""
CFLAGS= -fPIC -shared -Ilibs -O3 $(CMACROS)

DIST=out
$(DIST):
	mkdir -p $(DIST)

TARGET_LOADABLE_TICK=$(DIST)/tick.so
TARGET_LOADABLE_VECTOR=$(DIST)/vector.so

TARGET_LIB_CJSON=libs/cJSON/libcjson.so
TARGET_SRC_CJSON=libs/cJSON/cJSON.c

libs: $(TARGET_LIB_CJSON)

all: $(TARGET_LOADABLE_TICK) $(TARGET_LOADABLE_VECTOR) libs

$(TARGET_LOADABLE_TICK): tick.c $(DIST)
	$(CC) $(CFLAGS) \
	tick.c -o $@

$(TARGET_LOADABLE_VECTOR): vector.c $(TARGET_SRC_CJSON) $(DIST)
	$(CC) $(CFLAGS) \
	vector.c $(TARGET_SRC_CJSON) -o $@

$(TARGET_LIB_CJSON): $(TARGET_SRC_CJSON)
	make -C $(TARGET_SRC_CJSON) all

noop:
	@mkdir -p $(DIST)/cJSON
	cmake -S $(TARGET_LIB_CJSON_SRC) -B $(DIST)/cJSON -DENABLE_CJSON_UTILS=Off -DENABLE_CJSON_TEST=Off

clean:
	rm -rf $(DIST)

test:
	$(SQLITE_BIN) :memory: '.read tests/tick_test.sql'
	$(SQLITE_BIN) :memory: '.read tests/vector_test.sql'

.PHONY: all clean test all
