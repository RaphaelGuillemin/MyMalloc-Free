// Vous devez modifier ce fichier pour le TP 2
// Tout votre code doit être dans ce fichier
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "mymalloc.h"

void *mymalloc(size_t size){
  // doit retourner une plage mémoire assez large.
  return malloc(size);
}

int refinc(void *ptr){
  // incrémenter un compteur interne
  return 0;
}

void myfree(void *ptr){
  // décrémente le compteur et libère la mémoire si et seulement si ce dernier est à zéro.
  free(ptr);
}
