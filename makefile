# make file for dining philosophers algorithm

OBJS	= main.o
CC  	= gcc -pthread

all: dining

main.o: main.c
	$(CC) -c main.c

dining: main.o
	$(CC) -o dining main.o

clean:
	rm  	$(OBJS)
