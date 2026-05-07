/*
 * Nome, Cognome, Matricola
 * ops_utils.c
 * Implementazione delle funzioni di supporto per gli operatori di TensorForth.
 */

#include "ops_utils.h"
#include <stdio.h>
#include <stdlib.h>

/* ---------- Controlli sullo stack ---------- */

/*
 * Controlla che lo stack abbia almeno n elementi.
 * Fa exit(1) se la condizione non è soddisfatta.
 */
void check_stack_size(tf_stack_t *stack, int n, const char *op_name) {
    if (stack->top < n - 1) {
        fprintf(stderr, "Errore [%s]: stack insufficiente (top=%d, richiesti=%d)\n",op_name, stack->top, n);
        exit(1);
    }
}

/*
 * Controlla che il top dello stack sia STACK_TENSOR.
 * Fa exit(1) se il tipo è sbagliato o lo stack è vuoto.
 */
void check_top_is_tensor(tf_stack_t *stack, const char *op_name) {
    check_stack_size(stack, 1, op_name);
    if (stack->items[stack->top].type != STACK_TENSOR) {
        fprintf(stderr, "Errore [%s]: il top dello stack non è un tensore\n", op_name);
        exit(1);
    }
}

/*
 * Controlla che i due elementi in cima allo stack siano entrambi STACK_TENSOR.
 * Fa exit(1) se uno dei due ha tipo sbagliato.
 */
void check_top2_are_tensors(tf_stack_t *stack, const char *op_name) {
    check_stack_size(stack, 2, op_name);
    if (stack->items[stack->top].type != STACK_TENSOR ||
        stack->items[stack->top - 1].type != STACK_TENSOR) {
        fprintf(stderr, "Errore [%s]: i due elementi in cima non sono tensori\n", op_name);
        exit(1);
    }
}

/*
 * Controlla che il top dello stack sia STACK_STRING.
 * Fa exit(1) se il tipo è sbagliato o lo stack è vuoto.
 */
void check_top_is_string(tf_stack_t *stack, const char *op_name) {
    check_stack_size(stack, 1, op_name);
    if (stack->items[stack->top].type != STACK_STRING) {
        fprintf(stderr, "Errore [%s]: il top dello stack non è una stringa\n", op_name);
        exit(1);
    }
}

/* ---------- Lettura senza pop (peek) ---------- */

/*
 * Restituisce il tensore in cima allo stack senza fare pop.
 * Fa exit(1) se lo stack è vuoto o il tipo non è STACK_TENSOR.
 */
tensor_t *peek_tensor(tf_stack_t *stack, const char *op_name) {
    check_top_is_tensor(stack, op_name);
    return stack->items[stack->top].value.tensor;
}

/* ---------- Controlli sulla forma dei tensori ---------- */

/*
 * Controlla che a e b abbiano stesso ndim e stessa shape.
 * Fa exit(1) se non compatibili.
 */
void check_same_shape(tensor_t *a, tensor_t *b, const char *op_name) {
    if (a->ndim != b->ndim) {
        fprintf(stderr, "Errore [%s]: ndim diverso (%d vs %d)\n",
                op_name, a->ndim, b->ndim);
        exit(1);
    }
    for (int d = 0; d < a->ndim; d++) {
        if (a->shape[d] != b->shape[d]) {
            fprintf(stderr, "Errore [%s]: shape diversa su dim %d (%d vs %d)\n",
                    op_name, d, a->shape[d], b->shape[d]);
            exit(1);
        }
    }
}

/*
 * Controlla che il tensore t sia 1D.
 * Fa exit(1) se non lo è.
 */
void check_is_1d(tensor_t *t, const char *op_name) {
    if (t->ndim != 1) {
        fprintf(stderr, "Errore [%s]: tensore deve essere 1D (ndim=%d)\n",
                op_name, t->ndim);
        exit(1);
    }
}

/*
 * Controlla che il tensore t sia 2D.
 * Fa exit(1) se non lo è.
 */
void check_is_2d(tensor_t *t, const char *op_name) {
    if (t->ndim != 2) {
        fprintf(stderr, "Errore [%s]: tensore deve essere 2D (ndim=%d)\n",
                op_name, t->ndim);
        exit(1);
    }
}

/* ---------- Pattern pop-decref-push ---------- */

/*
 * Pattern finale per operatori binari:
 * pop × 2, decref degli input, push del risultato, decref locale del risultato.
 */
void replace_top2_with_result(tf_stack_t *stack,
                               tensor_t *a, tensor_t *b,
                               tensor_t *result) {
    stack_pop_tensor(stack);  // rimuove a (cima)
    stack_pop_tensor(stack);  // rimuove b (sotto)
    tensor_decref(a);
    tensor_decref(b);
    stack_push_tensor(stack, result);  // incref interno
    tensor_decref(result);             // bilancia la referenza locale
}

/*
 * Pattern finale per operatori unari:
 * pop × 1, decref dell'input, push del risultato, decref locale del risultato.
 */
void replace_top1_with_result(tf_stack_t *stack,
                               tensor_t *a,
                               tensor_t *result) {
    stack_pop_tensor(stack);           // rimuove a
    tensor_decref(a);
    stack_push_tensor(stack, result);  // incref interno
    tensor_decref(result);             // bilancia la referenza locale
}