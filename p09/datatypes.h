// PingPongOS - PingPong Operating System
// Prof. Carlos A. Maziero, DAINF UTFPR
// Versão 1.0 -- Março de 2015
//
// Estruturas de dados internas do sistema operacional

#include <ucontext.h>
#ifndef __DATATYPES__
#define __DATATYPES__

typedef enum status {READY, EXEC, SUSP, ENDED, SLEEP} status_t;


// Estrutura que define uma tarefa
typedef struct task_t
{
    struct task_t *prev;
    struct task_t *next;
    struct queue_t **fila;
    ucontext_t taskContext;
    int tid ;
    int taskPrioEst;
    int taskPrioDin;
    int taskUser; //indica se a tarefa eh de usuario ou de sistema
    unsigned int tempoEx;
    unsigned int tempoComeco;
    unsigned int tempoProc;
    int at;
    int exitCode;
    status_t  status;
    struct task_t *dependente;
    int bloqueio;
    int tempoAcordar;
} task_t ;


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
