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


//push
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
    stack->items[stack->top].type = STACK_TENSOR;
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



// pop
tensor_t *stack_pop_tensor(tf_stack_t *stack) {
    if (stack_is_empty(stack)) {
        fprintf(stderr, "Errore: stack vuoto\n");
        exit(1);
    }
    if (stack->items[stack->top].type != STACK_TENSOR) {
        fprintf(stderr, "Errore: tipo non tensor\n");
        exit(1);
    }
    tensor_t *tensor = stack->items[stack->top].value.tensor;
    stack->top--;
    return tensor;
}

char *stack_pop_string(tf_stack_t *stack) {
    if (stack_is_empty(stack)) {
        fprintf(stderr, "Errore: stack vuoto\n");
        exit(1);
    }
    if (stack->items[stack->top].type != STACK_STRING) {
        fprintf(stderr, "Errore: tipo non string\n");
        exit(1);
    }
    char *string = stack->items[stack->top].value.string;
    stack->top--;
    return string;
}


//peek

stack_item_t *stack_peek(tf_stack_t *stack){
    if (stack_is_empty(stack)) {
        fprintf(stderr, "Errore: stack vuoto\n");
        exit(1);
    }
    stack_item_t *item = &stack->items[stack->top];

    return item;
} 
