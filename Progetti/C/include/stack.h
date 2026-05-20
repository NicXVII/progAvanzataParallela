//NICOLAS DIMINICH SM3201661

#ifndef STACK_H
#define STACK_H

#include "tensor.h"
#include <stdbool.h>

#define MAX_STACK_SIZE 2048

typedef enum {
    STACK_TENSOR,
    STACK_STRING
} stack_item_type;

typedef struct {
    stack_item_type type;
    union {
        tensor_t *tensor;
        char     *string;
    } value;
} stack_item_t;

typedef struct {
    stack_item_t items[MAX_STACK_SIZE];
    int          top;
} tf_stack_t;


//------------------------------------------------------------------------------------------------------
void      stack_init(tf_stack_t *stack);
bool      stack_is_empty(tf_stack_t *stack);

void      stack_push_tensor(tf_stack_t *stack, tensor_t *tensor);
void      stack_push_string(tf_stack_t *stack, const char *string);

tensor_t *stack_pop_tensor(tf_stack_t *stack);   
char     *stack_pop_string(tf_stack_t *stack);   

stack_item_t *stack_peek(tf_stack_t *stack);    
#endif
