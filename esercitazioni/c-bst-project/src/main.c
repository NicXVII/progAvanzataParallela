#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "bst.h"
#include "t_node.h"

void s_test(int n);
void r_test(int n);

int main(int argc, char *argv[]) {
    int n;
    srand((int)time(NULL));
    printf("Numero di nodi: ");
    while (!scanf("%d", &n));
    s_test(n);
    r_test(n);
    return 0;
}