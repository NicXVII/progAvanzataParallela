#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>

#include "ctree.h"

ctree make_cnode(int key, float val)
{
  ctree t = malloc(sizeof(struct _ctree_node));
  if (t == NULL) {
    return NULL;
  }

  t->key[0] = key;
  t->val[0] = val;
  t->first_free = 1;

  for (int i = 1; i < N; i++) {
    t->key[i] = 0;
    t->val[i] = 0.0f;
  }

  for (int i = 0; i <= N; i++) {
    t->children[i] = NULL;
  }

  return t;
}

ctree cinsert(ctree t, int key, float val)
{
  if (t == NULL) {
    return make_cnode(key, val);
  }

  int pos = 0;
  while (pos < t->first_free && key > t->key[pos]) {
    pos++;
  }

  if (pos < t->first_free && key == t->key[pos]) {
    t->val[pos] = val;
    return t;
  }

  if (t->first_free < N) {
    for (int j = t->first_free; j > pos; j--) {
      t->key[j] = t->key[j - 1];
      t->val[j] = t->val[j - 1];
    }
    t->key[pos] = key;
    t->val[pos] = val;
    t->first_free++;
    return t;
  }

  t->children[pos] = cinsert(t->children[pos], key, val);
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
    if (t == NULL) {
    printf(".");
    return;
  }
    printf("(");

  for(int i = 0; i < t->first_free; i++){

    printf(" %d ", t->key[i]);
    print_ctree(t->children[i]);

  }
  print_ctree(t->children[t->first_free]);
        printf(")");


}
