# # # Makefle # # #
# na consola executar: make

CC = gcc
CSTD = -ansi
CFLAGS = -Wall -g 
CLIBS = -lm -ljpeg -lblas -llapack

all:stereom

# stereom

stereom:abmatching.o photojpeglib.o stereom.o oex.o epipolar.o matrixlib.o meastereo.o
	$(CC) $(CFLAGS) matrixlib.o abmatching.o photojpeglib.o oex.o epipolar.o meastereo.o stereom.o -o stereom $(CLIBS)

stereom.o:stereom.c photojpeglib.h abmatching.h epipolar.h meastereo.h
	$(CC) $(CFLAGS) $(CSTD) -c stereom.c -o stereom.o

photojpeglib.o:photojpeglib.c
	$(CC) $(CFLAGS) $(CSTD) -c photojpeglib.c -o photojpeglib.o

abmatching.o:abmatching.c photojpeglib.h epipolar.h matrixlib.h
	$(CC) $(CFLAGS) $(CSTD) -c abmatching.c -o abmatching.o

# oex

oex.o:oex.c matrixlib.h
	$(CC) $(CFLAGS) $(CSTD) -c oex.c -o oex.o

# epipolar

epipolar.o:epipolar.c matrixlib.h
	$(CC) $(CFLAGS) $(CSTD) -c epipolar.c -o epipolar.o

# meastereo

meastereo.o: meastereo.c
	$(CC) $(CFLAGS) $(CSTD) -c meastereo.c -o meastereo.o 

# matrixlib

matrixlib.o:matrixlib.c
	$(CC) $(CFLAGS) $(CSTD) -c matrixlib.c -o matrixlib.o

# outros

clean:
	rm -rf *.o

mrproper:clean
	rm -rf stereom
