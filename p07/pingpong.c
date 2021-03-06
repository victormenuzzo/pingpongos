#include "pingpong.h"

task_t *scheduler();
void dispatcher_body();
void task_yield ();
int task_id ();
void pingpong_init();
int task_create (task_t *task, void (*start_routine)(void *),  void *arg);
int task_switch (task_t *task);
void task_exit (int exit_code);
void tratador(int sinal);
/*-----------------Inicio do p02-----------------------------*/
void pingpong_init()
{
    // função fornecida pelo professor para desativar o buffer da saida padrao (stdout), usado pela função printf
    setvbuf (stdout, 0, _IONBF, 0) ;

    numeroTask = 0;

    tempo = 0;

    taskMain.tid = 0;//iniciamos com a main sendo identificador 0

	getcontext(&(taskMain.taskContext)); //atribuimos o contexto atualmente ativo para a taskMain

    taskMain.next = NULL;
	taskMain.prev = NULL;
    taskMain.taskUser = 1;
    taskMain.at++;
    taskMain.tempoEx = 0;
    taskMain.tempoComeco = systime();
    taskMain.tempoProc = 0;
    taskMain.taskPrioEst = 0;
    taskMain.taskPrioDin = 0;
    queue_append((queue_t **) &taskProntas,(queue_t*) &taskMain);
	taskAtual = &taskMain;
    

    #ifdef DEBUG
	printf("Criou Task Main %d\n", taskMain.tid);
	#endif

    task_create(&dispatcher, (void*) (dispatcher_body),NULL);//criamos a task dispatcher que é uma tarefa

    // registra a a��o para o sinal de timer SIGALRM
	  action.sa_handler = tratador ;
	  sigemptyset (&action.sa_mask) ;
	  action.sa_flags = 0 ;
	  if (sigaction (SIGALRM, &action, 0) < 0)
	  {
	    perror ("Erro em sigaction: ") ;
	    exit (1) ;
	  }

	  // ajusta valores do temporizador
	  timer.it_value.tv_usec = 1000;      // primeiro disparo, em micro-segundos alteramos de 1000ms para 1120 pois 1000 estava dando errado
	  timer.it_value.tv_sec  = 0.001;      // primeiro disparo, em segundos
	  timer.it_interval.tv_usec = 1000;   // disparos subsequentes, em micro-segundos  alteramos de 1000ms para 1120 pois 1000 estava dando errado
	  timer.it_interval.tv_sec  = 0.001;   // disparos subsequentes, em segundos

	  // arma o temporizador ITIMER_REAL (vide man setitimer)
	  if (setitimer (ITIMER_REAL, &timer, 0) < 0)
	  {
	    perror ("Erro em setitimer: ") ;
	    exit (1) ;
	  }


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
    task->next = NULL;
	task->prev = NULL;
	task->taskUser = 0; //esse caso eh para o dispatcher, se for de usuario vai ser mudado do if
	task->tempoEx = 0;
    task->tempoComeco = systime();
    task->tempoProc = 0;
	task->at = 0;

    if (&dispatcher != task){ //temos que ver se a tarefa que está sendo criada não é o dispatcher
		  queue_append((queue_t **) &taskProntas,(queue_t*) task);//se não for inserimos ela na fila de tarefas
      task->taskPrioEst = 0;
      task->taskPrioDin = 0;
      task->taskUser = 1;

    }


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
	printf("Tarefa %d terminada\n", task_id());
	#endif
    taskAtual->tempoEx = systime() - taskAtual->tempoComeco;     

	printf("Task %d exit: execution time %dms, processor time %dms, %d activations", task_id(), taskAtual->tempoEx, taskAtual->tempoProc, taskAtual->at);

	if(taskAtual != &dispatcher){    //caso a tarefa finalizada nao seja o dispatcher, ela deve ser removida da fila de prontos e o controle deve ser retornado ao dispatcher
		queue_remove((queue_t **)&taskProntas, (queue_t *)taskAtual);
		task_switch(&dispatcher);
	}
	else{   //se for o dispatcher, o controle volta para a main
    	task_switch(&taskMain);
	}
}

int task_id (){

    #ifdef DEBUG
	printf("Id da Tarefa %d", taskAtual->tid);
	#endif

	return taskAtual->tid;
}
/*-----------------Fim do p02-----------------------------*/

/*-----------------Inicio do p03--------------------------*/

//permite a uma tarefa voltar ao final da fila de prontas, devolvendo o processador ao dispatcher
void task_yield (){
	if(task_id() != 1){ //utilizamos o id da task atual diferente de 1, pois 1 eh o dispatcher 
		queue_append((queue_t **) &taskProntas,(queue_t*) taskAtual); //coloca na fila taskAtiva a taskAtual
	}
	task_switch(&dispatcher);//faz o task_switch para a função dispatcher
}


void dispatcher_body(){// dispatcher é uma tarefa
    dispatcher.at++;
    while ( ((queue_t *) taskProntas)){
        dispatcher.at++;
        taskProxima = scheduler(); // scheduler é uma função que ainda não foi implementada
        if (taskProxima){
        	quantum = 20; //declaramos o quantum com o valor de 20 ticks
            queue_remove((queue_t **) &taskProntas,(queue_t*) taskProxima);
            // ações antes de lançar a tarefa "next", se houverem
            task_switch (taskProxima) ; // transfere controle para a tarefa "next"
            // ações após retornar da tarefa "next", se houverem
        }
    }
    task_exit(0) ; // encerra a tarefa dispatcher
}

/*-----------------Fim do p03-----------------------------*/

/*-----------------Inicio do p04-----------------------------*/
void task_setprio(task_t *task, int prio){
  if(task && task->taskPrioEst > -20 && task->taskPrioEst < 20 ){ //caso a tarefa exista e sua prioridade esteja entre os limites -20 e 20
    task->taskPrioEst = prio;
    task->taskPrioDin = prio;
  }
  else if(task == NULL){    //caso a tarefa seja nula, ajustamos a prioridade da tarefa atual
    taskAtual->taskPrioEst = prio;
    taskAtual->taskPrioDin = prio;
  }
}

int task_getprio (task_t *task){   //obtem o valor da prioridade estatica da task passada por parametro
  if(task){
    return task->taskPrioEst;
  }
  else{
    return taskAtual->taskPrioEst;
  }

}

task_t *scheduler(){ //função scheduler que implementa politica prioridade com envelhecimento
  task_t *taskAuxProx = taskProntas->next; 
  task_t *taskMPrio = taskProntas;

  if(taskMPrio->taskPrioDin > taskAuxProx->taskPrioDin){  
      taskMPrio = taskAuxProx;
  }
  taskAuxProx = taskAuxProx->next;

  while(taskAuxProx != taskProntas){
    if(taskMPrio->taskPrioDin > taskAuxProx->taskPrioDin){  //acha a task que tem a maior prioridade(menor numero)
      taskMPrio = taskAuxProx;
    }
    taskAuxProx = taskAuxProx->next; 
  }

  if(taskAuxProx == taskMPrio){
    taskMPrio->taskPrioDin = taskMPrio->taskPrioEst;
  }
  else{
    taskAuxProx->taskPrioDin = taskAuxProx->taskPrioDin - 1;
  }
  taskAuxProx = taskAuxProx->next;
  
  while(taskAuxProx != taskProntas){//rodamos todas as tarefas
    if(taskAuxProx == taskMPrio){ //se a que esta atual for a de maior prioridade
      taskMPrio->taskPrioDin = taskMPrio->taskPrioEst; 
    }
    else{//caso nao seja a de maior prioridade
      taskAuxProx->taskPrioDin = taskAuxProx->taskPrioDin - 1;//envelhece
    }
    taskAuxProx = taskAuxProx->next;     
  }
  
  return taskMPrio;
}
//mudancas no createtask
/*-----------------Fim do p04-----------------------------*/

/*-----------------Inicio do p05-----------------------------*/
void tratador(int sinal){
	tempo = systime() + 1;
	if(taskAtual->taskUser){    //se a task for de usuario
		if(quantum != 0){
			quantum--;
            taskAtual->tempoProc++;
		}
		else{
			#ifdef DEBUG
			printf("fim do quantum");
			#endif

			taskAtual->at++; 

			task_yield(); 
		}
	}   

}

/*-----------------Fim do p05-----------------------------*/

/*-----------------Inicio do p06-----------------------------*/
unsigned int systime(){
	return tempo;
}

/*-----------------Fim do p06-----------------------------*/
