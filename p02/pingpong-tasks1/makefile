main:	pingpong-tasks1.o	pingpong.o
	gcc pingpong-tasks1.o pingpong.o -o main

pingpong-tasks1.o:	pingpong-tasks1.c
	gcc -c pingpong-tasks1.c

pingpong.o:	pingpong.c	pingpong.h
	gcc -c pingpong.c

clean:
	rm *.o


