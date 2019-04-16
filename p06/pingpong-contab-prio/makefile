main:	pingpong-contab-prio.o	pingpong.o	queue.o
	gcc pingpong-contab-prio.o pingpong.o queue.o -o main

pingpong-contab-prio.o:	pingpong-contab-prio.c
	gcc -c pingpong-contab-prio.c

pingpong.o:	pingpong.c	pingpong.h
	gcc -c pingpong.c

queue.o:	queue.c	queue.h
	gcc -c queue.c

clean:
	rm *.o


