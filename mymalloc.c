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
	int maxSizeBetweenBlocks;
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
		byte = byte - chunkSize;
	}
	return &chunk;
}

void *mymalloc(size_t size){
  // doit retourner une plage mémoire assez large.
  // première étape : définir un bloc de 4*1024 octets avec malloc
  	if (&memory == NULL){
		memoryChunk *chunk = malloc(chunkSize);
		block block = {0, size, 1, NULL};
		chunk->head = &block;
		chunk->maxSizeBetweenBlocks = chunkSize - block.size - sizeof(block);
		memory = &chunk;
		return &block+sizeof(block);
	} else {
		memoryChunk *chunk = &memory;
		//Verification taille maximale entre deux blocs est plus grande que la taille souhaitée + la taille de la métadonnée block
		if (chunk->maxSizeBetweenBlocks > size + sizeof(block)){
			block *currentBlock = chunk->head;
			//Si le premier bloc du chunck n'est pas suivi ou si espace suffisant entre deux blocs
			while(currentBlock->nextBlock !=NULL && currentBlock->nextBlock->adress - currentBlock->size + sizeof(currentBlock) < size){
				currentBlock = currentBlock->nextBlock;
			}
			block *newBlock = {currentBlock->size + currentBlock->adress + 1, size, 1, NULL};
			currentBlock->nextBlock = &newBlock;
			chunk->maxSizeBetweenBlocks = chunk->maxSizeBetweenBlocks - newBlock->size - sizeof(newBlock);
		} else if (chunk->nextChunk == NULL){
			//CREER NOUVEAU CHUNK
			memoryChunk *chunk = malloc(chunkSize);
			block block = {0, size, 1, NULL};
			chunk->head = &block;
			chunk->maxSizeBetweenBlocks = chunkSize - block.size - sizeof(block);
		} else {
			//PASSER AU PROCHAIN CHUNK
			while(chunk->nextChunk != NULL && chunk->maxSizeBetweenBlocks < size){
				chunk = chunk->nextChunk;
			}
		}
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
