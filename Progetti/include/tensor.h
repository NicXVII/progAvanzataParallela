//NICOLAS DIMINICH SM3201661

#ifndef TENSOR_H
#define TENSOR_H
#define MAX_DIM 2
#include <stdbool.h>
#include <sys/types.h>
#include <stdint.h>

struct tensor {
    float    *data;
    int32_t   shape[MAX_DIM];
    int32_t   ndim;
    int       refcount;
    bool      is_mmap;   
    int       fd;
    off_t    mmap_size;
};

typedef struct tensor tensor_t;

tensor_t *tensor_alloc(int32_t *shape, int32_t ndim);  // ritorna puntatore
void      tensor_incref(tensor_t *t);                   // void
void      tensor_decref(tensor_t *t);                   // void, fa free se refcount=0
int       tensor_numel(const tensor_t *t);              // ritorna int
void      tensor_print(const tensor_t *t);              // void

#endif
