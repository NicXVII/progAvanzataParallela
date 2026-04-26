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

    int kernel_side = 2 * RADIUS_BLUR + 1;
    int kernel_area = kernel_side * kernel_side;

    int err = create_output_image(path, src, dst, RADIUS_BLUR);
    if (err != 0) {
        return err;
    }

    unsigned char *src_data = (unsigned char *)src->data + src->offset;
    unsigned char *dst_data = (unsigned char *)dst->data + dst->offset;

    #pragma omp parallel default(none) shared(src, dst, src_data, dst_data, kernel_area)
    {
        #pragma omp for collapse(2)
        for (int x = RADIUS_BLUR; x < src->width - RADIUS_BLUR; x++) {
            for (int y = RADIUS_BLUR; y < src->height - RADIUS_BLUR; y++) {
                int sum = 0;

                for (int kx = -RADIUS_BLUR; kx <= RADIUS_BLUR; kx++) {
                        for (int ky = -RADIUS_BLUR; ky <= RADIUS_BLUR; ky++) {
                        sum += src_data[(y + ky) * src->width + (x + kx)];
                    }
                }

                dst_data[(y - RADIUS_BLUR) * dst->width + (x - RADIUS_BLUR)] =
                    (unsigned char)(sum / kernel_area);
            }
        }
    }

    return 0;
}

unsigned long long *build_prefix_sum(const unsigned char *src_data, int w, int h) {

    unsigned long long *prefix = calloc((size_t)(h + 1) * (w + 1), sizeof(unsigned long long));

    if (prefix == NULL) {
        return NULL;
    }

    #pragma omp parallel for schedule(static) default(none) shared(prefix, src_data, w, h)
    for (int y = 0; y < h; y++) {
        unsigned long long row_sum = 0;
        size_t prefix_row = (size_t)(y + 1) * (w + 1);
        size_t src_row = (size_t)y * w;

        for (int x = 0; x < w; x++) {
            row_sum += src_data[src_row + x];
            prefix[prefix_row + (x + 1)] = row_sum;
        }
    }

    #pragma omp parallel for collapse(2) schedule(static) default(none) shared(prefix, w, h)
    for (int x = 1; x <= w; x++) {
        for (int y = 1; y <= h; y++) {
            prefix[(size_t)y * (w + 1) + x] +=
                prefix[(size_t)(y - 1) * (w + 1) + x];
        }
    }

    return prefix;
}


int edge_highlight(netpbm_ptr src, char *path, netpbm_ptr dst) {
    if (src == NULL || dst == NULL || path == NULL) {
        return -1;
    }

    const int radius = RADIUS_BORDER_HIGHLIGHT;
    const int kernel_side = 2 * radius + 1;
    const int kernel_area = kernel_side * kernel_side;

    int err = create_output_image(path, src, dst, radius);
    if (err != 0) {
        return err;
    }

    unsigned char *src_data = (unsigned char *)src->data + src->offset;
    unsigned char *dst_data = (unsigned char *)dst->data + dst->offset;

    const int w = src->width;
    const int h = src->height;

    unsigned long long *prefix = build_prefix_sum(src_data, w, h);
    if (prefix == NULL) {
        close_image(dst);
        return -2;
    }

    #pragma omp parallel for collapse(2) schedule(static) default(none) shared(src_data, dst_data, prefix, w, h, radius, kernel_area, dst)
    for (int y = radius; y < h - radius; y++) {
        for (int x = radius; x < w - radius; x++) {
            int x0 = x - radius;
            int y0 = y - radius;
            int x1 = x + radius;
            int y1 = y + radius;

            unsigned long long A = prefix[y0 * (w + 1) + x0];
            unsigned long long B = prefix[y0 * (w + 1) + (x1 + 1)];
            unsigned long long C = prefix[(y1 + 1) * (w + 1) + x0];
            unsigned long long D = prefix[(y1 + 1) * (w + 1) + (x1 + 1)];

            unsigned long long sum = D - B - C + A;

            int avg = (int)(sum / kernel_area);
            int center = src_data[y * w + x];

            dst_data[(y - radius) * dst->width + (x - radius)] =
                (unsigned char)(255 * (avg >= center));
        }
    }

    free(prefix);
    return 0;
}
