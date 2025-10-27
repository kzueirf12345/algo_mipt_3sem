#include <stdlib.h>
#include <string.h>
#include <stdio.h>

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

static inline void prefetch(const void *ptr) {
    __builtin_prefetch(ptr, 0, 3);
}

void fast_sort(unsigned *begin, unsigned *end) 
{
    size_t n = end - begin;
    if (n <= 1) return;

    if (n < 128) {
        for (size_t i = 1; i < n; i++) {
            unsigned key = begin[i];
            size_t j = i;
            while (j > 0 && begin[j - 1] > key) {
                begin[j] = begin[j - 1];
                j--;
            }
            begin[j] = key;
        }
        return;
    }

    unsigned *temp = (unsigned*)aligned_alloc(32, n * sizeof(unsigned));
    if (!temp) return;

    unsigned *src = begin;
    unsigned *dst = temp;

    size_t count[256] __attribute__((aligned(64)));

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
            if (i + 4 < 256) prefetch(count + i + 4);
            size_t tmp = count[i];
            count[i] = sum;
            sum += tmp;
        }

        size_t positions[256];
        memcpy(positions, count, sizeof(positions));

        for (size_t i = 0; i < n; ++i) {
            unsigned b = (src[i] >> shift) & 0xFF;
            dst[positions[b]++] = src[i];
        }

        unsigned *tmp = src;
        src = dst;
        dst = tmp;
    }

    if (src != begin) {
        memcpy(begin, src, n * sizeof(unsigned));
    }

    free(temp);
}
