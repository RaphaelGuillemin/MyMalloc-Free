// Vous devez modifier ce fichier pour le TP 2
// Tout votre code doit être dans ce fichier
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "mymalloc.h"

void *memory;
int chunkSize = 4096;
int numberOfChunks;

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
	//maxSizeBetweenBlocks contient également les metadonnees des blocks
	int maxSizeBetweenBlocks;
	memoryChunk *nextChunk;
};

//SUREMENT PU VALIDE DEPUIS QU'ON SAIT QU'IL FAUT LES SIZEOF
void* block_get (memoryChunk *chunk, int adress) {
	if (!chunk) return NULL;
	block *block = chunk->head;
	while(block->adress + block->size < adress && block->nextBlock != NULL) {
		block = block->nextBlock;
	}
	return &block;
}

//SUREMENT PU VALIDE DEPUIS QU'ON SAIT QU'IL FAUT LES SIZEOF
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
	//Si aucun chunk, créer premier chunk et premier block
  	if (&memory == NULL){
		memoryChunk *chunk = malloc(chunkSize);
		block block = {0, size, 1, NULL};
		chunk->head = &block;
		chunk->maxSizeBetweenBlocks = chunkSize - block.size - sizeof(block);
		memory = &chunk;
		numberOfChunks = 1;
		return &block + sizeof(block);
	} else {
		//Si chunk deja présent, vérifier que le premier contient assez d'espace pour nouveau block
		memoryChunk *chunk = &memory;
		//Verification taille maximale entre deux blocs est plus grande que la taille souhaitée + la taille de la métadonnée block
		if (chunk->maxSizeBetweenBlocks >= size + sizeof(block)){
			block *currentBlock = chunk->head;
			//Tant que le premier bloc du chunck n'est pas suivi ou si espace insuffisant entre deux blocs, passer au prochain
			while(currentBlock->nextBlock != NULL && currentBlock->nextBlock->adress - (currentBlock->size + currentBlock->adress) < size){
				currentBlock = currentBlock->nextBlock;
			}
			//Si espace trouvé entre 2 block
			if (currentBlock->nextBlock != NULL){
				block *temp = currentBlock->nextBlock;
				block *newBlock = {currentBlock->adress + currentBlock->size + 1, size, 1, temp};
				currentBlock->nextBlock = newBlock;
				//Modifier quand on aura FREE car pour l'instant maxSize = taille restante à droite
				chunk->maxSizeBetweenBlocks = chunk->maxSizeBetweenBlocks - newBlock->size - sizeof(newBlock);
				return &newBlock + sizeof(newBlock);
			} else {
				//Si aucun prochain bloc et assez d'espace pour en ajouter un à droite
				block *newBlock = {currentBlock->adress + currentBlock->size + 1, size, 1, NULL};
				currentBlock->nextBlock = &newBlock;
				//Modifier quand on aura FREE car pour l'instant maxSize = taille restante à droite
				chunk->maxSizeBetweenBlocks = chunk->maxSizeBetweenBlocks - newBlock->size - sizeof(newBlock);
				return &newBlock + sizeof(newBlock);
			}
		} else {
			//tant que pas assez d'espace dans le chunk et prochain chunk disponible, passer au prochain chunk
			while(chunk->nextChunk != NULL && chunk->maxSizeBetweenBlocks < size + sizeof(block)){
				chunk = chunk->nextChunk;
			}
			//si prochain chunk vide et pas assez d'espace dans le dernier visité, en créer un nouveau
			if(chunk->nextChunk == NULL && chunk->maxSizeBetweenBlocks < size + sizeof(block)){
				memoryChunk *newChunk = malloc(chunkSize);
				block block = {chunkSize*numberOfChunks+1, size, 1, NULL};
				newChunk->head = &block;
				newChunk->maxSizeBetweenBlocks = chunkSize - block.size - sizeof(block);
				chunk->nextChunk = newChunk;
				numberOfChunks++;
				return &block + sizeof(block);
			} else {
				//Si assez d'espace pour ajouter un bloc dans le dernier chunk visité
				block *currentBlock = chunk->head;
				//trouver un espace entre 2 blocks ou arriver au dernier block
				while(currentBlock->nextBlock !=NULL && currentBlock->nextBlock->adress - (currentBlock->size + currentBlock->adress) < size){
					currentBlock = currentBlock->nextBlock;
				}
				//Si espace trouvé entre 2 block
				if (currentBlock->nextBlock != NULL){
					block *temp = currentBlock->nextBlock;
					block *newBlock = {currentBlock->adress + currentBlock->size + 1, size, 1, temp};
					currentBlock->nextBlock = newBlock;
					//Modifier quand on aura FREE car pour l'instant maxSize = taille restante à droite
					chunk->maxSizeBetweenBlocks = chunk->maxSizeBetweenBlocks - newBlock->size - sizeof(newBlock);
					return &newBlock + sizeof(newBlock);
				} else {
					//Si aucun prochain bloc et assez d'espace pour en ajouter un à droite
					block *newBlock = {currentBlock->adress + currentBlock->size + 1, size, 1, NULL};
					currentBlock->nextBlock = &newBlock;
					//Modifier quand on aura FREE car pour l'instant maxSize = taille restante à droite
					chunk->maxSizeBetweenBlocks = chunk->maxSizeBetweenBlocks - newBlock->size - sizeof(newBlock);
					return &newBlock + sizeof(newBlock);
				}
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
