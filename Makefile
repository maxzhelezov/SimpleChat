CC= gcc
CFLAGS= -Wall -Wextra -ansi -g

BINARIES= client.out server.out
SOURCES= client.c server.c

OBJS=$(patsubst %.c, %.o, $(SOURCES))

all: $(BINARIES)

client.out: client.o
	$(CC) $(CFLAGS) $^ -o $@

server.out: server.o
	$(CC) $(CFLAGS) $^ -o $@

%.o: %.c 
	$(CC) $(CFLAGS) -c $^

server: server.out
	./server.out

client: client.out
	./server.out

leak: all
	valgrind --leak-check=full --show-leak-kinds=all ./$(BINARIES)

clean:
	rm -f *.o

distclean: clean
	rm -f $(BINARIES)

