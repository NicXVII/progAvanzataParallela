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
// Se il compilatore definisce _OPENMP, allora <omp.h> esiste
#ifdef _OPENMP
    #include <omp.h>
    #define OMP_PARALLEL_FOR _Pragma("omp parallel for")
#else
    // Fallback: nessun OpenMP, loop normali
    #define OMP_PARALLEL_FOR /* no-op */
#endif


void execute_op(const char *op, tf_stack_t *stack) {
    if (op == NULL || op[0] == '\0') {
        fprintf(stderr, "Errore: operatore sconosciuto '%s'\n", op);
        exit(1);
    }

    switch (op[0]) {
        // case '+': op_add(stack); break;
        // case '-': op_sub(stack); break;
        // case '*': op_mul(stack); break;
        // case '<':
        //     if (op[1] == '\0') {
        //         op_lt(stack);
        //     } else if (op[1] == 'r' && op[2] == '\0') {
        //         op_read_pgm(stack);
        //     } else {
        //         fprintf(stderr, "Errore: operatore sconosciuto '%s'\n", op);
        //         exit(1);
        //     }
        //     break;
        // case '>':
        //     if (op[1] == '\0') {
        //         op_gt(stack);
        //     } else if (op[1] == 'w' && op[2] == '\0') {
        //         op_write_pgm(stack);
        //     } else {
        //         fprintf(stderr, "Errore: operatore sconosciuto '%s'\n", op);
        //         exit(1);
        //     }
        //     break;
        // case '=': op_eq(stack); break;
        // case '&': op_and(stack); break;
        // case '|': op_or(stack); break;
        // case '!': op_not(stack); break;
        // case '?': op_select(stack); break;
        // case '@': op_matmul(stack); break;
        // case '.': op_dot(stack); break;
        // case 'c': op_conv(stack); break;
        // case 'R': op_relu(stack); break;
        // case 'm': op_min(stack); break;
        // case 'M': op_max(stack); break;
        // case 'S': op_sum(stack); break;
        // case 'f': op_fill(stack); break;
        // case 'r': op_reshape(stack); break;
        // case 'v': op_ravel(stack); break;
        // case 'q': op_shape(stack); break;
        // case '~': op_rand(stack); break;
        // case 'd': op_dup(stack); break;
        // case 's': op_swap(stack); break;
        // case 'o': op_over(stack); break;
        // case 'D': op_drop(stack); break;
        // case 'p': op_print(stack); break;
        // case '[': if (op[1] == '\0') op_read_bin(stack); else { fprintf(stderr, "Errore: operatore sconosciuto '%s'\n", op); exit(1); } break;
        // case ']': if (op[1] == '\0') op_write_bin(stack); else { fprintf(stderr, "Errore: operatore sconosciuto '%s'\n", op); exit(1); } break;
        default:
            fprintf(stderr, "Errore: operatore sconosciuto '%s'\n", op);
            exit(1);
    }
}


//operazioni aritmetiche
void op_add(tf_stack_t *stack) {
    // Implementazione dell'operazione di addizione
    if(stack->top < 1) {
        fprintf(stderr, "Errore: stack sotto dimensionato per addizione\n");
        exit(1);
    }
    stack_item_t *item_a = &stack->items[stack->top];
    stack_item_t *item_b = &stack->items[stack->top - 1];
    if (item_a->type != STACK_TENSOR || item_b->type != STACK_TENSOR) {
        fprintf(stderr, "Errore: addizione richiede due tensori\n");
        exit(1);
    }
    tensor_t *a = item_a->value.tensor;
    tensor_t *b = item_b->value.tensor;
    // Verifica che a e b abbiano la stessa forma
    if ( a->ndim != b->ndim) {
        fprintf(stderr, "Errore: i tensori devono avere lo stesso numero di elementi\n");
        exit(1);
    }
    tensor_t *result = tensor_alloc(a->shape, a->ndim);
    for (int i = 0; i < tensor_numel(a); i++) {
        result->data[i] = a->data[i] + b->data[i];
    }
    // Rimuovi i due tensori originali dallo stack
    



    stack_pop_tensor(a);
    stack_pop_tensor(b);
    // Metti il risultato in cima allo stack
    stack_push_tensor(stack, result);



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
void op_select(tf_stack_t *stack ) {
    // Implementazione dell'operazione di selezione
}

//Si assume that m is a tensor composed only of 0.0 and 1.0: if the value is 1.0
//the corresponding element from a is taken, otherwise from b. This allows to
//select elements between the two tensors based on the mask tensor m. Clearly
//all tensors must have the same dimensions.



//Operazioni specifiche per tensori
 void op_matmul(tf_stack_t *stack) {
    // Implementazione dell'operazione di moltiplicazione matriciale
}

void op_dot(tf_stack_t *stack) {
    // Implementazione dell'operazione di prodotto scalare
}

void op_conv(tf_stack_t *stack) {
    // Implementazione dell'operazione di convoluzione
}


//Operazioni sulla forma dei tensori
tensor_t * op_reshape(tf_stack_t *stack) {
    // Implementazione dell'operazione di reshape
}

tensor_t * op_ravel(tf_stack_t *stack) {
    // Implementazione dell'operazione di ravel
    //torna versione 1D del tensore
}

int * op_shape(tf_stack_t *stack) {
    // Implementazione dell'operazione di shape
    //torna un vettore con la dimesione di a
}


//Operazioni di generazione di numeri casuali
void op_rand(tf_stack_t *stack) {
    // Implementazione dell'operazione di generazione di numeri casuali
    //torna un tensore con valori casuali tra 0,1
}


//Operazioni elemento per elemento


void op_relu(tf_stack_t *stack) {
    // Implementazione dell'operazione di ReLU
}

void op_min(tf_stack_t *stack) {
    // Implementazione dell'operazione di minimo elemento per elemento     ritorna elemento perelemento il minimo tra a e b

}

void op_max(tf_stack_t *stack) {
    // Implementazione dell'operazione di massimo elemento per elemento     ritorna elemento perelemento il massimo tra a e b

}

//Per m e M i tensori devono essere delle stessa dimensione.


//Operazioni di riduzione
void op_sum(tf_stack_t *stack) {
    // Implementazione dell'operazione di somma di tutti gli elementi del tensore
    //ritorna un tensore 0D con la somma di tutti gli elementi di a
}

//Operazioni di filling di tensori
void op_fill(tf_stack_t *stack) {
    // Implementazione dell'operazione di filling di un tensore con un valore specifico
    //riempie un tensore con un valore specifico
}



//Operazioni di utilità
void op_print(tf_stack_t *stack) {
    //stampa il contenuto del tensore
    if(stack->top < 0) {
        fprintf(stderr, "Errore: stack vuoto\n");
        exit(1);
    }
    tensor_t *t = stack->items[stack->top].value.tensor;
    printf("Tensor (ndim=%d, shape=[", t->ndim);
    for (int i = 0; i < t->ndim; i++) {
        printf("%d", t->shape[i]);
        if (i < t->ndim - 1) printf(", ");
    }
    printf("])\n");
}


//Operazioni di manipolazione dello stack

void op_dup(tf_stack_t *stack) {
    // Duplica l'elemento in cima allo stack
}
void op_swap(tf_stack_t *stack) {
    // Scambia i due elementi in cima allo stack
}

void op_over(tf_stack_t *stack) {
    // Copia il secondo elemento dallo stack e lo mette in cima
}

void op_drop(tf_stack_t *stack) {
    // Rimuove l'elemento in cima allo stack
}



//I/O operations
void op_read_pgm(tf_stack_t *stack) {
    // Implementazione dell'operazione di lettura di un'immagine PGM
}

void op_write_pgm(tf_stack_t *stack) {
    // Implementazione dell'operazione di scrittura di un'immagine PGM
}

void op_read_bin(tf_stack_t *stack) {
    // Implementazione dell'operazione di lettura di un file binario
}

void op_save_bin(tf_stack_t *stack) {
    // salva tensore in file name
}



