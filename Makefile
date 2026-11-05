src/cdb: $(wildcard src/*.c src/*.h)
	gcc -std=c11 -O2 src/*.c -o $@ -lm -pthread

run: src/cdb
	cd src && ./cdb

clean:
	$(RM) src/cdb

.PHONY: run clean
