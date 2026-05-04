//NICOLAS DIMINICH SM3201661
#include "ops.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>
#include <omp.h>



void execute_op(const char *op, tf_stack_t *stack) {
    if      (strcmp(op, "+")  == 0) op_add(stack);
    else if (strcmp(op, "-")  == 0) op_sub(stack);
    else if (strcmp(op, "*")  == 0) op_mul(stack);
    else if (strcmp(op, "<")  == 0) op_lt(stack);
    else if (strcmp(op, ">")  == 0) op_gt(stack);
    else if (strcmp(op, "=")  == 0) op_eq(stack);
    else if (strcmp(op, "&")  == 0) op_and(stack);
    else if (strcmp(op, "|")  == 0) op_or(stack);
    else if (strcmp(op, "!")  == 0) op_not(stack);
    else if (strcmp(op, "?")  == 0) op_select(stack);
    else if (strcmp(op, "@")  == 0) op_matmul(stack);
    else if (strcmp(op, ".")  == 0) op_dot(stack);
    else if (strcmp(op, "c")  == 0) op_conv(stack);
    else if (strcmp(op, "R")  == 0) op_relu(stack);
    else if (strcmp(op, "m")  == 0) op_min(stack);
    else if (strcmp(op, "M")  == 0) op_max(stack);
    else if (strcmp(op, "S")  == 0) op_sum(stack);
    else if (strcmp(op, "f")  == 0) op_fill(stack);
    else if (strcmp(op, "r")  == 0) op_reshape(stack);
    else if (strcmp(op, "v")  == 0) op_ravel(stack);
    else if (strcmp(op, "q")  == 0) op_shape(stack);
    else if (strcmp(op, "~")  == 0) op_rand(stack);
    else if (strcmp(op, "d")  == 0) op_dup(stack);
    else if (strcmp(op, "s")  == 0) op_swap(stack);
    else if (strcmp(op, "o")  == 0) op_over(stack);
    else if (strcmp(op, "D")  == 0) op_drop(stack);
    else if (strcmp(op, "p")  == 0) op_print(stack);
    else if (strcmp(op, "<r") == 0) op_read_pgm(stack);
    else if (strcmp(op, ">w") == 0) op_write_pgm(stack);
    else if (strcmp(op, "[")  == 0) op_read_bin(stack);
    else if (strcmp(op, "]")  == 0) op_write_bin(stack);
    else {
        fprintf(stderr, "Errore: operatore sconosciuto '%s'\n", op);
        exit(1);
    }
}


//operazioni aritmetiche
void op_add(tf_stack_t *stack) {
    // Implementazione dell'operazione di addizione
}

void op_sub(tf_stack_t *stack) {
    // Implementazione dell'operazione di sottrazione
}

void op_mul(tf_stack_t *stack) {
    // Implementazione dell'operazione di moltiplicazione
}


//Operazioni di comparazione
void op_lt(tf_stack_t *stack) {
    // Implementazione dell'operazione di confronto "less than"
}

void op_gt(tf_stack_t *stack) {
    // Implementazione dell'operazione di confronto "greater than"
}

void op_eq(tf_stack_t *stack) {
    // Implementazione dell'operazione di confronto "equal to"
}

//Operazioni logiche

void op_and(tf_stack_t *stack) {
    // Implementazione dell'operazione logica AND
}

void op_or(tf_stack_t *stack) {
    // Implementazione dell'operazione logica OR
}

void op_not(tf_stack_t *stack) {
    // Implementazione dell'operazione logica NOT
}

//operazioni di selezione


