main:	pingpong-preempcao.o	pingpong.o	queue.o
	gcc pingpong-preempcao.o pingpong.o queue.o -o main

pingpong-preempcao.o:	pingpong-preempcao.c
	gcc -c pingpong-preempcao.c

pingpong.o:	pingpong.c	pingpong.h
	gcc -c pingpong.c

queue.o:	queue.c	queue.h
	gcc -c queue.c

clean:
	rm *.o


