#include "tensor.h"

#define MAX_STACK_SIZE 100

typedef enum {
    STACK_TENSOR,
    STACK_STRING
} stack_item_type;


union {
    tensor_t *tensor;
    char     *string;
} value;

typedef struct {
    stack_item_type type;   // STACK_TENSOR o STACK_STRING
    union {
        tensor_t *tensor;
        char     *string;
    } value;                // il dato vero
} stack_item_t;


