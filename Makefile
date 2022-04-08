C = gcc
CFLAGS = -Wall -O2

all: fork-given-tree tree-example

fork-given-tree: fork-given-tree.o proc-common.o
	$(CC) -o fork-given-tree fork-given-tree.o proc-common.o

proc-common.o: proc-common.c proc-common.h
	$(CC) $(CFLAGS) -o proc-common.o -c proc-common.c

fork-given-tree.o: fork-given-tree.c proc-common.h
	$(CC) $(CFLAGS) -o fork-given-tree.o -c fork-given-tree.c

tree-example: tree-example.o tree.o proc-common.o
	$(CC) -o tree-example tree-example.o tree.o proc-common.o

tree.o: tree.c tree.h proc-common.h
	$(CC) $(CFLAGS) -o tree.o -c tree.c 

tree-example.o: tree-example.c tree.h proc-common.h
	$(CC) $(CFLAGS) -o tree-example.o -c tree-example.c 

clean:
	rm -f proc-common.o fork-given-tree.o tree.o tree-example.o
