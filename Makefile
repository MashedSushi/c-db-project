src/cdb: src/main.c src/sqlite3.c src/sqlite3.h
	gcc -std=c11 -O2 src/main.c src/sqlite3.c -o $@ -lm -pthread

run: src/cdb
	cd src && ./cdb

clean:
	$(RM) src/cdb

.PHONY: run clean
