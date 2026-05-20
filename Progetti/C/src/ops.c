//NICOLAS DIMINICH SM3201661
#include "ops.h"
#include "ops_utils.h"
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
    check_stack_size(stack, 2, "+");
    check_top2_are_tensors(stack, "+");
    tensor_t *a = stack->items[stack->top].value.tensor;
    tensor_t *b = stack->items[stack->top - 1].value.tensor;
    check_same_shape(a, b, "+");

    tensor_t *result = tensor_alloc(a->shape, a->ndim);
    int n = tensor_numel(a);
    #pragma omp parallel for
    for (int i = 0; i < n; i++)
        result->data[i] = a->data[i] + b->data[i];

    replace_top2_with_result(stack, a, b, result);
}

void op_sub(tf_stack_t *stack) {
    check_stack_size(stack, 2, "-");
    check_top2_are_tensors(stack, "-");
    tensor_t *a = stack->items[stack->top].value.tensor;      // cima
    tensor_t *b = stack->items[stack->top - 1].value.tensor;  // sotto
    check_same_shape(a, b, "-");

    tensor_t *result = tensor_alloc(a->shape, a->ndim);
    int n = tensor_numel(a);
    #pragma omp parallel for
    for (int i = 0; i < n; i++)
        result->data[i] = a->data[i] - b->data[i]; // b a -- a-b

    replace_top2_with_result(stack, a, b, result);
}

void op_mul(tf_stack_t *stack) {
    check_stack_size(stack, 2, "*");
    check_top2_are_tensors(stack, "*");
    tensor_t *a = stack->items[stack->top].value.tensor;
    tensor_t *b = stack->items[stack->top - 1].value.tensor;
    check_same_shape(a, b, "*");

    tensor_t *result = tensor_alloc(a->shape, a->ndim);
    int n = tensor_numel(a);
    #pragma omp parallel for
    for (int i = 0; i < n; i++)
        result->data[i] = a->data[i] * b->data[i];

    replace_top2_with_result(stack, a, b, result);
}


//Operazioni di comparazione
void op_lt(tf_stack_t *stack) {
    // Implementazione dell'operazione di confronto "less than"
    check_stack_size(stack, 2, "<");
    check_top2_are_tensors(stack, "<");
    tensor_t *a = stack->items[stack->top].value.tensor;
    tensor_t *b = stack->items[stack->top - 1].value.tensor;
    check_same_shape(a, b, "<");

    tensor_t *result = tensor_alloc(a->shape, a->ndim);
    int n = tensor_numel(a);
    #pragma omp parallel for
    for (int i = 0; i < n; i++)
        result->data[i] = (a->data[i] < b->data[i]) * 1.0f; // se a[i] < b[i] allora 1.0 altrimenti 0.0

    replace_top2_with_result(stack, a, b, result);

}

void op_gt(tf_stack_t *stack) {
    // Implementazione dell'operazione di confronto "greater than"
    check_stack_size(stack, 2, ">");
    check_top2_are_tensors(stack, ">");
    tensor_t *a = stack->items[stack->top].value.tensor;
    tensor_t *b = stack->items[stack->top - 1].value.tensor;
    check_same_shape(a, b, ">");

    tensor_t *result = tensor_alloc(a->shape, a->ndim);
    int n = tensor_numel(a);
    #pragma omp parallel for
    for (int i = 0; i < n; i++)
        result->data[i] = (a->data[i] > b->data[i]) * 1.0f; // se a[i] > b[i] allora 1.0 altrimenti 0.0

    replace_top2_with_result(stack, a, b, result);
}

void op_eq(tf_stack_t *stack) {
    // Implementazione dell'operazione di confronto "equal to"
    check_stack_size(stack, 2, "==");
    check_top2_are_tensors(stack, "==");
    tensor_t *a = stack->items[stack->top].value.tensor;
    tensor_t *b = stack->items[stack->top - 1].value.tensor;
    check_same_shape(a, b, "==");

    tensor_t *result = tensor_alloc(a->shape, a->ndim);
    int n = tensor_numel(a);
    #pragma omp parallel for
    for (int i = 0; i < n; i++)
        result->data[i] = (a->data[i] == b->data[i]) * 1.0f; // se a[i] ==b[i] allora 1.0 altrimenti 0.0

    replace_top2_with_result(stack, a, b, result);
}

//Operazioni logiche

void op_and(tf_stack_t *stack) {
    // Implementazione dell'operazione logica AND
    check_stack_size(stack, 2, "&");
    check_top2_are_tensors(stack, "&");
    tensor_t *a = stack->items[stack->top].value.tensor;
    tensor_t *b = stack->items[stack->top - 1].value.tensor;
    check_same_shape(a, b, "&");

    tensor_t *result = tensor_alloc(a->shape, a->ndim);
    int n = tensor_numel(a);

    #pragma omp parallel for
    for(int i = 0; i < n; i++){
        if ((a->data[i] != 0.0f && a->data[i] != 1.0f) || (b->data[i] != 0.0f && b->data[i] != 1.0f)) {
            fprintf(stderr, "Errore [&]: tensore logico deve contenere solo 0.0 e 1.0\n");
            exit(1);
        }
        result->data[i] = ((a->data[i] != 0.0f) & (b->data[i] != 0.0f)) * 1.0f;

    }

    replace_top2_with_result(stack, a, b, result);

}

void op_or(tf_stack_t *stack) {
    // Implementazione dell'operazione logica OR
    check_stack_size(stack, 2, "\|");
    check_top2_are_tensors(stack, "\|");
    tensor_t *a = stack->items[stack->top].value.tensor;
    tensor_t *b = stack->items[stack->top - 1].value.tensor;
    check_same_shape(a, b, "\|");

    tensor_t *result = tensor_alloc(a->shape, a->ndim);
    int n = tensor_numel(a);

    #pragma omp parallel for
    for(int i = 0; i < n; i++){
        if ((a->data[i] != 0.0f && a->data[i] != 1.0f) || (b->data[i] != 0.0f && b->data[i] != 1.0f)) {
            fprintf(stderr, "Errore [|]: tensore logico deve contenere solo 0.0 e 1.0\n");
            exit(1);
        }
        result->data[i] = ((a->data[i] != 0.0f) || (b->data[i] != 0.0f)) * 1.0f;
    }

    replace_top2_with_result(stack, a, b, result);

}

void op_not(tf_stack_t *stack) {
    // Implementazione dell'operazione logica NOT
    check_stack_size(stack, 1, "!");
    check_top_is_tensor(stack, "!");
    tensor_t *a = stack->items[stack->top].value.tensor;

    tensor_t *result = tensor_alloc(a->shape, a->ndim);
    int n = tensor_numel(a);

    #pragma omp parallel for
    for (int i = 0; i < n; i++) {
        if (a->data[i] != 0.0f && a->data[i] != 1.0f) {
            fprintf(stderr, "Errore [!]: tensore logico deve contenere solo 0.0 e 1.0\n");
            exit(1);
        }
        result->data[i] = 1.0f - a->data[i];
    }
    replace_top1_with_result(stack, a, result);
}

//Si assume che il tensore m sia composto solo da 0.0 e 1.0

//operazioni di selezione
void op_select(tf_stack_t *stack ) {
    check_stack_size(stack, 3, "$");
    check_top2_are_tensors(stack, "$");
    if (stack->items[stack->top - 2].type != STACK_TENSOR) {
        fprintf(stderr, "Errore [%s]: il top dello stack non è un tensore\n", "$");
        exit(1);
    }
    tensor_t *m = stack->items[stack->top].value.tensor;      // cima
    tensor_t *a = stack->items[stack->top - 1].value.tensor;  // sotto
    tensor_t *b = stack->items[stack->top - 2].value.tensor;  // sotto sotto
    check_same_shape(a, b, "$");
    check_same_shape(a, m, "$");

    tensor_t *result = tensor_alloc(a->shape, a->ndim);
    int n = tensor_numel(a);
    #pragma omp parallel for
    for (int i = 0; i < n; i++) {
        if (m->data[i] != 0.0f && m->data[i] != 1.0f) {
            fprintf(stderr, "Errore [$]: tensore di maschera deve contenere solo 0.0 e 1.0\n");
            exit(1);
        }
        result->data[i] = m->data[i] * a->data[i] + (1.0f - m->data[i]) * b->data[i];  
        // se m == 0 a =0, b = b
        // se m == 1 a = a, b = 0  
    }

    // rimuove m, a, b
    stack_pop_tensor(stack);  // rimuove m (cima)
    stack_pop_tensor(stack);  // rimuove a (sotto)
    stack_pop_tensor(stack);  // rimuove b (sotto sotto)
    tensor_decref(m);
    tensor_decref(a);
    tensor_decref(b);
    stack_push_tensor(stack, result);  // push del risultato
    tensor_decref(result);  // decref locale del risultato
}



//Operazioni specifiche per tensori
 void op_matmul(tf_stack_t *stack) {
    // Implementazione dell'operazione di moltiplicazione matriciale
    check_stack_size(stack, 2, "@");
    check_top2_are_tensors(stack, "@");
    tensor_t *a = stack->items[stack->top].value.tensor;      
    tensor_t *b = stack->items[stack->top - 1].value.tensor;

    if (a->ndim != 2 || b->ndim != 2) { //devono essere 2D
        fprintf(stderr, "Errore [@]: entrambi i tensori devono essere 2D\n");
        exit(1);
    }

    int M = a->shape[0], K = a->shape[1];
    int K2 = b->shape[0], N = b->shape[1];

    // Dimensioni compatibili: colonne di a == righe di b
    if (K != K2) {
        fprintf(stderr, "Errore [@]: dimensioni incompatibili (%dx%d) @ (%dx%d)\n", M, K, K2, N);
        exit(1);
    }

    int shape[2] = {M, N};
    tensor_t *result = tensor_alloc(shape, 2);

    memset(result->data, 0, M * N * sizeof(float));

        #pragma omp parallel for
        for (int i = 0; i < M; i++) {
            for (int k = 0; k < K; k++) {
                float aik = a->data[i * K + k]; 
                for (int j = 0; j < N; j++) {
                    result->data[i * N + j] += aik * b->data[k * N + j];
                }
            }
        }
    replace_top2_with_result(stack, a, b, result);


}

void op_dot(tf_stack_t *stack) {
    // Implementazione dell'operazione di prodotto scalare
    check_stack_size(stack, 2, ".");
    check_top2_are_tensors(stack, ".");
    tensor_t *a = stack->items[stack->top].value.tensor;      
    tensor_t *b = stack->items[stack->top - 1].value.tensor;

    check_same_shape(a, b, ".");
    
    int shape[0] = {1}; // tensore 0D
    tensor_t *result = tensor_alloc(shape, 1); 
    result->data[0] = 0.0f;
    int n = tensor_numel(a);

    float sum = 0.0f;
    #pragma omp parallel for reduction(+:sum)
    for (int i = 0; i < n; i++)
        sum += a->data[i] * b->data[i];
    result->data[0] = sum;


    replace_top2_with_result(stack, a, b, result);



}

void op_conv(tf_stack_t *stack) {
    check_stack_size(stack, 2, "c");
    check_top2_are_tensors(stack, "c");
    tensor_t *kernel = stack->items[stack->top].value.tensor;
    tensor_t *a = stack->items[stack->top - 1].value.tensor;

    if (a->ndim != 2 || kernel->ndim != 2) {
        fprintf(stderr, "Errore [c]: entrambi i tensori devono essere 2D\n");
        exit(1);
    }

    int H = a->shape[0], W = a->shape[1];
    int KH = kernel->shape[0], KW = kernel->shape[1];

    int shape[2] = {H, W};                             
    tensor_t *result = tensor_alloc(shape, 2);
    memset(result->data, 0, H * W * sizeof(float));    //inizializzo con zero, non viene fatto con la malloc

    #pragma omp parallel for
    for (int i = 0; i < H; i++) {
        for (int j = 0; j < W; j++) { //scorro prima sulla matrice a, poi su kernel
            float sum = 0.0f;
            for (int ki = 0; ki < KH; ki++) {
                for (int kj = 0; kj < KW; kj++) {
                    int ai = i + ki - KH/2;
                    int aj = j + kj - KW/2;

                    int valid = (ai >= 0) & (ai < H) & (aj >= 0) & (aj < W); //forzo con &  a fare operazione bit a bit invece del salto condizionale
                    int ai_clamped = ai * valid;
                    int aj_clamped = aj * valid;

                    sum += valid * a->data[ai_clamped * W + aj_clamped]
                                 * kernel->data[ki * KW + kj];
                }
            }
            result->data[i * W + j] = sum;
            
            
        }
    }

    replace_top2_with_result(stack, a, kernel, result);

}


//Operazioni sulla forma dei tensori
void op_reshape(tf_stack_t *stack) {
    check_stack_size(stack, 2, "r");
    check_top2_are_tensors(stack, "r");
    tensor_t *s = stack->items[stack->top].value.tensor;      // s in cima
    tensor_t *a = stack->items[stack->top - 1].value.tensor;  // a sotto

    // s deve essere 1D con 1 o 2 elementi
    if (s->ndim != 1 || s->shape[0] < 1 || s->shape[0] > MAX_DIM) {
        fprintf(stderr, "Errore [r]: s non valido\n");
        exit(1);
    }

    // verifica compatibilità PRIMA di fare qualsiasi cosa
    int total_s = 1;
    for (int i = 0; i < s->shape[0]; i++)
        total_s *= (int)s->data[i]; //trovo numero totale di elementi nel nuovo shape

    if (tensor_numel(a) != total_s) {
        fprintf(stderr, "Errore [r]: dimensioni incompatibili\n");
        exit(1);
    }

    // aggiorna solo i metadati di a, data rimane invariato
    a->ndim = s->shape[0];
    for (int i = 0; i < a->ndim; i++)
        a->shape[i] = (int)s->data[i];

    stack_pop_tensor(stack);  // rimuove s (cima)
    tensor_decref(s);

}
void op_ravel(tf_stack_t *stack) {
    // Implementazione dell'operazione di ravel
    //torna versione 1D del tensore
    check_stack_size(stack, 1, "r");
    check_top_is_tensor(stack, "r");
    tensor_t *a = stack->items[stack->top].value.tensor;

    a-> shape[0] = tensor_numel(a);
    a->ndim = 1;


}

void op_shape(tf_stack_t *stack) {
    // ritorna un vettore rappresentante le dimensioni di a
    check_stack_size(stack, 1, "#");
    check_top_is_tensor(stack, "#");
    tensor_t *a = stack->items[stack->top].value.tensor;
     // il risultato è un tensore 1D con ndim elementi
    
    int shape[1] = {a->ndim};
    tensor_t *result = tensor_alloc(shape, 1);

    for (int i = 0; i < a->ndim; i++)
        result->data[i] = (float)a->shape[i];

    stack_push_tensor(stack, result); 
}


//Operazioni di generazione di numeri casuali
void op_rand(tf_stack_t *stack) {
    // Implementazione dell'operazione di generazione di numeri casuali
    //torna un tensore con valori casuali tra 0,1

    check_stack_size(stack, 1, "?");
    check_top_is_tensor(stack, "?");
    tensor_t *shape_tensor = stack->items[stack->top].value.tensor;

    if (shape_tensor->ndim != 1 || shape_tensor->shape[0] < 1 || shape_tensor->shape[0] > MAX_DIM) {
        fprintf(stderr, "Errore [?]: shape non valido\n");
        exit(1);
    }

    int total_size = 1;
    for (int i = 0; i < shape_tensor->shape[0]; i++)
        total_size *= (int)shape_tensor->data[i];

    int shape[1] = {total_size};
    tensor_t *result = tensor_alloc(shape, 1);

    for (int i = 0; i < total_size; i++)
        result->data[i] = (float)rand() / RAND_MAX;

    stack_push_tensor(stack, result);
    tensor_decref(result);  // decref locale del risultato
}


//Operazioni elemento per elemento


void op_relu(tf_stack_t *stack) {
    // Implementazione dell'operazione di ReLU
    //tutti gli elementi di anegativi sonosostituiti con 0(relu(x) = max(0, x))
    check_stack_size(stack, 1, "R");
    check_top_is_tensor(stack, "R");
    tensor_t *a = stack->items[stack->top].value.tensor;
    int n = tensor_numel(a);
    #pragma omp parallel for
    for (int i = 0; i < n; i++)
        a->data[i] = fmaxf(0.0f, a->data[i]);

    


}

void op_min(tf_stack_t *stack) {
    // Implementazione dell'operazione di minimo elemento per elemento     ritorna elemento perelemento il minimo tra a e b
    check_stack_size(stack, 2, "m");
    check_top2_are_tensors(stack, "m");
    tensor_t *a = stack->items[stack->top].value.tensor;
    tensor_t *b = stack->items[stack->top - 1].value.tensor;
    check_same_shape(a, b, "m");

    tensor_t *result = tensor_alloc(a->shape, a->ndim);
    int n = tensor_numel(a);
    #pragma omp parallel for
    for (int i = 0; i < n; i++)
        result->data[i] = fminf(a->data[i], b->data[i]);
    
    replace_top2_with_result(stack, a, b, result);


}

void op_max(tf_stack_t *stack) {
    // Implementazione dell'operazione di massimo elemento per elemento     ritorna elemento perelemento il massimo tra a e b
        // Implementazione dell'operazione di minimo elemento per elemento     ritorna elemento perelemento il minimo tra a e b
    check_stack_size(stack, 2, "M");
    check_top2_are_tensors(stack, "M");
    tensor_t *a = stack->items[stack->top].value.tensor;
    tensor_t *b = stack->items[stack->top - 1].value.tensor;
    check_same_shape(a, b, "M");
    
    tensor_t *result = tensor_alloc(a->shape, a->ndim);
    int n = tensor_numel(a);
    #pragma omp parallel for
    for (int i = 0; i < n; i++)
        result->data[i] = fmaxf(a->data[i], b->data[i]);
    
    replace_top2_with_result(stack, a, b, result);

}

//Per m e M i tensori devono essere delle stessa dimensione.


//Operazioni di riduzione
void op_sum(tf_stack_t *stack) {
    // Implementazione dell'operazione di somma di tutti gli elementi del tensore
    //ritorna un tensore 0D con la somma di tutti gli elementi di a
    check_stack_size(stack, 1, "S");
    check_top_is_tensor(stack, "S");
    tensor_t *a = stack->items[stack->top].value.tensor;
    int shape[1] = {1}; // tensore 1D di un elemento
    tensor_t *result = tensor_alloc(shape, 1);
    
    float sum = 0.0f;

    int n = tensor_numel(a);
    #pragma omp parallel for reduction(+:sum) // faccio la somma delle somme quando finiscono tutti i thread
    for (int i = 0; i < n; i++)
        sum += a->data[i];
    
    result->data[0] = sum;
    replace_top1_with_result(stack, a, result);
}   

//Operazioni di filling di tensori
void op_fill(tf_stack_t *stack) {
    check_stack_size(stack, 2, "f");
    check_top2_are_tensors(stack, "f");
    tensor_t *v = stack->items[stack->top].value.tensor;      // v in cima
    tensor_t *s = stack->items[stack->top - 1].value.tensor;  // s sotto

    if (s->ndim != 1 || s->shape[0] < 1 || s->shape[0] > MAX_DIM) {
        fprintf(stderr, "Errore [f]: s deve essere 1D con 1 o 2 elementi\n");
        exit(1);
    }
    if (v->ndim != 1) {
        fprintf(stderr, "Errore [f]: v deve essere un tensore 1D\n");
        exit(1);
    }

    int shape[MAX_DIM];
    for (int i = 0; i < s->shape[0]; i++)
        shape[i] = (int)s->data[i];                         

    tensor_t *result = tensor_alloc(shape, s->shape[0]);    
    int size_v = tensor_numel(v);

    #pragma omp parallel for
    for (int i = 0; i < tensor_numel(result); i++)
        result->data[i] = v->data[i % size_v];      // struttando il modulo rimango sempre dentro i limiti di v        
    replace_top2_with_result(stack, s, v, result);
}

//Operazioni di utilità
void op_print(tf_stack_t *stack) {
    check_stack_size(stack, 1, "P");
    check_top_is_tensor(stack, "P");

    tensor_t *a = stack->items[stack->top].value.tensor;

    printf("Tensor(shape=[");
    for (int i = 0; i < a->ndim; i++) {
        printf("%d", a->shape[i]);
        if (i < a->ndim - 1)
            printf(", ");
    
    }
    printf("], data=[");
    int n = tensor_numel(a);
    for (int i = 0; i < n; i++) {
        printf("%.4f", a->data[i]);
        if (i < n - 1)
            printf(", ");

    }
    printf("])");

    stack_pop_tensor(stack);
    tensor_decref(a);
}


//Operazioni di manipolazione dello stack

void op_dup(tf_stack_t *stack) {
    // Duplica l'elemento in cima allo stack
    check_stack_size(stack, 1, "d");
    stack_item_t item = stack->items[stack->top];

    if (item.type == STACK_TENSOR) {
        stack->top++;
        tensor_incref(item.value.tensor);
        stack->items[stack->top] = item;
    } else {
        stack_push_string(stack, item.value.string); //push string aumenta gia il top
    }
}
void op_swap(tf_stack_t *stack) {
    check_stack_size(stack, 2, "s");

    stack_item_t tmp = stack->items[stack->top]; //variabile temp che salva top
    stack->items[stack->top]     = stack->items[stack->top - 1];
    stack->items[stack->top - 1] = tmp;
}

void op_over(tf_stack_t *stack) {
    // Copia il secondo elemento dallo stack e lo mette in cima
    check_stack_size(stack, 2, "o");

    stack_item_t item = stack->items[stack->top - 1]; // secondo elemento dallo stack

    if (item.type == STACK_TENSOR){
        stack->top++;
        stack->items[stack->top] = item;
        tensor_incref(item.value.tensor);
    }else{
        stack_push_string(stack, item.value.string);
    }    
}

void op_drop(tf_stack_t *stack) {
    // Rimuove l'elemento in cima allo stack
    check_stack_size(stack, 1, "D");

    stack_item_t item = stack->items[stack->top];
    if (item.type == STACK_TENSOR) {
        stack_pop_tensor(stack);
        tensor_decref(item.value.tensor);
    } else {
        stack_pop_string(stack);
        free(item.value.string);
    }

}





//I/O operations


int tensor_from_pgm(const char *filename, tensor_t **out)
{
    FILE *fp = fopen(filename, "rb");
    if (!fp) {
        //fprintf(stderr, "Errore: impossibile aprire il file '%s'\n", filename);
        return 0;
    }


    char magic[3] = {0}; 

    if(fscanf(fp, "%2s", magic)  != 1 || strcmp(magic, "P5") != 0) {
        //fprintf(stderr, "Errore: formato PGM non valido (deve iniziare con 'P5')\n");
        fclose(fp);
        return 0;
    }

    int width, height;
    if (fscanf(fp, "%d %d", &width, &height) != 2) {
        // errore: non sono riuscito a leggere due interi
        fclose(fp);
        return 0;
    }

    int maxval;
    if (fscanf(fp, "%d", &maxval) != 1) {
        //fprintf(stderr, "Errore: impossibile leggere il valore massimo dei pixel\n");
        fclose(fp);
        return 0;
    }





    int sep = fgetc(fp);
    if (sep == EOF) {
        //fprintf(stderr, "Errore: file PGM troncato dopo l'header\n");
        fclose(fp);
        return 0;
    }

    tensor_t *img = tensor_alloc((int[]){height, width}, 2);


    for (int i = 0; i < height * width; i++) {
        int pixel = fgetc(fp);
        if (pixel == EOF) {
            //fprintf(stderr, "Errore: file troppo corto, aspettavo %d pixel ma ne ho letti solo %d\n", height * width, i);
            tensor_decref(img);
            fclose(fp);
            return 0;
        }
        img->data[i] = (float)pixel / maxval;
    }

    fclose(fp);
    *out = img;
    return 0;
}




void op_read_pgm(tf_stack_t *stack) {
    // Implementazione dell'operazione di lettura di un'immagine PGM
    check_stack_size(stack, 1, "(");
    check_top_is_string(stack, "(");

    char *filename = stack_pop_string(stack);

    if(filename == NULL || filename[0] == '\0') {
        fprintf(stderr, "Errore: nome file non valido '%s'\n", filename);
        free(filename);
        exit(1);
    }


    tensor_t *img;

    if (tensor_from_pgm(filename, &img) != 0) {
        fprintf(stderr, "Errore: impossibile leggere l'immagine '%s'\n", filename);
        free(filename);
        exit(1);
    }    
    stack_push_tensor(stack, img);
    tensor_decref(img); // decref locale del tensore appena creato
}





void op_write_pgm(tf_stack_t *stack) {
    // Implementazione dell'operazione di scrittura di un'immagine PGM
    check_stack_size(stack, 2, ")");
    check_top_is_string(stack, ")");
    stack_item_t img_item = stack->items[stack->top - 1];
    if (img_item.type != STACK_TENSOR) {
        fprintf(stderr, "Errore: il secondo elemento dello stack deve essere un tensore\n");
        exit(1);
    }

    char *filename = stack_pop_string(stack);
    tensor_t *img = img_item.value.tensor;

    if (filename == NULL || filename[0] == '\0') {
        fprintf(stderr, "Errore: nome file non valido '%s'\n", filename);
        free(filename);
        exit(1);
    }

    if (img->ndim != 2) {
        fprintf(stderr, "Errore: il tensore da salvare deve essere 2D\n");
        free(filename);
        exit(1);
    }

    if (tensor_from_pgm(filename, &img) != 0) {
        fprintf(stderr, "Errore: impossibile scrivere l'immagine '%s'\n", filename);
        free(filename);
        exit(1);
    }

    stack_pop_tensor(stack); // rimuove il tensore dopo averlo salvato
    free(filename);


}

void op_read_bin(tf_stack_t *stack) {
    // Implementazione dell'operazione di lettura di un file binario
    check_stack_size(stack, 1, "{");
    check_top_is_string(stack, "{");
    char *filename = stack_pop_string(stack);

    if (filename == NULL || filename[0] == '\0') {
        fprintf(stderr, "Errore: nome file non valido '%s'\n", filename);
        free(filename);
        exit(1);
    }

    tensor_t *tensor;
    if (tensor_from_bin_mmap(filename, &tensor) != 0) {
        fprintf(stderr, "Errore: impossibile leggere il file binario '%s'\n", filename);
        free(filename);
        exit(1);
    }

    //libero la memoria della stringa usata, pusho il tensore nello stack
    free(filename);
    stack_push_tensor(stack, tensor);
    tensor_decref(tensor);


}

int tensor_from_bin_mmap(const char *filename, tensor_t **out){
    int fd = open(filename, O_RDONLY);
    if (fd < 0) {
        return -1;
    }
    struct stat st; // prendo la struttura del file
    if (fstat(fd, &st) != 0) {
        close(fd);
        return -1;
    }

    off_t file_size = st.st_size;

    void *base = mmap(NULL, file_size, PROT_READ, MAP_PRIVATE, fd, 0); // mappo file in memoria
    if (base == MAP_FAILED) {
        close(fd);
        return -1;
    }

    close(fd);

    on_disk_tensor_t *hdr = (on_disk_tensor_t *)base; // prendo l'header del tensore

    if(hdr->ndim < 1 || hdr->ndim > MAX_DIM) { //check validita' ndim
        munmap(base, file_size);
        return -1;
    }

    int32_t ndim = hdr->ndim;
    int32_t shape[MAX_DIM];

    for(int i = 0; i < ndim; i++) {
        if(hdr->shape[i] <= 0) {
            munmap(base, file_size);
            return -1;
        }
        shape[i] = hdr->shape[i];
    }

    int64_t numel = 1;
    for(int i = 0; i < ndim; i++) {
        numel *= shape[i];
    }

    off_t data_offset = hdr->data_offset; 
    if(data_offset < (off_t)sizeof(on_disk_tensor_t)) {  // il data_offset deve essere almeno grande quanto l'header
        munmap(base, file_size);
        return -1;
    }
    if(data_offset + numel * sizeof(float) > file_size) { //la dimensione del offset + dimensione dei dati deve essere minore della dim totale del file
        munmap(base, file_size);
        return -1;
    }

    tensor_t *tensor = malloc(sizeof(tensor_t));

    if(!tensor) {
        munmap(base, file_size);
        return -1;
    }


    //inizializzo tensore con dati mappati
    tensor->ndim = ndim;
    memcpy(tensor->shape, shape, ndim * sizeof(int32_t));
    tensor->refcount = 1;
    tensor->is_mmap = true;
    tensor->mmap_ptr = base;
    tensor ->mmap_size = file_size;
    tensor->data = (float *)((char *)base + data_offset);

    *out = tensor;
    return 0;
}




void op_save_bin(tf_stack_t *stack) {
    // salva tensore in file name
    check_stack_size(stack, 2, "}");
    check_top_is_string(stack, "}");
    
    stack_item_t tensor_item = stack->items[stack->top - 1];
    if (tensor_item.type != STACK_TENSOR) {
        fprintf(stderr, "Errore: il secondo elemento dello stack deve essere un tensore\n");
        exit(1);
    }

    char *filename = stack_pop_string(stack);
    tensor_t *tensor = tensor_item.value.tensor;

    if (filename == NULL || filename[0] == '\0') {
        fprintf(stderr, "Errore: nome file non valido '%s'\n", filename);
        free(filename);
        exit(1);
    }

    if (tensor_to_bin(filename, tensor) != 0) {
        fprintf(stderr, "Errore: impossibile scrivere il file binario '%s'\n", filename);
        free(filename);
        exit(1);
    }

    stack_pop_tensor(stack); // rimuove il tensore dopo averlo salvato
    free(filename);
    
}





