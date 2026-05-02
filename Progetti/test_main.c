// test_main.c — test rapido per stack e tensori TensorForth

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "stack.h"
#include "tensor.h"

// ---- helpers di stampa ----

static void print_sep(const char *titolo) {
    printf("\n=== %s ===\n", titolo);
}

static void print_tensor(tensor_t *t) {
    printf("Tensor shape=[");
    for (int i = 0; i < t->ndim; i++)
        printf("%d%s", t->shape[i], i < t->ndim - 1 ? "," : "");
    printf("] data=[");
    int n = 1;
    for (int i = 0; i < t->ndim; i++) n *= t->shape[i];
    for (int i = 0; i < n; i++)
        printf("%.2f%s", t->data[i], i < n - 1 ? ", " : "");
    printf("]\n");
}

// ---- test stack con tensori ----

static void test_push_pop_tensor(tf_stack_t *stack) {
    print_sep("PUSH/POP TENSOR");

    int shape1[] = {3};
    tensor_t *t1 = tensor_alloc(shape1, 1);
    t1->data[0] = 1.0f; t1->data[1] = 2.0f; t1->data[2] = 3.0f;

    int shape2[] = {2};
    tensor_t *t2 = tensor_alloc(shape2, 1);
    t2->data[0] = 9.0f; t2->data[1] = 8.0f;

    stack_push_tensor(stack, t1);
    stack_push_tensor(stack, t2);
    printf("Stack dopo 2 push tensori — top atteso t2\n");

    // peek senza consumare
    stack_item_t *top = stack_peek(stack);
    printf("Peek tipo: %s\n", top->type == STACK_TENSOR ? "TENSOR ✅" : "STRING ❌");

    tensor_t *popped2 = stack_pop_tensor(stack);
    printf("Pop t2: "); print_tensor(popped2);
    tensor_decref(popped2);  // rilascio responsabilità chiamante

    tensor_t *popped1 = stack_pop_tensor(stack);
    printf("Pop t1: "); print_tensor(popped1);
    tensor_decref(popped1);

    printf("Stack vuoto: %s\n", stack_is_empty(stack) ? "sì ✅" : "no ❌");

    // rilascio riferimenti iniziali (push aveva fatto incref)
    tensor_decref(t1);
    tensor_decref(t2);
}

// ---- test stack con stringhe ----

static void test_push_pop_string(tf_stack_t *stack) {
    print_sep("PUSH/POP STRING");

    stack_push_string(stack, "input.pgm");
    stack_push_string(stack, "output.pgm");

    stack_item_t *top = stack_peek(stack);
    printf("Peek tipo: %s\n", top->type == STACK_STRING ? "STRING ✅" : "TENSOR ❌");

    char *s2 = stack_pop_string(stack);
    printf("Pop: %s\n", s2);
    free(s2);  // responsabilità chiamante

    char *s1 = stack_pop_string(stack);
    printf("Pop: %s\n", s1);
    free(s1);

    printf("Stack vuoto: %s\n", stack_is_empty(stack) ? "sì ✅" : "no ❌");
}

// ---- test overflow ----

static void test_tipo_errato(tf_stack_t *stack) {
    print_sep("TIPO ERRATO (deve stampare errore e uscire)");
    stack_push_string(stack, "ciao");
    // tentiamo pop_tensor su una stringa → deve dare errore
    tensor_t *t = stack_pop_tensor(stack);  // ❌ tipo sbagliato
    (void)t;
}

// ---- main ----

int main(int argc, char *argv[]) {
    tf_stack_t stack;
    stack_init(&stack);

    test_push_pop_tensor(&stack);
    test_push_pop_string(&stack);

    // Decommentare per testare la gestione errore tipo:
    // test_tipo_errato(&stack);

    printf("\n✅ Tutti i test superati.\n");
    return 0;
}