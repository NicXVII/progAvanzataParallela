//NICOLAS DIMINICH SM3201661

#include "lexer.h"
#include "stack.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

lexer_t *lexer_create(const char *path)
{
    FILE *fp = fopen(path,"r");
    if (fp == NULL){
            fprintf(stderr, "Errore: impossibile aprire il file %s\n", path);
            exit(1);
    }

    fseek(fp, 0, SEEK_END);  
    long size = ftell(fp);   
    fseek(fp, 0, SEEK_SET);


    lexer_t *lexer = (lexer_t *) malloc(sizeof(lexer_t));
    if (lexer == NULL) {
    fprintf(stderr, "Errore: malloc fallita\n");
    exit(1);
    }    
    lexer->src = malloc(size + 1);
   if (lexer->src == NULL) {
    fprintf(stderr, "Errore: malloc fallita\n");
    exit(1);
    }
    fread(lexer->src, 1, size, fp);      
    lexer->src[size] = '\0';           
    lexer->len = size;
    lexer->pos = 0;

    fclose(fp);

    return lexer;

}

void lexer_free(lexer_t *lexer)
{
    if (lexer == NULL) return;


    free(lexer->src);
    free(lexer);
}

void token_free(token_t *token) {
    if (token == NULL) return;
    switch (token->type) {
        case TOKEN_TENSOR: tensor_decref(token->value.tensor); break;
        case TOKEN_STRING: free(token->value.string);          break;
        case TOKEN_OP:     // op[32] è array fisso, nessun free
        case TOKEN_EOF:    break;
    }
}


token_t lexer_next_token(lexer_t *lexer){
    while (lexer->pos < lexer->len && 
       (lexer->src[lexer->pos] == ' '  || 
        lexer->src[lexer->pos] == '\n' ||
        lexer->src[lexer->pos] == '\t')) {
    lexer->pos++;
    }

    if(lexer->pos >= lexer->len)
    {
        
        token_t token;
        token.type = TOKEN_EOF;
        return token;
    }
    
    char c = lexer->src[lexer->pos];

    if (c == '"'){
        
        lexer->pos++;
        size_t start = lexer->pos;

        while(lexer->pos < lexer->len && lexer->src[lexer->pos] != '"')
        {
            lexer->pos++;
        }
        size_t len = lexer->pos - start;

        lexer->pos++;

        char *string = strndup(lexer->src + start, len);

        token_t token;
        token.type = TOKEN_STRING;
        token.value.string = string;
        return token;
    }else if(isdigit((unsigned char)c) || (c == '-' && (lexer->pos + 1 < lexer->len) && isdigit((unsigned char)lexer->src[lexer->pos + 1])))  {
    
    float vals[MAX_STACK_SIZE];
    int count = 0;

    while (lexer->pos < lexer->len) {
        char *end;
        float val = strtof(lexer->src + lexer->pos, &end);
        if (end == lexer->src + lexer->pos) break;  // nessun float letto

        vals[count++] = val;
        lexer->pos = end - lexer->src;

        // salta spazi ← loop separato
        while (lexer->pos < lexer->len && lexer->src[lexer->pos] == ' '){
                lexer->pos++;
        }

        // FUORI dal while degli spazi → controlla il prossimo carattere
        char next = lexer->src[lexer->pos];
        if(!isdigit((unsigned char)next) && !(next == '-' && (lexer->pos + 1 < lexer->len) && isdigit((unsigned char)lexer->src[lexer->pos + 1])))
            break;  // non è un float → fine tensore
    }

    if (count > 0){
        int shape[] = {count};
        tensor_t *tensor = tensor_alloc(shape, 1);
        for(int i=0;i<count;i++)
        {
            tensor->data[i] = vals[i];
        }
        token_t token;
        token.type = TOKEN_TENSOR;
        token.value.tensor = tensor;
        return token;
    }
        token_t token;
        token.type = TOKEN_EOF;
        return token;
    }else{
         size_t start = lexer->pos;

        while (lexer->pos < lexer->len && !isspace((unsigned char)lexer->src[lexer->pos])) {
            lexer->pos++;
        }

        size_t len = lexer->pos - start;

        token_t token;
        token.type = TOKEN_OP;
        strncpy(token.value.op, lexer->src + start, len);
        token.value.op[len] = '\0';
        return token;
    }

 
}
