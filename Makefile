C = gcc
CFLAGS = -Wall -O2

all: fork-given-tree

fork-given-tree: fork-given-tree.o proc-common.o
        $(CC) -o fork-given-tree fork-given-tree.o proc-common.o

proc-common.o: proc-common.c proc-common.h
        $(CC) $(CFLAGS) -o proc-common.o -c proc-common.c

fork-given-tree.o: fork-given-tree.c proc-common.h
        $(CC) $(CFLAGS) -o fork-given-tree.o -c fork-given-tree.c

clean:
        rm -f proc-common.o fork-given-tree.o