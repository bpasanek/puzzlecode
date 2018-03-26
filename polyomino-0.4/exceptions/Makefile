BINARIES=problem65 polyomino-find hexomino-new
CC=cc -Wall -O4 -funroll-loops

all:	${BINARIES}

hexomino-new:	hexomino-new.c
	${CC} hexomino-new.c -o hexomino-new
	strip hexomino-new

problem65:	problem65.c
	${CC} problem65.c -o problem65 -march=pentiumpro
	strip problem65

polyomino-find:	polyomino-find.c
	${CC} polyomino-find.c -o polyomino-find
	strip polyomino-find

clean:
	rm -f ${BINARIES} set*.ps *.core

