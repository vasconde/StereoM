# # # Makefle # # #
# na consola executar: make

CC = gcc
CFLAGS = -Wall -ansi

all:stereom

stereom:imagenslib.o stereom.o
	$(CC) $(CFLAGS) -ljpeg -lm imagenslib.o stereom.o -o stereom

stereom.o:stereom.c imagenslib.h
	$(CC) $(CFLAGS) -c stereom.c -o stereom.o

imagenslib.o:imagenslib.c
	$(CC) $(CFLAGS) -c imagenslib.c -o imagenslib.o	

clean:
	rm -rf *.o

mrproper:clean
	rm -rf stereom
