CC= gcc
CFLAGS= -Wall -Wextra -ansi -g

BINARIES= client.out server.out
SOURCES= client.c server.c utils.c

OBJS=$(patsubst %.c, %.o, $(SOURCES))

all: $(BINARIES)

client.out: client.o
	$(CC) $(CFLAGS) $^ -o $@

server.out: server.o utils.o
	$(CC) $(CFLAGS) $^ -o $@

%.o: %.c 
	$(CC) $(CFLAGS) -c $^

utils.c: utils.h

clean:
	rm -f *.o

distclean: clean
	rm -f $(BINARIES)

