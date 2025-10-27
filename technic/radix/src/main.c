#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stddef.h>
#include <stdalign.h>

// void fast_sort(unsigned *begin, unsigned *end);

// int main() {
//     unsigned array[8] = {3,1,4,1,5,9,2,6};
//     fast_sort(array, array+8);

//     for (size_t i = 0; i < 8; ++i)
//     {
//         fprintf(stderr, "%u ", array[i]);
//     }
//     fprintf(stderr, "\n");
// }

void fast_sort(unsigned *begin, unsigned *end) 
{
    size_t n = end - begin;
    if (n <= 1) return;

    alignas(64) unsigned *temp = (unsigned*)calloc(n, sizeof(unsigned));
    unsigned *src = begin;
    unsigned *dst = temp;

    alignas(64) size_t count[256];

    const size_t blockSize = 4096;

    for (int shift = 0; shift < 32; shift += 8) {
        memset(count, 0, sizeof(count));

        for (size_t start = 0; start < n; start += blockSize) {
            size_t endBlock = (start + blockSize < n) ? start + blockSize : n;
            size_t i = start;

            for (; i + 3 < endBlock; i += 4) {
                unsigned b0 = (src[i] >> shift) & 0xFF;
                unsigned b1 = (src[i+1] >> shift) & 0xFF;
                unsigned b2 = (src[i+2] >> shift) & 0xFF;
                unsigned b3 = (src[i+3] >> shift) & 0xFF;

                ++count[b0]; ++count[b1]; ++count[b2]; ++count[b3];
            }
            for (; i < endBlock; ++i) {
                ++count[(src[i] >> shift) & 0xFF];
            }
        }

        size_t sum = 0;
        for (int i = 0; i < 256; ++i) {
            size_t tmp = count[i];
            count[i] = sum;
            sum += tmp;
        }

        for (size_t start = 0; start < n; start += blockSize) {
            size_t endBlock = (start + blockSize < n) ? start + blockSize : n;
            size_t i = start;

            for (; i + 3 < endBlock; i += 4) {
                unsigned b0 = (src[i] >> shift) & 0xFF;
                unsigned b1 = (src[i+1] >> shift) & 0xFF;
                unsigned b2 = (src[i+2] >> shift) & 0xFF;
                unsigned b3 = (src[i+3] >> shift) & 0xFF;

                dst[count[b0]++] = src[i];
                dst[count[b1]++] = src[i+1];
                dst[count[b2]++] = src[i+2];
                dst[count[b3]++] = src[i+3];
            }
            for (; i < endBlock; ++i) {
                unsigned b = (src[i] >> shift) & 0xFF;
                dst[count[b]++] = src[i];
            }
        }

        unsigned *tmpPtr = src;
        src = dst;
        dst = tmpPtr;
    }

    if (src != begin) {
        memcpy(begin, src, n * sizeof(unsigned));
    }

    free(temp);
}