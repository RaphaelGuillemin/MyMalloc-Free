// Vous devez modifier ce fichier pour le TP 2
// Tout votre code doit être dans ce fichier
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "mymalloc.h"

//taille par défault des chunks de mémoire
int chunkSize = 4096;
//le nombre de chunks de mémoire
int numberOfChunks;

//structure d'un bloc de mémoire (liste chainée de blocs)
typedef struct block block;
struct block{
	void* address; //adresse du data apres la metadonnée
	size_t size; //size du data sans la metadonnee
	int count; //nombre de pointeurs sur le bloc
	bool free;
	block *nextBlock;
	block *prevBlock;
};

//structure d'un chunk de mémoire (liste chainée de chunks)
typedef struct memoryChunk memoryChunk;
struct memoryChunk{
	block *head; //pointeur vers le premier bloc du chunk
	block *maxFreeBlock; //pointeur vers le bloc de mémoire libre le plus grand
	memoryChunk *nextChunk;
};

memoryChunk* memory; //pointeur vers le premier chunk de memoire
memoryChunk* lastChunk; //pointeur vers le dernier chunk de memoire
memoryChunk* bestChunk; //pointeur vers le chunk contenant le bloc libre le plus grand

//fonction qui crée un nouveau chunk
memoryChunk* createNewChunk(int size) {
	memoryChunk *newChunk; //le nouveau chunk
	block* blockPtr; //le nouveau bloc
	//si la taille de bloc demandée est plus grande que la taille
	if (chunkSize >= size) {
		newChunk = malloc(chunkSize+sizeof(memoryChunk)+sizeof(block));
		blockPtr = (block*)(newChunk+sizeof(memoryChunk));
		blockPtr->address = (blockPtr+1);
		blockPtr->size = size;
		blockPtr->count = 1;
		blockPtr->free = false;
		blockPtr->prevBlock = NULL;

		block* freeBlock = (block*)(blockPtr->address + blockPtr->size);
		freeBlock->address = freeBlock+1;
		freeBlock->size = chunkSize - blockPtr->size;
		freeBlock->count = 0;
		freeBlock->free = true;
		freeBlock->prevBlock = blockPtr;
		freeBlock->nextBlock = NULL;
		blockPtr->nextBlock = freeBlock;
		newChunk->maxFreeBlock = freeBlock;
	} else {
		newChunk = malloc(size + sizeof(block) + sizeof(memoryChunk));
		blockPtr = (block*)(newChunk+1);
		blockPtr->address = (blockPtr+1);
		blockPtr->size = size;
		blockPtr->count = 1;
		blockPtr->free = false;
		blockPtr->prevBlock = NULL;
		blockPtr->nextBlock = NULL;
		newChunk->maxFreeBlock = NULL;
		//newChunk->maxFreeBlock->size = 0;
	}

	newChunk->head = blockPtr;
	newChunk->nextChunk = NULL;
	numberOfChunks++;

	if (memory == NULL) {
		memory = newChunk;
		lastChunk = newChunk;
	} else {
		lastChunk->nextChunk = newChunk;
		lastChunk = newChunk;
	}

	return newChunk;
};

void findBestChunk() {
	memoryChunk* curChunk = memory;
	memoryChunk* bestFreeSizeChunk = curChunk;
	while(curChunk->nextChunk != NULL) {
		if(curChunk->maxFreeBlock != NULL && bestFreeSizeChunk->maxFreeBlock != NULL &&
			curChunk->maxFreeBlock->size > bestFreeSizeChunk->maxFreeBlock->size) {
			bestFreeSizeChunk = curChunk;
		}
		curChunk = curChunk->nextChunk;
	}
	if(bestFreeSizeChunk->maxFreeBlock == NULL) {
		bestChunk = NULL;
	} else {
		bestChunk = bestFreeSizeChunk;
	}
};

void findMaxFreeBlock(memoryChunk *chunk){
	block *findBlock = chunk->head;
	block *maxBlock = findBlock;
	while(findBlock->nextBlock != NULL){
		if (findBlock->free == true && findBlock->size >= maxBlock->size){
			maxBlock = findBlock;
		}
		findBlock = findBlock->nextBlock;
	}
	if(maxBlock->free == false){
		chunk->maxFreeBlock = NULL;
	} else {
		chunk->maxFreeBlock = maxBlock;
	}
};

void *mymalloc(size_t size){
	// doit retourner une plage mémoire assez large.
	//si espace mémoire demandé de 0, ne rien faire
	if (size == 0){
		return NULL;
	//Si aucun chunk, créer premier chunk et premier block
	} else if (memory == NULL) {
		//printf("memoire vide");
		memoryChunk *chunk = createNewChunk(size);
		bestChunk = chunk;

		return (chunk->head->address);

	// Tous les chunks sont remplis
	} else if (bestChunk == NULL) {
		//printf("Créer nouveau chunk car autres remplis");
		memoryChunk *chunk = createNewChunk(size);
		if(chunk->maxFreeBlock != NULL) {
			bestChunk = chunk;
		}
        return chunk->head->address;
	} else if (size > bestChunk->maxFreeBlock->size) {
		//printf("taille plus grande que bestChunk");
		memoryChunk *chunk = createNewChunk(size);
		if(chunk->maxFreeBlock != NULL && chunk->maxFreeBlock->size > bestChunk->maxFreeBlock->size){
			bestChunk = chunk;
		}
		return (chunk->head->address);

		//Si on a un free block disponible de la bonne taille
	} else if(size == bestChunk->maxFreeBlock->size) {
		//printf("block de taille exacte trouvé");
		block* blockPtr = bestChunk->maxFreeBlock;
		blockPtr->free = false;
		blockPtr->count = 1;
		findMaxFreeBlock(bestChunk);
		findBestChunk();

		return blockPtr->address;

	// Si la taille du plus grand block free est plus grande que la size voulue
	} else {
		//printf("taille bloc free plus grand que taille voulue");
		block *temp = (block *)bestChunk->maxFreeBlock->nextBlock;
		block *newBlock = bestChunk->maxFreeBlock;
		newBlock->address = newBlock + 1;
		newBlock->size = size;
		newBlock->count = 1;
		newBlock->free = false;
		newBlock->prevBlock = bestChunk->maxFreeBlock->prevBlock;

		// Reste du bloc free pas utilisé
		block *freeBlock = (block *)(newBlock->address + newBlock->size);
		freeBlock->address = freeBlock + 1;
		freeBlock->size = bestChunk->maxFreeBlock->size;
		freeBlock->count = 0;
		freeBlock->free = true;
		freeBlock->prevBlock = newBlock;
		freeBlock->nextBlock = temp;

		newBlock->nextBlock = freeBlock;
		findMaxFreeBlock(bestChunk);
		findBestChunk();

		return newBlock->address;
	}
	printf("This shouldn't appear\n");
};

int refinc(void *ptr) {
	block *block = ptr-sizeof(block);
	block->count++;
	return block->count;
};

memoryChunk* findChunk(void* ptr){
	memoryChunk *chunkPtr = memory;
	while(chunkPtr->nextChunk != NULL){
		if(ptr-sizeof(chunkPtr) < (void*) chunkPtr->nextChunk){
			break;
		}
		chunkPtr = chunkPtr->nextChunk;
	}
	return chunkPtr;
};

void myfree(void *ptr){
  // décrémente le compteur et libère la mémoire si et seulement si ce dernier est à zéro.
	if (ptr == NULL){
		return;
	}
	block* ptrBlock = ((block*) ptr) - 1;
	ptrBlock->count = ptrBlock->count - 1;

  if (ptrBlock->count == 0){
		ptrBlock->free = true;
		if (ptrBlock->prevBlock!=NULL && ptrBlock->prevBlock->free == true) {
			ptrBlock->prevBlock->size = ptrBlock->prevBlock->size + ptrBlock->size;
			//printf("Premier : %i\n",ptrBlock->prevBlock->size);
			ptrBlock->prevBlock->nextBlock = ptrBlock->nextBlock;
			ptrBlock = ptrBlock->prevBlock;
		}

		if(ptrBlock->nextBlock != NULL && ptrBlock->nextBlock->free == true) {
            //printf("%i\n",ptrBlock->size);
			ptrBlock->size = ptrBlock->size + ptrBlock->nextBlock->size;
			//printf("Second : %i\n",ptrBlock->size);
			//printf("%i\n",ptrBlock->nextBlock->size);
			ptrBlock->nextBlock = ptrBlock->nextBlock->nextBlock;
		}

		if(bestChunk == NULL || ptrBlock->size > bestChunk->maxFreeBlock->size) {
		memoryChunk *chunkPtr = findChunk(ptrBlock);
		bestChunk = chunkPtr;
		bestChunk->maxFreeBlock = ptrBlock;
		}
	}
};
