PREFIX=/usr/local

center: center.c
	$(CC) center.c -o center -Wall -Wextra

install: center
	mkdir -p $(PREFIX)/bin
	cp center $(PREFIX)/bin
