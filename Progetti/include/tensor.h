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
    void    *mmap_ptr; // puntatore alla memoria mappata
};


struct on_disk_tensor {
    int32_t shape[MAX_DIM];
    int32_t ndim;
    off_t   data_offset;
};


typedef struct tensor tensor_t;
typedef struct on_disk_tensor on_disk_tensor_t;

tensor_t *tensor_alloc(int32_t *shape, int32_t ndim);  // ritorna puntatore
void      tensor_incref(tensor_t *t);                   // void
void      tensor_decref(tensor_t *t);                   // void, fa free se refcount=0
int       tensor_numel(const tensor_t *t);              // ritorna int
void      tensor_print(const tensor_t *t);              // void

#endif
