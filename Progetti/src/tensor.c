#include "tensor.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/mman.h>
#include <unistd.h>

tensor_t *tensor_alloc(int32_t *shape, int32_t ndim) {
    tensor_t *t = (tensor_t *)malloc(sizeof(tensor_t));
    if (!t) {
        fprintf(stderr, "Errore: malloc fallita\n");
        exit(1);
    }
    t->ndim = ndim;
    memcpy(t->shape, shape, ndim * sizeof(int32_t));
    t->refcount = 1;
    t->is_mmap = false;
    t->fd = -1;
    t->mmap_size = 0;

    int numel = tensor_numel(t);
    t->data = (float *)malloc(numel * sizeof(float));
    if (!t->data) {
        free(t);
        fprintf(stderr, "Errore: malloc fallita per data\n");
        exit(1);
    }

    return t;
}

void tensor_incref(tensor_t *t) {
    if (t) {
        t->refcount++;
    }
}

void tensor_decref( tensor_t *t ){
    if(t){
        t ->refcount--;
        if(t->refcount == 0){
           if (t->is_mmap) {
                munmap(t->data, t->mmap_size);
                if (t->fd != -1) close(t->fd);
            } else {
                free(t->data);   // solo se NON è mmap
            }
            free(t);  // la struct è sempre malloc'd
        }
    }
}


int tensor_numel(const tensor_t *t){
    int numel = 1;
    for(int i = 0; i < t->ndim; i++){
        numel *= t->shape[i];
    }
    return numel;
}

void tensor_print(const tensor_t *t) {
        if (!t) {
            fprintf(stderr, "Errore: tensor è NULL\n");
            return;      
        }
    printf("Tensor(shape=[");

        for (int i = 0; i < t->ndim; i++) {
            printf("%d", t->shape[i]);
            if (i < t->ndim - 1) printf(" ");
        }
        printf("], data=[");
        int numel = tensor_numel(t);
        for (int i = 0; i < numel; i++) {
            printf("%g", t->data[i]);
            if (i < numel - 1) printf(" ");
        }
        printf("])\n");
    }
