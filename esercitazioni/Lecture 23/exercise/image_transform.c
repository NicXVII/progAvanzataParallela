#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <omp.h>

#include "netpbm.h"
#include "image_transform.h"


int blur_image(netpbm_ptr src, char *path, netpbm_ptr dst) {
    if (src == NULL || dst == NULL || path == NULL) {
        return -1;
    }
    int r = RADIUS_BLUR;

    int width = src->width;
    int height = src->height;

    int err = create_output_image(path, src, dst, r);
    if (err != 0) return err;
    #pragma omp parallel for collapse(2)
    for(int x = r; x < width-r; x++) {
        for(int y = r; y < height-r; y++) {
            unsigned long long sum = 0;
            for(int i = -r; i <= r; i++) {
                for(int j = -r; j <= r; j++) {
                    sum += (unsigned char)(src->data[(y+j)*width + (x+i) + src->offset]);
                }
            }
            
            dst->data[(y - r) * (dst->width) + (x - r) + dst->offset] = (unsigned char)(sum / ((2*r+1)*(2*r+1)));
            
        }
    }
    return 0;
}

unsigned long long *build_prefix_sum(const unsigned char *src_data, int w, int h) {

    unsigned long long *prefix = calloc((size_t)(h + 1) * (w + 1), sizeof(unsigned long long));

    if (prefix == NULL) {
        return NULL;
    }

    /*
     * TODO: Da implementare
     */

    return prefix;
}


int edge_highlight(netpbm_ptr src, char *path, netpbm_ptr dst) {
    if (src == NULL || dst == NULL || path == NULL) {
        return -1;
    }
    int r = RADIUS_BORDER_HIGHLIGHT;
    int err = create_output_image(path, src, dst, r);
    if (err != 0) return err;
    /*
     * TODO: Da implementare
     */
    return 0;
}