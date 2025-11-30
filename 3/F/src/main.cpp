#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

int compare(const void *a, const void *b);

int compare(const void *a, const void *b) {
    int32_t x = *(const int32_t*)a;
    int32_t y = *(const int32_t*)b;
    return (x > y) - (x < y);
}

int main() {
    uint32_t N, M;
    if (scanf("%u %u", &N, &M) != 2) {
        return EXIT_FAILURE;
    }
    
    int32_t **sets = (int32_t**)calloc(N, sizeof(int32_t*));
    
    for (uint32_t i = 0; i < N; ++i) {
        sets[i] = (int32_t*)calloc(M, sizeof(int32_t));
        
        for (uint32_t j = 0; j < M; ++j) {
            if (scanf("%d", &sets[i][j]) != 1) {
                return EXIT_FAILURE;
            }
        }
        
        qsort(sets[i], M, sizeof(int32_t), compare);
    }
    
    uint16_t maxIntersection = 0;
    
    for (uint32_t i = 0; i < N; ++i) {
        for (uint32_t j = i + 1; j < N; ++j) {
            uint16_t count = 0;
            uint32_t idx1 = 0, idx2 = 0;
            int32_t *arr1 = sets[i];
            int32_t *arr2 = sets[j];
            
            while (idx1 < M && idx2 < M) {
                if (arr1[idx1] == arr2[idx2]) {
                    ++count;
                    ++idx1;
                    ++idx2;
                } else if (arr1[idx1] < arr2[idx2]) {
                    ++idx1;
                } else {
                    ++idx2;
                }
            }
            
            if (count > maxIntersection) {
                maxIntersection = count;
            }
        }
    }
    
    printf("%u\n", maxIntersection);
    
    for (uint32_t i = 0; i < N; ++i) {
        free(sets[i]);
    }
    free(sets);
    
    return 0;
}