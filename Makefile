# # # Makefle # # #
# na consola executar: make

CC = gcc
CFLAGS = -Wall -ansi

all:stereom epipolar meastereo.o

# stereom

stereom:abmatching.o photojpeglib.o stereom.o oex.o matrixlib.o
	$(CC) $(CFLAGS) -ljpeg -lm -lblas -llapack matrixlib.o abmatching.o photojpeglib.o oex.o stereom.o -o stereom

stereom.o:stereom.c photojpeglib.h abmatching.h
	$(CC) $(CFLAGS) -c stereom.c -o stereom.o

photojpeglib.o:photojpeglib.c
	$(CC) $(CFLAGS) -c photojpeglib.c -o photojpeglib.o

abmatching.o:abmatching.c photojpeglib.h
	$(CC) $(CFLAGS) -c abmatching.c -o abmatching.o

# oex
oex.o:oex.c matrixlib.h
	$(CC) $(CFLAGS) -c oex.c -o oex.o

matrixlib.o:matrixlib.c
	$(CC) $(CFLAGS) -c matrixlib.c -o matrixlib.o

# epipolar

epipolar:matrixlib.o epipolar.o
	$(CC) $(CFLAGS) -lm -lblas -llapack matrixlib.o epipolar.o -o epipolar

epipolar.o:epipolar.c matrixlib.h
	$(CC) $(CFLAGS) -c epipolar.c -o epipolar.o

# meastereo

meastereo.o: meastereo.c
	$(CC) $(CFLAGS) -c meastereo.c -o meastereo.o 

# outros

clean:
	rm -rf *.o

mrproper:clean
	rm -rf stereom
	rm -rf oex
	rm -rf epipolar
