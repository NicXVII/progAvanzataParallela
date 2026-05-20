/*
 * NICOLAS DIMINICH SM3201661
 * ops_utils.h
 * Funzioni di supporto per gli operatori di TensorForth.
 * Centralizza controlli su stack, tipo e forma dei tensori.
 */

#ifndef OPS_UTILS_H
#define OPS_UTILS_H

#include "stack.h"
#include "tensor.h"

/* ---------- Controlli sullo stack ---------- */

/*
 * Controlla che lo stack abbia almeno n elementi.
 * op_name: nome dell'operatore chiamante, usato nel messaggio di errore.
 * Fa exit(1) se la condizione non è soddisfatta.
 */
void check_stack_size(tf_stack_t *stack, int n, const char *op_name);

/*
 * Controlla che il top dello stack sia STACK_TENSOR.
 * Fa exit(1) se il tipo è sbagliato o lo stack è vuoto.
 */
void check_top_is_tensor(tf_stack_t *stack, const char *op_name);

/*
 * Controlla che i due elementi in cima allo stack siano entrambi STACK_TENSOR.
 * Fa exit(1) se uno dei due ha tipo sbagliato o lo stack ha meno di 2 elementi.
 */
void check_top2_are_tensors(tf_stack_t *stack, const char *op_name);

/*
 * Controlla che il top dello stack sia STACK_STRING.
 * Fa exit(1) se il tipo è sbagliato o lo stack è vuoto.
 */
void check_top_is_string(tf_stack_t *stack, const char *op_name);

/* ---------- Lettura senza pop (peek) ---------- */

/*
 * Restituisce il tensore in cima allo stack senza fare pop.
 * Fa exit(1) se lo stack è vuoto o il tipo non è STACK_TENSOR.
 */
tensor_t *peek_tensor(tf_stack_t *stack, const char *op_name);

/* ---------- Controlli sulla forma dei tensori ---------- */

/*
 * Controlla che a e b abbiano stesso ndim e stessa shape su ogni dimensione.
 * Fa exit(1) se non compatibili.
 * Attenzione: a e b devono essere già stati estratti dallo stack.
 */
void check_same_shape(tensor_t *a, tensor_t *b, const char *op_name);

/*
 * Controlla che il tensore t sia 1D (ndim == 1).
 * Fa exit(1) se non lo è.
 */
void check_is_1d(tensor_t *t, const char *op_name);

/*
 * Controlla che il tensore t sia 2D (ndim == 2).
 * Fa exit(1) se non lo è.
 */
void check_is_2d(tensor_t *t, const char *op_name);

/* ---------- Pattern pop-decref-push ---------- */

/*
 * Pattern finale comune a tutti gli operatori binari:
 *   - pop di due elementi dallo stack
 *   - decref di a e b (gli input già letti prima del pop)
 *   - push del risultato con bilanciamento refcount
 *
 * Precondizione: a e b sono i puntatori letti da stack->items[top]
 *               e stack->items[top-1] prima di chiamare questa funzione.
 */
void replace_top2_with_result(tf_stack_t *stack,
                               tensor_t *a, tensor_t *b,
                               tensor_t *result);

/*
 * Pattern finale per operatori unari:
 *   - pop di un elemento dallo stack
 *   - decref di a
 *   - push del risultato con bilanciamento refcount
 */
void replace_top1_with_result(tf_stack_t *stack,
                               tensor_t *a,
                               tensor_t *result);

#endif /* OPS_UTILS_H */