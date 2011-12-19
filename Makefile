# # # Makefle # # #
# na consola executar: make

CC = clang
CSTD = -std=c89
CFLAGS = -Wall 

all:main

# stereom

main:abmatching.o photojpeglib.o main.o oex.o epipolar.o matrixlib.o
	$(CC) $(CFLAGS) -ljpeg -lm -lblas -llapack matrixlib.o abmatching.o photojpeglib.o oex.o epipolar.o main.o -o main

main.o:main.c photojpeglib.h abmatching.h epipolar.h
	$(CC) $(CFLAGS) $(CSTD) -c main.c -o main.o

photojpeglib.o:photojpeglib.c
	$(CC) $(CFLAGS) $(CSTD) -c photojpeglib.c -o photojpeglib.o

abmatching.o:abmatching.c photojpeglib.h
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
	rm -rf main
