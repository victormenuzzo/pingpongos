#include <stdio.h>
#include <stdlib.h>
#include "queue.h"


//------------------------------------------------------------------------------
// Insere um elemento no final da fila.
// Condicoes a verificar, gerando msgs de erro:
// - a fila deve existir
// - o elemento deve existir
// - o elemento nao deve estar em outra fila

void queue_append (queue_t **queue, queue_t *elem){

	queue_t *aux, *primeiroQ;
	if(elem == NULL){
		printf("Elemento inexistente.");	
		return;
	}
	if(elem->prev != NULL || elem->next != NULL){
		printf("Elemento pertencente a uma fila.");
		return;
	}
	if(queue == NULL){
		printf("Fila não existente.");
		return;
	}
	if(!*queue){
		////printf("Fila vazia.");
		aux = elem;
		aux->next = elem;
		aux->prev = elem;
		*queue = aux;
		return;
	}	
		aux = elem;
		primeiroQ = *queue;
		primeiroQ->prev->next = aux;
		aux->prev = primeiroQ->prev;
		aux->next = primeiroQ;
		primeiroQ->prev = aux;
}

//------------------------------------------------------------------------------
// Remove o elemento indicado da fila, sem o destruir.
// Condicoes a verificar, gerando msgs de erro:
// - a fila deve existir
// - a fila nao deve estar vazia
// - o elemento deve existir
// - o elemento deve pertencer a fila indicada
// Retorno: apontador para o elemento removido, ou NULL se erro

queue_t *queue_remove (queue_t **queue, queue_t *elem){
	
	queue_t *aux, *primeiroQ;

	if(*queue == NULL){
		//printf("Fila inexistente.");
		return NULL;
	}
	if(*queue == NULL){
		//printf("Fila vazia.");
		return NULL;
	}
	if(elem == NULL){
		//printf("Elemento inexistente.");
		return NULL;	
	}
	primeiroQ = *queue;

	if (elem == primeiroQ) {
		if (queue_size(*queue) == 1)
			*queue=NULL;
		else {
			primeiroQ->next->prev = primeiroQ->prev;
			primeiroQ->prev->next = primeiroQ->next;
			primeiroQ = primeiroQ->next;
			*queue = primeiroQ;
		}
		elem->prev = NULL;
		elem->next = NULL;
		return elem;
	}
	else{
		for(aux=*queue; aux->next!=*queue; aux=aux->next){
			if(aux == elem){
				aux->prev->next = aux->next;
				aux->next->prev = aux->prev;
				aux->next = NULL;
				aux->prev = NULL;
				return aux;
			}
		}

		if(aux == elem){
			aux->prev->next = aux->next;
			aux->next->prev = aux->prev;
			aux->next = NULL;
			aux->prev = NULL;
			return aux;
		}
	}
	//printf("Elemento não está na fila.");
	return NULL;

}


//------------------------------------------------------------------------------
// Conta o numero de elementos na fila
// Retorno: numero de elementos na fila

int queue_size (queue_t *queue){
	
	int i = 0; 
	
	queue_t *aux;

	if(!queue)
		return 0;

	for(aux = queue; aux->next!=queue; aux = aux->next)
		i++;

	return i+1; // colocamos i+1 pois é necessário contar o ultimo elemento
}


//------------------------------------------------------------------------------
// Percorre a fila e imprime na tela seu conteúdo. A impressão de cada
// elemento é feita por uma função externa, definida pelo programa que
// usa a biblioteca. Essa função deve ter o seguinte protótipo:
//

// void print_elem (void *ptr) ; // ptr aponta para o elemento a imprimir

void queue_print (char *name, queue_t *queue, void print_elem (void*)){
	
	queue_t *aux;
	puts(name);
	if(!queue){
		return;
	}
	for(aux=queue; aux->next!=queue; aux = aux->next){
		print_elem (aux);		
	}
	print_elem (aux);
}
