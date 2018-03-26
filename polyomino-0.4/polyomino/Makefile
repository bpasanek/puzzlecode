BINARIES=pentomino one_sided_pentomino one_sided_hexomino pent_hexomino tetr_pentomino heptomino rook cross log-tee parallelogram punctured_rectangle identical_hexomino identical_heptomino
INCLUDES=polyomino.h poly-list.h
CC=cc -Wall -O4 -funroll-loops -lm

all:	${BINARIES}

pentomino:	pentomino.c ${INCLUDES}
	${CC} pentomino.c -o pentomino
	strip pentomino

one_sided_pentomino:	one_sided_pentomino.c ${INCLUDES}
	${CC} one_sided_pentomino.c -o one_sided_pentomino
	strip one_sided_pentomino

one_sided_hexomino:	one_sided_hexomino.c ${INCLUDES}
	${CC} one_sided_hexomino.c -o one_sided_hexomino
	strip one_sided_hexomino

pent_hexomino:	pent_hexomino.c ${INCLUDES}
	${CC} pent_hexomino.c -o pent_hexomino
	strip pent_hexomino

tetr_pentomino:	tetr_pentomino.c ${INCLUDES}
	${CC} tetr_pentomino.c -o tetr_pentomino
	strip tetr_pentomino

heptomino:	heptomino.c ${INCLUDES}
	${CC} heptomino.c -o heptomino
	strip heptomino

rook:	rook.c ${INCLUDES}
	${CC} rook.c -o rook
	strip rook

cross:	cross.c ${INCLUDES}
	${CC} cross.c -o cross
	strip cross

parallelogram:	parallelogram.c ${INCLUDES}
	${CC} parallelogram.c -o parallelogram
	strip parallelogram

punctured_rectangle:	punctured_rectangle.c ${INCLUDES}
	${CC} punctured_rectangle.c -o punctured_rectangle
	strip punctured_rectangle

problem65:	problem65.c ${INCLUDES}
	${CC} problem65.c -o problem65
	strip problem65

identical_hexomino:	identical_hexomino.c ${INCLUDES}
	${CC} identical_hexomino.c -o identical_hexomino
	strip identical_hexomino

identical_heptomino:	identical_heptomino.c ${INCLUDES}
	${CC} identical_heptomino.c -o identical_heptomino
	strip identical_heptomino

log-tee:	log-tee.c
	${CC} log-tee.c -o log-tee
	strip log-tee

clean:
	rm -f ${BINARIES} set*.ps *.core

