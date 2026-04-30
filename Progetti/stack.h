#ifndef STACK_H 
#define STACK_H  

#include "tensor.h"
#include <stdbool.h>
#define MAX_STACK_SIZE 100

typedef enum {
    STACK_TENSOR,
    STACK_STRING
} stack_item_type;




typedef struct {
    stack_item_type type;   // STACK_TENSOR o STACK_STRING
    union {
        tensor_t *tensor;
        char     *string;
    } value;                // il dato vero
} stack_item_t;


typedef struct {
    stack_item_t items[MAX_STACK_SIZE];  
    int          top;                    
} stack_t;


// Funzione per inizializzare lo stack
void stack_init(stack_t *stack);

void stack_push_tensor(stack_t *stack, tensor_t *tensor);

void stack_push_string(stack_t *stack, const char *string);

stack_item_t stack_pop(stack_t *stack);

stack_item_t * stack_peak(stack_t *stack);


bool stack_is_empty(stack_t *stack);

#endif