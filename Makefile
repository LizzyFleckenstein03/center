PREFIX=/usr/local
CFLAGS=-Wall -Wextra

center: center.c
	$(CC) center.c -o center $(CFLAGS)

install: center
	mkdir -p $(PREFIX)/bin
	cp center $(PREFIX)/bin
