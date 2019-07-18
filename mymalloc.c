// Vous devez modifier ce fichier pour le TP 2
// Tout votre code doit être dans ce fichier
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "mymalloc.h"

void *head = NULL;
int remainingSize;

typedef struct memoryBlock memoryBlock;

struct memoryBlock{
	size_t size;
	void *adress;
	int count;
	memoryBlock *nextBlock;
};

void *mymalloc(size_t size){
  // doit retourner une plage mémoire assez large.
  // première étape : définir un bloc de 4*1024 octets avec malloc
  	if (&head==NULL){
		head=malloc(4*1024);
		remainingSize = 4*1024;
	}
	
	remainingSize=remainingSize-size;
	
}

int refinc(void *ptr){
  // incrémenter un compteur interne
  	return 0;
}

void myfree(void *ptr){
  // décrémente le compteur et libère la mémoire si et seulement si ce dernier est à zéro.
  	free(ptr);
}
