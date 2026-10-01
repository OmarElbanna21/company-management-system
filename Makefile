CC ?= gcc
CFLAGS ?= -std=gnu11 -Wall -Wextra -O2

company: main.c Functions.c Functions.h
	$(CC) $(CFLAGS) -o company main.c Functions.c

clean:
	rm -f company

.PHONY: clean
