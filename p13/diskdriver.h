// PingPongOS - PingPong Operating System
// Prof. Carlos A. Maziero, DAINF UTFPR
// Versão 1.0 -- Março de 2015
//
// interface do driver de disco rígido
#include "datatypes.h"
#ifndef __DISKDRIVER__
#define __DISKDRIVER__

// structura de dados que representa o disco para o SO
typedef struct filaPedidos_t{
	struct filaPedidos_t *prev, *next;
	int bloco;
	int atend;
	void *buffer;
	task_t *dono;
	int cmd;

} filaPedidos_t;

typedef struct disk_t
{
	struct  filaPedidos_t *filaPedidos;
	int numPedidos;
	int signal;
	semaphore_t semDisco;
	
} disk_t ;

// inicializacao do driver de disco
// retorna -1 em erro ou 0 em sucesso
// numBlocks: tamanho do disco, em blocos
// blockSize: tamanho de cada bloco do disco, em bytes
int diskdriver_init (int *numBlocks, int *blockSize) ;

// leitura de um bloco, do disco para o buffer indicado
int disk_block_read (int block, void *buffer) ;

// escrita de um bloco, do buffer indicado para o disco
int disk_block_write (int block, void *buffer) ;

#endif
