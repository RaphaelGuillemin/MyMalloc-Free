// Vous devez modifier ce fichier pour le TP 2
// Tout votre code doit être dans ce fichier

#include <stdlib.h>
#include "mymalloc.h"

void *mymalloc(size_t size){
  // À modifier
  return malloc(size);
}

int refinc(void *ptr){
  // À modifier
  return 0;
}

void myfree(void *ptr){
  // À modifier
  free(ptr);
}
