#include "stack.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <string.h>
void stack_init(stack_t *stack) {
    stack->top = -1;
}

bool stack_is_empty(stack_t *stack) {
    return stack->top == -1;
}

void stack_push_tensor(stack_t *stack, tensor_t *tensor) {
    if (stack->top >= MAX_STACK_SIZE - 1) {
        fprintf(stderr, "Errore: stack overflow\n");
        exit(1);
    }
    if (tensor == NULL) {
        fprintf(stderr, "Errore: tensore nullo\n");
        exit(1);
    }
    stack->top++;
    stack->items[stack->top].type = STACK_TENSOR;
    stack->items[stack->top].value.tensor = tensor;
    tensor_incref(tensor);  
}

void stack_push_string(stack_t *stack, char *string)
{
    if( stack->top >= MAX_STACK_SIZE - 1 ) {
        fprintf(stderr, "Errore: stack overflow\n");
        exit(1);
    }
        if (string == NULL) {
        fprintf(stderr, "Errore: tensore nullo\n");
        exit(1);
    }
}