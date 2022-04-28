C = gcc
CFLAGS = -Wall -O2

all: fork-given-tree fork-arbitrary-tree signals pipes

fork-given-tree: fork-given-tree.o proc-common.o
	$(CC) -o fork-given-tree fork-given-tree.o proc-common.o

proc-common.o: proc-common.c proc-common.h
	$(CC) $(CFLAGS) -o proc-common.o -c proc-common.c

fork-given-tree.o: fork-given-tree.c proc-common.h
	$(CC) $(CFLAGS) -o fork-given-tree.o -c fork-given-tree.c

fork-arbitrary-tree: fork-arbitrary-tree.o tree.o
	$(CC) -o fork-arbitrary-tree fork-arbitrary-tree.o tree.o proc-common.o

tree.o: tree.c tree.h
	$(CC) $(CFLAGS) -o tree.o -c tree.c

fork-arbitrary-tree.o: fork-arbitrary-tree.c tree.h
	$(CC) $(CFLAGS) -o fork-arbitrary-tree.o -c fork-arbitrary-tree.c

signals: signals.o tree.o proc-common.o
	$(CC) -o signals signals.o tree.o proc-common.o

signals.o: signals.c  tree.h proc-common.h
	$(CC) $(CFLAGS) -o signals.o -c signals.c

pipes: pipes.o tree.o proc-common.o
	$(CC) -o pipes pipes.o tree.o proc-common.o

pipes.o: pipes.c tree.h 
	$(CC) $(CFLAGS) -o pipes.o -c pipes.c

clean:
	rm -f proc-common.o fork-given-tree.o  fork-arbitrary-tree.o tree.o signals.o pipes.o