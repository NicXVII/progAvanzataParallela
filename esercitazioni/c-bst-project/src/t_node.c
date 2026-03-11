#include <stdio.h>
#include <stdlib.h>
#include "t_node.h"

t_node make_t_node(void) {
    t_node new_node = (t_node)malloc(sizeof(struct _t_node));
    if (new_node == NULL) {
        perror("make_t_node - node creation failed.");
        return NULL;
    }
    new_node->left = NULL;
    new_node->right = NULL;
    new_node->parent = NULL;
    new_node->value = NULL; // Assuming value is a pointer that needs to be initialized
    return new_node;
}