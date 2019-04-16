main:	pingpong-tasks2.o	pingpong.o
	gcc pingpong-tasks2.o pingpong.o -o main

pingpong-tasks2.o:	pingpong-tasks2.c
	gcc -c pingpong-tasks2.c

pingpong.o:	pingpong.c	pingpong.h
	gcc -c pingpong.c

clean:
	rm *.o


