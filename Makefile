C = gcc
CFLAGS = -Wall -O2

all: fork-given-tree fork-arbitrary-tree pipe-example

fork-given-tree: fork-given-tree.o proc-common.o
	$(CC) -o fork-given-tree fork-given-tree.o proc-common.o

proc-common.o: proc-common.c proc-common.h
	$(CC) $(CFLAGS) -o proc-common.o -c proc-common.c

fork-given-tree.o: fork-given-tree.c proc-common.h
	$(CC) $(CFLAGS) -o fork-given-tree.o -c fork-given-tree.c

fork-arbitrary-tree: fork-arbitrary-tree.o tree.o proc-common.o
	$(CC) -o fork-arbitrary-tree fork-arbitrary-tree.o tree.o proc-common.o

tree.o: tree.c tree.h proc-common.h
	$(CC) $(CFLAGS) -o tree.o -c tree.c 

fork-arbitrary-tree.o: fork-arbitrary-tree.c tree.h proc-common.h
	$(CC) $(CFLAGS) -o fork-arbitrary-tree.o -c fork-arbitrary-tree.c 

pipe-example: pipe-example.o proc-common.o
	$(CC) -o pipe-example pipe-example.o proc-common.o

pipe-example.o: pipe-example.c proc-common.h
	$(CC) $(CFLAGS) -o pipe-example.o -c pipe-example.c

clean:
	rm -f proc-common.o fork-given-tree.o tree.o fork-arbitrary-tree.o
