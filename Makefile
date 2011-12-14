# # # Makefle # # #
# na consola executar: make

CC = gcc
CFLAGS = -Wall -ansi

all:main meastereo.o

# stereom

main:abmatching.o photojpeglib.o main.o oex.o epipolar.o matrixlib.o
	$(CC) $(CFLAGS) -ljpeg -lm -lblas -llapack matrixlib.o abmatching.o photojpeglib.o oex.o epipolar.o main.o -o main

main.o:main.c photojpeglib.h abmatching.h epipolar.h
	$(CC) $(CFLAGS) -c main.c -o main.o

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



epipolar.o:epipolar.c matrixlib.h
	$(CC) $(CFLAGS) -c epipolar.c -o epipolar.o

# meastereo

meastereo.o: meastereo.c
	$(CC) $(CFLAGS) -c meastereo.c -o meastereo.o 

# outros

clean:
	rm -rf *.o

mrproper:clean
	rm -rf main
	rm -rf oex
	rm -rf epipolar
