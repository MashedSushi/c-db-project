CFLAGS ?= -std=c11 -O2
LDLIBS ?= -lm -pthread

cdb/cdb: cdb/main.c cdb/sqlite3.c cdb/sqlite3.h
	$(CC) $(CFLAGS) cdb/main.c cdb/sqlite3.c -o $@ $(LDFLAGS) $(LDLIBS)

run: cdb/cdb
	cd cdb && ./cdb

clean:
	$(RM) cdb/cdb

.PHONY: run clean
