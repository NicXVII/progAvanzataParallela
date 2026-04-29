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
    for(int j = 1; j < h+1; j++) {
        for(int i = 1; i <w+1; i++) {
            prefix[j*(w+1) + i] = src_data[(j-1)*w + (i-1)] + prefix[(j-1)*(w+1) + i] + prefix[j*(w+1) + (i-1)] - prefix[(j-1)*(w+1) + (i-1)];
        }
    }

  

    return prefix;
}


int edge_highlight(netpbm_ptr src, char *path, netpbm_ptr dst) {
    if (src == NULL || dst == NULL || path == NULL) {
        return -1;
    }
    int r = RADIUS_BORDER_HIGHLIGHT;
    int err = create_output_image(path, src, dst, r);
    if (err != 0) return err;
    unsigned long long *prefix = build_prefix_sum((unsigned char *)(src->data + src->offset), src->width, src->height);
    if (prefix == NULL) {
        return -1;
    }


    int width = src->width;
    int height = src->height;

    
     for(int x = r; x < width-r; x++) {
        for(int y = r; y < height-r; y++) {
            unsigned long long A = prefix[(y-r) * (width+1) + (x-r)];            
            unsigned long long                   B = prefix[(y-r)   * (width+1) + (x+r+1)];
            unsigned long long                    C = prefix[(y+r+1) * (width+1) + (x-r)];
            unsigned long long                    D = prefix[(y+r+1) * (width+1) + (x+r+1)];

            unsigned long long sum = D - B - C + A;
            unsigned long long avg = sum / ((2*r+1)*(2*r+1));
        

            dst->data[(y - r) * (width - 2*r) + (x - r) + dst->offset] = (avg >= *pixel_at(src, x, y)) ? 255 : 0;;
        }
    }
    return 0;
}