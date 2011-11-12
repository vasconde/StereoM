# # # Makefle # # #
# na consola executar: make

CC = gcc
CFLAGS = -Wall -ansi

all:stereom

stereom:photojpeglib.o stereom.o
	$(CC) $(CFLAGS) -ljpeg -lm photojpeglib.o stereom.o -o stereom

stereom.o:stereom.c photojpeglib.h
	$(CC) $(CFLAGS) -c stereom.c -o stereom.o

photojpeglib.o:photojpeglib.c
	$(CC) $(CFLAGS) -c photojpeglib.c -o photojpeglib.o	

clean:
	rm -rf *.o

mrproper:clean
	rm -rf stereom
