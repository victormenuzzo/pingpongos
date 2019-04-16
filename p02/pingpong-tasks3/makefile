main:	pingpong-tasks3.o	pingpong.o
	gcc pingpong-tasks3.o pingpong.o -o main

pingpong-tasks3.o:	pingpong-tasks3.c
	gcc -c pingpong-tasks3.c

pingpong.o:	pingpong.c	pingpong.h
	gcc -c pingpong.c

clean:
	rm *.o


