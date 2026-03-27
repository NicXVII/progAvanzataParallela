#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>

#include "ctree.h"

ctree make_cnode(int key, float val)
{
  ctree t = (ctree) malloc(sizeof(struct _ctree_node));
  if (t == NULL) return NULL;

  t->key[0] = key;
  t->val[0] = val;
  t->first_free = 1;

  for(int i = 0; i <= N; i++){
    t->children[i] = NULL;
  }
  return t;
}

ctree cinsert(ctree t, int key, float val)
{
  if (t == NULL) {
    return make_cnode(key, val);
  }
  if(t->first_free == N-1){
    if(key == t->key[N-1]){
      t->val[N-1] = val;
    } else if (key < t->key[N-1]) {
      t->children[N-1] = cinsert(t->children[N-1], key, val);
    } else {
      t->children[N] = cinsert(t->children[N], key, val);
    }
  }
 

  return t;
}

bool csearch(ctree t, int key, float * val)
{
  if(t == NULL) {
    return false;
  }

  for(int i = 0; i < t->first_free; i++){
    if(key == t->key[i]){
      *val = t->val[i];
      return true;
    } else if (key < t->key[i]) {
      return csearch(t->children[i], key, val);
    }
  }
  return csearch(t->children[t->first_free], key, val);
  return false;
}

void print_ctree(ctree t)
{
  printf("NON IMPLEMENTATO");
}
