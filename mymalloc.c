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
	//si la taille de bloc demandée est plus petite que la taille par défault d'un chunk
	if (chunkSize >= size) {
		newChunk = malloc(chunkSize+sizeof(memoryChunk)+sizeof(block));
		blockPtr = (block*)(newChunk+sizeof(memoryChunk));
		blockPtr->address = (blockPtr+1);
		blockPtr->size = size;
		blockPtr->count = 1;
		blockPtr->free = false;
		blockPtr->prevBlock = NULL;

		//créer un bloc vide de la taille restante du chunk
		block* freeBlock = (block*)(blockPtr->address + blockPtr->size);
		freeBlock->address = freeBlock+1;
		freeBlock->size = chunkSize - blockPtr->size;
		freeBlock->count = 0;
		freeBlock->free = true;
		freeBlock->prevBlock = blockPtr;
		freeBlock->nextBlock = NULL;
		blockPtr->nextBlock = freeBlock;
		newChunk->maxFreeBlock = freeBlock;

		//si la taille de bloc demandée est plus grande que la taille par défault d'un chunk
	} else {
		//créer un nouveau chunk avec le nouveau bloc de taille demandée prenant tout l'espace du chunk
		newChunk = malloc(size + sizeof(block) + sizeof(memoryChunk));
		blockPtr = (block*)(newChunk+1);
		blockPtr->address = (blockPtr+1);
		blockPtr->size = size;
		blockPtr->count = 1;
		blockPtr->free = false;
		blockPtr->prevBlock = NULL;
		blockPtr->nextBlock = NULL;
		newChunk->maxFreeBlock = NULL;
	}
	//actualiser pointeurs
	newChunk->head = blockPtr;
	newChunk->nextChunk = NULL;
	numberOfChunks++;

	//si premier appel a create chunk, définir le premier pointeur de la mémoire
	if (memory == NULL) {
		memory = newChunk;
		lastChunk = newChunk;
	} else {
		lastChunk->nextChunk = newChunk;
		lastChunk = newChunk;
	}

	return newChunk;
};

//trouver le chunk contenant le bloc libre le plus grand parmi tous les chunks
void findBestChunk() {
	memoryChunk* curChunk = memory;
	memoryChunk* bestFreeSizeChunk = curChunk;
	//passer au travers de tous les chunks
	while(curChunk->nextChunk != NULL) {
		//si block libre plus grand que celui de l'ancien plus grand chunk, remplacer par ce nouveau chunk
		if(curChunk->maxFreeBlock != NULL && bestFreeSizeChunk->maxFreeBlock != NULL &&
			curChunk->maxFreeBlock->size > bestFreeSizeChunk->maxFreeBlock->size) {
			bestFreeSizeChunk = curChunk;
		}
		curChunk = curChunk->nextChunk;
	}
	//si aucun chunk libre, mettre plus grand chunk libre a null
	if(bestFreeSizeChunk->maxFreeBlock == NULL) {
		bestChunk = NULL;
	} else {
		bestChunk = bestFreeSizeChunk;
	}
};

//trouver le bloc le plus grand d'un chunk
void findMaxFreeBlock(memoryChunk *chunk){
	block *findBlock = chunk->head;
	block *maxBlock = findBlock;
	//tant qu'il reste des blocs a visiter
	while(findBlock->nextBlock != NULL){
		//si bloc libre plus grand que l'ancien plus grand, actualiser a celui-ci
		if (findBlock->free == true && findBlock->size >= maxBlock->size){
			maxBlock = findBlock;
		}
		findBlock = findBlock->nextBlock;
	}
	//si aucun bloc libre, mettre plus grand bloc libre a null
	if(maxBlock->free == false){
		chunk->maxFreeBlock = NULL;
	} else {
		chunk->maxFreeBlock = maxBlock;
	}
};

//retourne une plage mémoire assez large.
void *mymalloc(size_t size){
	//si espace mémoire demandé de 0, ne rien faire
	if (size == 0){
		return NULL;
	//Si aucun chunk, créer premier chunk et premier block
	} else if (memory == NULL) {
		memoryChunk *chunk = createNewChunk(size);
		bestChunk = chunk;

		return (chunk->head->address);

	// Tous les chunks sont remplis, en ajouter un avec le bloc
	} else if (bestChunk == NULL) {
		memoryChunk *chunk = createNewChunk(size);
		if(chunk->maxFreeBlock != NULL) {
			bestChunk = chunk;
		}
		return chunk->head->address;
	//la taille demandée est plus grande que la taille maximale disponible dans un chunk, en créer un nouveau
	} else if (size > bestChunk->maxFreeBlock->size) {
		memoryChunk *chunk = createNewChunk(size);
		//assigner le chunk a celui de taille maximale si nécessaire
		if(chunk->maxFreeBlock != NULL && chunk->maxFreeBlock->size > bestChunk->maxFreeBlock->size){
			bestChunk = chunk;
		}
		return (chunk->head->address);

		//Si on a un bloc disponible de la taille exacte
	} else if(size == bestChunk->maxFreeBlock->size) {
		block* blockPtr = bestChunk->maxFreeBlock;
		blockPtr->free = false;
		blockPtr->count = 1;
		findMaxFreeBlock(bestChunk);
		findBestChunk();

		return blockPtr->address;

	// Si la taille du plus grand block free est plus grande que la size voulue
	} else {
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

//incrémenter le compteur de pointeurs d'un bloc
int refinc(void *ptr) {
	block *block = ptr-sizeof(block);
	block->count++;
	return block->count;
};

//trouver le chunk d'un bloc
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

//réduire le compteur de pointeurs d'un bloc et libére la mémoire du bloc si compteur = 0;
void myfree(void *ptr){
	//si pointeur null, ne rien faire
	if (ptr == NULL){
		return;
	}
	block* ptrBlock = ((block*) ptr) - 1;
	ptrBlock->count = ptrBlock->count - 1;

	//si compteur = 0, libérer mémoire
  if (ptrBlock->count == 0){
		ptrBlock->free = true;
		//si bloc vide est précéder d'un bloc vide, les rassembler
		if (ptrBlock->prevBlock!=NULL && ptrBlock->prevBlock->free == true) {
			ptrBlock->prevBlock->size = ptrBlock->prevBlock->size + ptrBlock->size;
			ptrBlock->prevBlock->nextBlock = ptrBlock->nextBlock;
			ptrBlock = ptrBlock->prevBlock;
		}

		//si bloc vide est suivi d'un bloc vide, les rassembler
		if(ptrBlock->nextBlock != NULL && ptrBlock->nextBlock->free == true) {
			ptrBlock->size = ptrBlock->size + ptrBlock->nextBlock->size;
			ptrBlock->nextBlock = ptrBlock->nextBlock->nextBlock;
		}

		//si bloc vide est plus grand que bloc vide du meilleur chunk, le remplacer
		if(bestChunk == NULL || ptrBlock->size > bestChunk->maxFreeBlock->size) {
		memoryChunk *chunkPtr = findChunk(ptrBlock);
		bestChunk = chunkPtr;
		bestChunk->maxFreeBlock = ptrBlock;
		}
	}
};
