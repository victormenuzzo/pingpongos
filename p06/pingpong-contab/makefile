main:	pingpong-contab.o	pingpong.o	queue.o
	gcc pingpong-contab.o pingpong.o queue.o -o main

pingpong-contab.o:	pingpong-contab.c
	gcc -c pingpong-contab.c

pingpong.o:	pingpong.c	pingpong.h
	gcc -c pingpong.c

queue.o:	queue.c	queue.h
	gcc -c queue.c

clean:
	rm *.o


