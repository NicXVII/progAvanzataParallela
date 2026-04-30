#include "stack.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <string.h>
void stack_init(tf_stack_t *stack) {
    stack->top = -1;
}

bool stack_is_empty(tf_stack_t *stack) {
    return stack->top == -1;
}

void stack_push_tensor(tf_stack_t *stack, tensor_t *tensor) {
    if (stack->top >= MAX_STACK_SIZE - 1) {
        fprintf(stderr, "Errore: stack overflow\n");
        exit(1);
    }
    if (tensor == NULL) {
        fprintf(stderr, "Errore: tensore nullo\n");
        exit(1);
    }
    stack->top++;
    stack->items[stack->top].type = tf_stack_tENSOR;
    stack->items[stack->top].value.tensor = tensor;
    tensor_incref(tensor);  
}

void stack_push_string(tf_stack_t *stack,const char *string)
{
    if( stack->top >= MAX_STACK_SIZE - 1 ) {
        fprintf(stderr, "Errore: stack overflow\n");
        exit(1);
    }
        if (string == NULL) {
        fprintf(stderr, "Errore: stringa nullo\n");
        exit(1);
    }
    stack->top++;
    stack->items[stack->top].type = STACK_STRING;
    stack->items[stack->top].value.string = strdup(string);
    if (stack->items[stack->top].value.string == NULL) {
        fprintf(stderr, "Errore: strdup fallita\n");
        exit(1);
    }
}

