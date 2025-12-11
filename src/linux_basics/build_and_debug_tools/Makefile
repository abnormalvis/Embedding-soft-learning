CC=gcc
CFLAGS=-Wall -Wextra -g

all: demo

demo: main.o
	$(CC) $(CFLAGS) $^ -o $@

clean:
	rm -f demo main.o

.PHONY: all clean
