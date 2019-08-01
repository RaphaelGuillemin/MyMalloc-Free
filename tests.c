// Vous pouvez compiler tests.c comme ceci depuis le terminal:
// gcc -o tests tests.c mymalloc.c
// Ensuite vous pouvez exécuter avec : ./tests 

#include <stdio.h>
#include <signal.h>
#include <string.h>
#include <setjmp.h>
#include "mymalloc.h"

// Context pour recover de segfault.
jmp_buf ctx;

// Free doit accepter le pointeur NULL
void test1(){
  myfree(NULL);
}

// Utilisation très simple de malloc et free
void test2(){
  int *a = mymalloc(sizeof(int));
  *a = 0;
  myfree(a);
}

// Test simple pour voir si free fonctionne
// Chaque demande entre dans un bloc mémoire de 4 Ko
void test3(){
  for (int i = 0; i < 500000; i++){
    int *a = mymalloc(sizeof(int) * 512);
    for(int j = 0; j < 512; j ++){
      a[j]= j;
    }
    myfree(a);
  }
}

// Utilisation très simple de malloc avec une grande taille
void test4(){
  int *a = mymalloc(sizeof(int) * 1024 * 1024);
  for(int j = 0; j < 1024 * 1024; j ++){
    a[j]= j;
  }
  myfree(a);
}

// Test simple pour voir si free fonctionne avec un malloc de grande taille
// Il suffit de regarder la mémoire utiliser par le programme
void test5(){
  for (int i = 0; i < 10000; i++){
    int *a = mymalloc(sizeof(int) * 1024 * 1024);
    for(int j = 0; j < 1024 * 1024; j += 1000){
      a[j]= j;
    }
    myfree(a);
  }
}

typedef struct node node;
struct node {
  int valueInt;
  char valueChar;
  node *next;
  node *previous;
};

// Petit programme pour testé si malloc et free fonctionne bien
int test6(){
  // La racine
  node *root = mymalloc(sizeof(node));
  root->valueInt = 0;
  root->valueChar = 115;
  root->next = NULL;
  root->previous = NULL;

  // Création d'une liste doublement chaînée
  node *current = root;
  for(int i = 0; i < 4 ; i ++){
    node *p = mymalloc(sizeof(node));
    p->valueInt = i;
    p->valueChar = 115;
    p->next = NULL;
    p->previous = current;
    current->next = p;
    current = p;
  }
  printf("étape 1");
  // On efface quelques éléments
  current = root;
  int n = 0;
  while (current->next){
    n++;
    if (n % 23 != 0) {
      // Rien à faire
      current = current->next;
    }
    else {
      // On efface ce noeud
      node *tmp;
      current->previous->next = current->next;
      current->next->previous = current->previous;
      tmp = current->next;
      myfree(current);
      current = tmp;
    }
  }
  printf("étape 2");
  // Quelques appel à malloc voir si cela vous mélange un peu
  void *ptrArray[1024];
  for(int i = 0; i < 1024; i ++){
    int *d = mymalloc(sizeof(double));
    *d = 435;
    ptrArray[i] = d;
  }
  printf("étape 3");
  // On ajoute quelques éléments
  current = root;
  n = 0;
  while (current->next){
    n++;
    if (n % 27 != 0) {
      // Rien à faire
      current = current->next;
    }
    else {
      // On ajoute un noeud
      node *p = mymalloc(sizeof(node));
      p->valueInt = 2346;
      p->valueChar = 115;
      p->next = current->next;
      p->previous = current;
      current->next->previous = p;
      current->next = p;
      current = p->next;
    }
  }
  printf("étape 4");
  // Calcul de la somme
  current = root;
  long sum = root->valueInt + root->valueChar;
  while (current->next){
    sum += current->valueInt + current->valueChar;
    current = current->next;
  }
  printf("étape 5");
  // Nettoyage de la mémoire
  current = root;
  while (current->next){
    node *tmp = current->next;
    myfree(current);
    current = tmp;
  }
  myfree(current);
  printf("étape 6");
  for(int i = 0; i < 1024; i ++){
    myfree(ptrArray[i]);
  }
  printf("étape 7");
  return sum;
}

void sigseg_handler(int signum) {
#if defined(_WIN32) || defined(_WIN64)
  signal(SIGSEGV, &sigseg_handler);
#endif
  printf("**Segfault\n");
  longjmp(ctx, 1);
}

#if !defined(_WIN32) && !defined(_WIN64)
#define signal(signum, handler)\
  do {\
    struct sigaction act; \
    memset(&act, '\0', sizeof(act));\
    act.sa_handler = handler;\
    act.sa_flags = SA_NODEFER;\
    sigaction(SIGSEGV, &act, NULL); \
  } while (0)
#endif

#define TEST_NOSEGFAULT(test, number) \
  if(!setjmp(ctx)) {\
    test;\
    printf("Test "#number" OK\n");\
  } else \
    printf("Test "#number" KO\n")


int main(){
  signal(SIGSEGV, &sigseg_handler);
  TEST_NOSEGFAULT(test1(), 1);

  TEST_NOSEGFAULT(test2(), 2);

  TEST_NOSEGFAULT(test3(), 3);

  TEST_NOSEGFAULT(test4(), 4);

  TEST_NOSEGFAULT(test5(), 5);

  long res = 0;
  TEST_NOSEGFAULT(res=test6(), 6);
  if (res == 632474318)
    printf("Test 6 %ld OK\n", res);
  else
    printf("Test 6 %ld KO\n", res);

  printf("Done\n");
  return 0;
}

