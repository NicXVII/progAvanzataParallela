#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "bst.h"
#include "t_node.h"

void s_test(int n);
void r_test(int n);

void s_test(int n) {
    int i = 0;
    bst t = make_bst();
    printf("Aggiunta dei nodi\n");
    for (; i < n; i++) {
        t_node node = make_t_node();
        node->key = i;
        bst_insert(t, node);
    }
    printf("L'albero e' profondo %d\n", bst_depth(t));
    delete_bst(t);
}

void r_test(int n) {
    int i = 0;
    bst t = make_bst();
    printf("Aggiunta dei nodi\n");
    for (; i < n; i++) {
        t_node node = make_t_node();
        node->key = rand();
        bst_insert(t, node);
    }
    printf("L'albero e' profondo %d\n", bst_depth(t));
    delete_bst(t);
}

int main(int argc, char *argv[]) {
    int n;
    (void)argc;
    (void)argv;
    srand((int)time(NULL));
    printf("Numero di nodi: ");
    while (!scanf("%d", &n));
    s_test(n);
    r_test(n);
    return 0;
}