#include "pingpong.h"

void pingpong_init()
{
    // função fornecida pelo professor para desativar o buffer da saida padrao (stdout), usado pela função printf
    setvbuf (stdout, 0, _IONBF, 0) ;

    numeroTask = 0;

    taskMain.tid = 0;//iniciamos com a main sendo identificador 0

	getcontext(&(taskMain.taskContext)); //atribuimos o contexto atualmente ativo para a taskMain

    taskMain.prev = &taskMain;
    taskMain.next = &taskMain;

	taskAtual = &taskMain;

    #ifdef DEBUG
	printf("Criou Task Main %d\n", taskMain.tid);
	#endif
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
    numeroTask++;
    int proximoID = numeroTask;
    task->tid = proximoID;

    #ifdef DEBUG
    printf("Tarefa %d criada\n", task->tid);
	#endif

   return task->tid;
}

int task_switch (task_t *task){

    if(!task){ //caso a task seja invalida retorna -1
        return -1;
    }

    //int troca;//variavel que recebe o retorno da troca de contexto
	task_t *taskAux; //variavel auxiliar para guardar a task

	taskAux = taskAtual;

    #ifdef DEBUG
	printf("Trocou de %d para %d\n", taskAtual->tid, task->tid);
	#endif
    taskAtual = task; //definimos que nossa taskAtual é a que nos foi passada por parametro
	//troca =
	swapcontext(&(taskAux->taskContext), &(task->taskContext)); //salva o contexto da task anterior e muda para a task que foi passada por parametro
    return 0;
}

void task_exit (int exit_code){

    #ifdef DEBUG
	printf("Tarefa %d terminada\n", taskAtual->tid);
	#endif

    task_switch(&taskMain);
}

int task_id (){

    #ifdef DEBUG
	printf("Id da Tarefa %d", taskAtual->tid);
	#endif

	return taskAtual->tid;
}
