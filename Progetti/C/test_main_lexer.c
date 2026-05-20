#include <stdio.h>
#include "lexer.h"

int main(void) {
    // Crea un file di test al volo
    FILE *f = fopen("test.tf", "w");
    fprintf(f, "\"hello world\" 1.0 2.0 3.0 + r print -5.0\n");
    fclose(f);

    lexer_t *lexer = lexer_create("test.tf");

    token_t tok;
    while (1) {
        tok = lexer_next_token(lexer);

        if (tok.type == TOKEN_EOF) {
            printf("TOKEN_EOF\n");
            break;
        } else if (tok.type == TOKEN_STRING) {
            printf("TOKEN_STRING: \"%s\"\n", tok.value.string);
        } else if (tok.type == TOKEN_TENSOR) {
            printf("TOKEN_TENSOR: [");
            for (int i = 0; i < tok.value.tensor->shape[0]; i++)
                printf(" %.2f", tok.value.tensor->data[i]);
            printf(" ]\n");
        } else if (tok.type == TOKEN_OP) {
            printf("TOKEN_OP: %s\n", tok.value.op);
        }

        token_free(&tok);
    }

    lexer_free(lexer);
    return 0;
}