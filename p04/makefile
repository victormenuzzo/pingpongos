main:	pingpong-scheduler.o	pingpong.o	queue.o
	gcc pingpong-scheduler.o pingpong.o queue.o -o main

pingpong-scheduler.o:	pingpong-scheduler.c
	gcc -c pingpong-scheduler.c

pingpong.o:	pingpong.c	pingpong.h
	gcc -c pingpong.c

queue.o:	queue.c	queue.h
	gcc -c queue.c

clean:
	rm *.o


