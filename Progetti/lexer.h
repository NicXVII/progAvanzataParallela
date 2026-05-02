#ifndef LEXER_H
#define LEXER_H

#include "tensor.h"
#include <stddef.h>

typedef enum {
    TOKEN_TENSOR,
    TOKEN_STRING,
    TOKEN_OP,
    TOKEN_EOF
} token_type_t;   

typedef struct {
    token_type_t type;   
    union {
        tensor_t *tensor;
        char     *string;
        char      op[32];
    } value;
} token_t;  

typedef struct {
    char *src;
    size_t      len;
    size_t      pos;
} lexer_t;


lexer_t *lexer_create(const char *src);

void    lexer_free(lexer_t *lexer);

token_t lexer_next_token(lexer_t *lexer);


void    token_free(token_t *token);
#endif