#include <stdlib.h>
#include <string.h>
#include <stddef.h>
#include <stdint.h>
#include <stdalign.h>
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

void fast_sort(unsigned *begin, unsigned *end)
{
    size_t n = (size_t)(end - begin);
    if (n <= 1) return;

    unsigned *buf = (unsigned*)malloc(n * sizeof(unsigned));

    unsigned *src = begin;
    unsigned *dst = buf;

    alignas(64) size_t count[256];

    const size_t n8 = n & (~(size_t)7); // округление до ближайшего кратного 8

    for (size_t shift = 0; shift < 32; shift += 8)
    {
        memset(count, 0, sizeof(count));

        size_t ind = 0;

        for (; ind < n8; ind += 8) 
        {
            unsigned v0 = src[ind];
            unsigned v1 = src[ind+1];
            unsigned v2 = src[ind+2];
            unsigned v3 = src[ind+3];
            unsigned v4 = src[ind+4];
            unsigned v5 = src[ind+5];
            unsigned v6 = src[ind+6];
            unsigned v7 = src[ind+7];

            ++count[(v0 >> shift) & 255u];
            ++count[(v1 >> shift) & 255u];
            ++count[(v2 >> shift) & 255u];
            ++count[(v3 >> shift) & 255u];
            ++count[(v4 >> shift) & 255u];
            ++count[(v5 >> shift) & 255u];
            ++count[(v6 >> shift) & 255u];
            ++count[(v7 >> shift) & 255u];
        }
        for (; ind < n; ++ind)
        {
            ++count[(src[ind] >> shift) & 255u];
        }
        

        size_t sum = 0;
        for (size_t i = 0; i < 256; ++i) 
        {
            size_t t = count[i];
            count[i] = sum;
            sum += t;
        }

        
        ind = 0;

        for (; ind < n8; ind += 8) 
        {
            unsigned v0 = src[ind];
            unsigned v1 = src[ind+1];
            unsigned v2 = src[ind+2];
            unsigned v3 = src[ind+3];
            unsigned v4 = src[ind+4];
            unsigned v5 = src[ind+5];
            unsigned v6 = src[ind+6];
            unsigned v7 = src[ind+7];

            dst[count[(v0 >> shift) & 255u]++] = v0;
            dst[count[(v1 >> shift) & 255u]++] = v1;
            dst[count[(v2 >> shift) & 255u]++] = v2;
            dst[count[(v3 >> shift) & 255u]++] = v3;
            dst[count[(v4 >> shift) & 255u]++] = v4;
            dst[count[(v5 >> shift) & 255u]++] = v5;
            dst[count[(v6 >> shift) & 255u]++] = v6;
            dst[count[(v7 >> shift) & 255u]++] = v7;
        }
        for (; ind < n; ++ind) 
        {
            unsigned v = src[ind];
            dst[count[(v >> shift) & 255u]++] = v;
        }
    
        unsigned *tmp = src;
        src = dst;
        dst = tmp;
    }

    if (src != begin) 
    {
        memcpy(begin, src, n * sizeof(unsigned));
    }

    free(buf);
}
