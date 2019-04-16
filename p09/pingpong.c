#include "queue.h"
#include "pingpong.h"
#include <stdlib.h>
#include <stdio.h>
#include <ucontext.h>

#include <signal.h>
#include <sys/time.h>


#define STACKSIZE 32768	


//#define DEBUG true
//#define DEBUG_JOIN true
//#define DEBUG_QUANTUM


int id = 0;
task_t taskMain;	
task_t *taskAtual; 
task_t dispatcher;  
task_t *taskProntas = NULL; 
int taskUser = 0;  //quantas tarefas tem na fila
int quantum = 0;   
unsigned int tempo = 0;
struct sigaction action;
struct itimerval timer;
task_t *scheduler();
void dispatcher_body(void *arg);
void tratador(int signum);
task_t *taskDormindo = NULL;



void pingpong_init ()
{
    /* desativa o buffer da saida padrao (stdout), usado pela função printf */
    setvbuf (stdout, 0, _IONBF, 0);



    taskMain.tid = id++;	//comeca com id 0
    task_setprio(&taskMain, 0);

    taskMain.tempoComeco = 0;
    taskMain.tempoProc = 0;
    taskMain.tempoEx = 0;

    taskMain.dependente = NULL;

    taskMain.status = EXEC;

    taskMain.bloqueio = 0;
    taskUser++;
    taskMain.at = 1;
    taskAtual = &taskMain;

    #ifdef DEBUG
    printf("Criou Task Main %d\n", taskMain.tid);
    #endif

    task_create(&dispatcher, dispatcher_body, NULL);

    // registra a ação para o sinal de timer SIGALRM
    action.sa_handler = tratador;
    sigemptyset (&action.sa_mask);
    action.sa_flags = 0;
    if (sigaction (SIGALRM, &action, 0) < 0) {
        perror ("Erro em sigaction: ");
        exit (1) ;
    }
    // ajusta valores do temporizador
    timer.it_value.tv_usec = 1;      // primeiro disparo, em micro-segundos
    timer.it_value.tv_sec  = 0 ;      // primeiro disparo, em segundos
    timer.it_interval.tv_usec = 1000;   // disparos subsequentes, em micro-segundos
    timer.it_interval.tv_sec  = 0 ;   // disparos subsequentes, em segundos

    // arma o temporizador ITIMER_REAL (vide man setitimer)
    if (setitimer (ITIMER_REAL, &timer, 0) < 0) {
        perror ("Erro em setitimer: ");
        exit (1);
    }
    //-------- ------------------------ --------------------------------------

}

int task_create (task_t *task, void (*start_routine)(void *),  void *arg)
{
    if(!task){ // ve a task que foi passada existe
         return -1;
    }

    char *pilha; //declaramos a pilha de caracteres
    pilha = malloc(STACKSIZE); //alocamos o espaço necessário para nossa pilha de caracteres

    getcontext(&(task->taskContext));//pegamos o contexto atual da tarefa que nos foi fornecida

   if (pilha) //utilizamos isso para não haver compartilhamento com outro espaço de memória que esteja sendo utilizado
   {
      task->taskContext.uc_stack.ss_sp = pilha ;
      task->taskContext.uc_stack.ss_size = STACKSIZE;
      task->taskContext.uc_stack.ss_flags = 0;
      task->taskContext.uc_link = 0;
   }
   else
   {
      perror ("Erro na criação da pilha: ");
      exit (1);
   }

    makecontext(&(task->taskContext), (void*)(*start_routine), 1, arg); //atribui a nossa taskContext a função passada por parametro e o argumento também passado

    int proximoID = id++;
    task->tid = proximoID;
    task->next = NULL;
    task->prev = NULL;
    task->tempoEx = 0;
    task->tempoComeco = systime();
    task->tempoProc = 0;
    task->at = 0;
    task->status = READY;
    task->dependente = NULL;
    task->bloqueio = 0;

    if (&dispatcher != task){ //temos que ver se a tarefa que está sendo criada não é o dispatcher
      queue_append((queue_t **) &taskProntas,(queue_t*) task);//se não for inserimos ela na fila de tarefas
      task->fila = (queue_t **) &taskProntas;
      taskUser++;
      task->taskPrioEst = 0;
      task->taskPrioDin = 0;

    }


    #ifdef DEBUG
    printf("Tarefa %d criada\n", task->tid);
      #endif

   return task->tid;
}

int task_switch (task_t *task){

    if(task==NULL){ 
        return -1;
    }

    task_t *taskAux; 

    taskAux = taskAtual;

    #ifdef DEBUG
    printf("Trocou de %d para %d\n", taskAtual->tid, task->tid);
    #endif
    taskAtual = task; 
    quantum = 20;
    taskAtual->at++;

    swapcontext(&(taskAux->taskContext), &(task->taskContext)); 
    return 0;
}


void task_exit (int exit_code){

    taskAtual->bloqueio = 1;

    unsigned int tempoEx = systime() - taskAtual->tempoComeco;
    taskAtual->tempoEx = tempoEx;

    #ifdef DEBUG
    printf("Tarefa %d terminada\n", task_id());
    #endif
    taskAtual->tempoEx = systime() - taskAtual->tempoComeco;
    taskAtual->exitCode = exit_code;
    taskAtual->status = ENDED;

    printf("Task %d exit: execution time %dms, processor time %dms, %d activations\n", task_id(), taskAtual->tempoEx, taskAtual->tempoProc, taskAtual->at);

    while (taskAtual->dependente) {
        taskAtual->dependente->status = READY;
        taskAtual->dependente->fila =NULL;
        queue_append ((queue_t **) &taskProntas, queue_remove((queue_t **)&(taskAtual->dependente), (queue_t *)taskAtual->dependente));
    }

    taskAtual->bloqueio = 0;

    if(taskAtual != &dispatcher){
    	taskUser--;   
        task_switch(&dispatcher);
    }
}

int task_id (){

    #ifdef DEBUG
    printf("Id da Tarefa %d", taskAtual->tid);
    #endif

    return taskAtual->tid;
}

void task_yield ()
{
    
    if(task_id() != 1) {
        queue_append((queue_t **) &taskProntas,(queue_t*) taskAtual);
        taskAtual->fila = (queue_t **) taskProntas;  
        taskAtual->status = READY;
        task_switch(&dispatcher);
    }

    task_switch(&dispatcher);   
}

void dispatcher_body(void *arg)
{
    while ( taskUser > 0 ) {
        if(queue_size((queue_t *)taskDormindo) > 0) {
            task_t *prox = taskDormindo;
            task_t *aux;
            int i;
            for (i = 0; i < queue_size((queue_t*) taskDormindo); i++) {
                aux = prox;
                prox = prox->next; 
                if(systime() >= aux->tempoAcordar){
                	aux->status = READY;
                	//printf("tempoAtual: %d \nid: %d \n tempoAcordar: %d\n", systime(), aux->tid, aux->tempoAcordar); -> para verificar se estao acordando na hora certa
                	aux->tempoAcordar = 0; 
                    queue_remove((queue_t **)&taskDormindo, (queue_t *)aux);
                    queue_append((queue_t **) &taskProntas, (queue_t *) aux);
                    aux->fila = (queue_t **) &taskProntas;
                    
                 }
             }
        }
        task_t *next = scheduler() ; 
        if (next) {
            task_switch (next) ; 
            // ações após retornar da tarefa "next", se houverem
        }
    }
    task_exit(0) ; 
}


//com envelhecimento
task_t *scheduler()
{   
    if(taskProntas == NULL){
        return NULL;
    }
    
    int i;   
    task_t * next;
    task_t *aux = taskProntas;
    int menPrio = 21;
    //acha a tarefa prioritaria
    for (i = 0; i < queue_size((queue_t *)taskProntas); i++) {
        if (aux->taskPrioDin < menPrio) {
            next = aux;
            menPrio = aux->taskPrioDin;
        }
        aux = aux->next; 
    }
 
    queue_remove((queue_t **)&taskProntas, (queue_t *)next);
    

    //envelhece outras
    aux = taskProntas;
    for (i = 0; i < queue_size((queue_t *)taskProntas); i++) {
        aux->taskPrioDin = aux->taskPrioDin - 1;
        aux = aux->next; 
    }
    
    //reseta a prioridade dinamica da tarefa retirada
    next->taskPrioDin = next->taskPrioEst;
    next->fila = NULL;
    next->status = EXEC;

    return next;
    
}

void task_setprio (task_t *task, int prio)
{
    if(prio >-20 && prio< 20){
        if(task){
            task->taskPrioEst = prio;
            task->taskPrioDin = prio;
        }
        else{
            taskAtual->taskPrioEst = prio;
            taskAtual->taskPrioDin = prio;
        }
    }
}

int task_getprio (task_t *task)
{
    if(task) {
        return task->taskPrioEst;
    }
    return taskAtual->taskPrioEst;
}

void tratador (int signum)
{
    tempo = systime()+1;
    taskAtual->tempoProc++;

    if (task_id() != 1 && taskAtual->bloqueio == 0) {
        quantum--;
        if (quantum <= 0) {
            queue_append((queue_t **) &taskProntas, (queue_t *) taskAtual);
            taskAtual->fila = (queue_t **) &taskProntas;
            taskAtual->status = READY;
            task_switch(&dispatcher);   
        }
    }
}

unsigned int systime ()
{
    return tempo;
}


/*-----------------Inicio do p08-----------------------------*/
int task_join (task_t *task){
    if(task){
        if(task->status == ENDED) {
            return task->exitCode;
        }
        taskAtual->bloqueio = 1;

        taskAtual->status = SUSP;
        queue_append((queue_t **) &(task->dependente),(queue_t*) taskAtual);
        taskAtual->fila =  (queue_t**)task->dependente;
        taskAtual->bloqueio = 0;
        task_switch(&dispatcher); 


        return task->exitCode;
    }
    else{
        return -1;
    }
}
/*-----------------Fim do p08-----------------------------*/

/*-----------------Inicio do p09-----------------------------*/

void task_sleep (int t) {
	taskAtual->bloqueio = 1;

	taskAtual->fila = (queue_t**) taskDormindo;
	queue_append((queue_t**)&taskDormindo, (queue_t*) taskAtual );
	taskAtual->status = SLEEP;
	taskAtual->tempoAcordar = systime() + t * 1000 ;

	taskAtual->bloqueio = 0;
	task_switch(&dispatcher);


}
/*-----------------Fim do p09-----------------------------*/
