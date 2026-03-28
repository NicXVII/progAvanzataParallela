#include <stdlib.h>
#include <stdio.h>

#include "unrolled_linked_list.h"

unrolled_linked_list ulst_make(void)
{
  unrolled_linked_list node = malloc(sizeof(struct _unrolled_linked_list));
  if (node == NULL) {
      return NULL;
  }
  node->head = NULL;
  return node;
}

void ulst_delete(unrolled_linked_list lst)
{
if (lst == NULL){\
    return;
  }

  unode current = lst->head;
  while (current != NULL) {
    unode next = current->next;
    free(current);
    current = next;
  }
  free(lst);
}

void ulst_add(unrolled_linked_list lst, int key)
{
    if (lst == NULL) {
        return;
    }

    unode h = lst->head;
    if (h != NULL) {
        for (int i = 0; i < UNROLLED_SIZE; i++) {
            if (!h->valid[i]) {
                h->keys[i] = key;
                h->valid[i] = true;
                return;
            }
        }
    }
    unode new_node = malloc(sizeof(struct _unrolled_node));
    new_node-> valid[0] = true;
    new_node->keys[0] = key;
    


}

bool ulst_search(unrolled_linked_list lst, int key)
{
    /* TODO */
}

void ulst_print(unrolled_linked_list lst)
{
  if (lst == NULL) {
    printf("NIL");
    return;
  }
  printf("(");
  unode current = lst->head;
  while (current != NULL) {
    printf("[");
    for (int i = 0; i < UNROLLED_SIZE; i++) {
      if (current->valid[i]) {
	printf("%d", current->keys[i]);
      } else {
	printf(".");
      }
      if (i < UNROLLED_SIZE - 1) {
	printf(" ");
      }
    }
    printf("]");
    if (current->next != NULL) {
      printf(" ");
    }
    current = current->next;
  }
  printf(")");
}
