#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "merge.h"
#include "merge.h"


void merge_sort(int * v,int len,void (* m) (int *, int, int *, int, int *))
{
    if (v == NULL || len <= 1 || m == NULL) {
        return;
    }

    int * tmp = (int *) malloc((size_t) len * sizeof(int));
    if (tmp == NULL) {
        fprintf(stderr, "Errore: allocazione fallita in merge_sort.\n");
        return;
    }

    int * src = v;
    int * dst = tmp;

    for (int width = 1; width < len; width *= 2) {
        for (int left = 0; left < len; left += 2 * width) {
            int mid = left + width;
            int right = left + 2 * width;

            if (mid > len) {
                mid = len;
            }
            if (right > len) {
                right = len;
            }

            if (mid == right) {
                memcpy(&dst[left], &src[left], (size_t) (right - left) * sizeof(int));
            } else {
                m(&src[left], mid - left, &src[mid], right - mid, &dst[left]);
            }
        }

        int * swap = src;
        src = dst;
        dst = swap;
    }

    if (src != v) {
        memcpy(v, src, (size_t) len * sizeof(int));
    }

    free(tmp);
}

static int is_sorted(const int * v, int len)
{
    for (int i = 1; i < len; i++) {
        if (v[i - 1] > v[i]) {
            return 0;
        }
    }
    return 1;
}

int main(void)
{
    const int len = 20;
    int original[20];
    int a[20];
    int b[20];

    srand((unsigned int) time(NULL));
    for (int i = 0; i < len; i++) {
        original[i] = rand() % 100;
    }

    memcpy(a, original, sizeof(original));
    memcpy(b, original, sizeof(original));

    merge_sort(a, len, merge);
    merge_sort(b, len, merge_branchless);

    printf("merge ordinato: %s\n", is_sorted(a, len) ? "SI" : "NO");
    printf("merge_branchless ordinato: %s\n", is_sorted(b, len) ? "SI" : "NO");

    return 0;
}
