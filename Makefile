PREFIX?=/usr/X11R6
CFLAGS?=-Os -pedantic -Wall

all:
	$(CC) $(CFLAGS) -Wall -Wextra -std=c99 -g -O0 -I$(PREFIX)/include main.c -L$(PREFIX)/lib -lX11 -o spwm
clean:
	rm -f spwm
install:
	mv -i spwm $(HOME)/bin
