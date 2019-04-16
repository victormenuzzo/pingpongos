#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "pingpong.h"

int buffer[5], bufferTam = 0, bufferTam2 = 0;
task_t p1, p2, p3, c1, c2;
semaphore_t s_buffer, s_item, s_vaga;

void produtorBody(void* arg){
	int item;
	while(1){
		task_sleep(1);
		item = rand()%100;

		sem_down(&s_vaga);
		
		sem_down(&s_buffer);
		buffer[bufferTam] = item;
		bufferTam = (bufferTam + 1)%5;
		sem_up(&s_buffer);
		
		sem_up(&s_item);
		
		printf("%s produziu %d\n", (char*)arg, item);
	}
	task_exit(0);
}

void consumidorBody(void* arg){
    int item;
    while(1){
        sem_down(&s_item);
        
        sem_down(&s_buffer);
        item = buffer[bufferTam2];
        bufferTam2 = (bufferTam2 + 1)%5;;
        sem_up(&s_buffer);
        
        sem_up(&s_vaga);
        
        printf("                    %s consumiu %d\n", (char*)arg, item);
        task_sleep(1);
    }    
    task_exit(0);
}



int main(){
    srand(time(NULL));
    pingpong_init();
    sem_create(&s_buffer, 1);
    sem_create(&s_item, 0);
    sem_create(&s_vaga, 5);
    
    task_create(&p1, produtorBody, "p1");
    task_create(&p2, produtorBody, "p2");
    task_create(&p3, produtorBody, "p3");
    

    task_create(&c1, consumidorBody, "c1");
    task_create(&c2, consumidorBody, "c2");

    task_exit(0);
}


