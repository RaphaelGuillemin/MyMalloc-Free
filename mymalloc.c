// Vous devez modifier ce fichier pour le TP 2
// Tout votre code doit être dans ce fichier
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "mymalloc.h"

void *memory;
int chunkSize = 4096;

typedef struct block block;
struct block{
	int adress;
	size_t size;
	int count;
	block *nextBlock;
};

typedef struct memoryChunk memoryChunk;
struct memoryChunk{
	block *head;
	int remainingSize;
	memoryChunk *nextChunk;
};

void* block_get (memoryChunk *chunk, int adress) {
	if (!chunk) return NULL;
	block *block = chunk->head;
	while(block->adress + block->size < adress && block->nextBlock != NULL) {
		block = block->nextBlock;
	}
	return &block;
}

void* chunk_get (void *memory, int byte) {
	if (!memory) return NULL;
	memoryChunk *chunk = memory;
	while(byte > 0 && chunk->nextChunk != NULL) {
		chunk = chunk->nextChunk;
		byte = byte - 4096;
	}
}


void *mymalloc(size_t size){
  // doit retourner une plage mémoire assez large.
  // première étape : définir un bloc de 4*1024 octets avec malloc
  	if (&memory == NULL){
		memoryChunk *chunk = malloc(4*1024);
		block block = {0, size, 1, NULL};
		chunk->head = &block;
		chunk->remainingSize = 4*1024 - block.size;
		memory = &chunk;
	} else {

	}


	
}

int refinc(void *ptr){
  // incrémenter un compteur interne
  	return 0;
}

void myfree(void *ptr){
  // décrémente le compteur et libère la mémoire si et seulement si ce dernier est à zéro.
  	free(ptr);
}
