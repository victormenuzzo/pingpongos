// PingPongOS - PingPong Operating System
// Prof. Carlos A. Maziero, DAINF UTFPR
// Versão 1.0 -- Março de 2015
//
// Estruturas de dados internas do sistema operacional
#include <stdio.h>
#include <stdlib.h>
#include <ucontext.h>
#include "queue.h"
#include <signal.h>
#include <sys/time.h>
#define STACKSIZE 32768
#ifndef __DATATYPES__
#define __DATATYPES__
//#define DEBUG

// Estrutura que define uma tarefa
typedef struct task_t
{
    struct task_t *prev;
    struct task_t *next;
    ucontext_t taskContext;
    int tid ;
    int taskPrioEst;
    int taskPrioDin;
    int taskUser; //indica se a tarefa eh de usuario ou de sistema
    unsigned int tempoEx;
    unsigned int tempoComeco;
    unsigned int tempoProc;
    int at;
} task_t ;

struct sigaction action;
struct itimerval timer;


task_t *taskAtual; //variavel que define qual é nossa tarefa que está sendo executada atualmente
task_t taskMain; //variavel da taskMain
task_t dispatcher; //tarefa dispatcher
int numeroTask;
task_t *taskProntas;
task_t *taskProxima;
task_t *taskSuspensas;
int quantum;
unsigned int tempo, tempoFinal;
//task_t *taskEscolhida = NULL;

// estrutura que define um semáforo
typedef struct
{
  // preencher quando necessário
} semaphore_t ;

// estrutura que define um mutex
typedef struct
{
  // preencher quando necessário
} mutex_t ;

// estrutura que define uma barreira
typedef struct
{
  // preencher quando necessário
} barrier_t ;

// estrutura que define uma fila de mensagens
typedef struct
{
  // preencher quando necessário
} mqueue_t ;

#endif
