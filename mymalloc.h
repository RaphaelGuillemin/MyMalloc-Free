// Fichier header pour définir les deux fonctions tu TP 2

// Noter que les fonctions d'appellent mymalloc et myfree
// au lieu de malloc et free.

#ifndef MYMALLOC_H_
#define MYMALLOC_H_

#include <stddef.h>

void *mymalloc(size_t size);

int refinc(void *ptr);

void myfree(void *ptr);

#endif // MYMALLOC_H_
