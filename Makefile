# # # Makefle # # #
# na consola executar: make

CC = gcc
CFLAGS = -Wall -ansi

all:stereom oex epipolar

# stereom

stereom:photojpeglib.o stereom.o
	$(CC) $(CFLAGS) -ljpeg -lm photojpeglib.o stereom.o -o stereom

stereom.o:stereom.c photojpeglib.h
	$(CC) $(CFLAGS) -c stereom.c -o stereom.o

photojpeglib.o:photojpeglib.c
	$(CC) $(CFLAGS) -c photojpeglib.c -o photojpeglib.o

# oex

oex:matrixlib.o oex.o
	$(CC) $(CFLAGS) -lm -lblas -llapack matrixlib.o oex.o -o oex

oex.o:oex.c matrixlib.h
	$(CC) $(CFLAGS) -c oex.c -o oex.o

matrixlib.o:matrixlib.c
	$(CC) $(CFLAGS) -c matrixlib.c -o matrixlib.o

# epipolar

epipolar:matrixlib.o epipolar.o
	$(CC) $(CFLAGS) -lm -lblas -llapack matrixlib.o epipolar.o -o epipolar

epipolar.o:epipolar.c matrixlib.h
	$(CC) $(CFLAGS) -c epipolar.c -o epipolar.o

# outros

clean:
	rm -rf *.o

mrproper:clean
	rm -rf stereom
	rm -rf oex
	rm -rf epipolar
