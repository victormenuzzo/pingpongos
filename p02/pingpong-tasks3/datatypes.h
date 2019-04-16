// PingPongOS - PingPong Operating System
// Prof. Carlos A. Maziero, DAINF UTFPR
// Versão 1.0 -- Março de 2015
//
// Estruturas de dados internas do sistema operacional
#include <stdio.h>
#include <stdlib.h>
#include <ucontext.h>
#define STACKSIZE 32768
#ifndef __DATATYPES__
#define __DATATYPES__
#define DEBUG

// Estrutura que define uma tarefa
typedef struct task_t
{
    ucontext_t taskContext;
    struct task_t *prev, *next ;
    int tid ;
} task_t ;

task_t *taskAtual; //variavel que define qual é nossa tarefa que está sendo executada atualmente
task_t taskMain; //variavel da taskMain
int numeroTask;
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
