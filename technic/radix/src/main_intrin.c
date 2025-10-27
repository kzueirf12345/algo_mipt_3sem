#include <stdlib.h>
#include <string.h>
#include <stdio.h>

void fast_sort(unsigned *begin, unsigned *end);

int main() {
    unsigned array[8] = {3,1,4,1,5,9,2,6};
    fast_sort(array, array+8);

    for (size_t i = 0; i < 8; ++i)
    {
        fprintf(stderr, "%u ", array[i]);
    }
    fprintf(stderr, "\n");
}

#include <immintrin.h>
#include <string.h>
#include <stdlib.h>

#ifndef _MM_HINT_T0
#define _MM_HINT_T0 1
#endif

__attribute__((target("avx2")))
static inline void prefetch(const void *ptr) {
    __builtin_prefetch(ptr);
}

void fast_sort(unsigned *begin, unsigned *end) 
{
    if (end - begin <= 1) return;
    
    const size_t n = (size_t)(end - begin);
    
    // Для очень маленьких массивов используем сортировку вставками
    if (n < 1024) {
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

    // Выделяем память с выравниванием 32 байта для AVX
    unsigned *temp = (unsigned*)_mm_malloc(n * sizeof(unsigned), 32);
    if (!temp) return;

    unsigned *src = begin;
    unsigned *dst = temp;

    for (int shift = 0; shift < 32; shift += 8) {
        size_t count[256] = {0};
        
        // Векторизованный подсчет с AVX2
        for (size_t i = 0; i < n; i += 8) {
            if (i + 8 <= n) {
                // Загружаем 8 чисел одновременно
                __m256i data = _mm256_loadu_si256((__m256i*)(src + i));
                // Извлекаем нужный байт
                __m256i bytes = _mm256_srli_epi32(data, shift);
                bytes = _mm256_and_si256(bytes, _mm256_set1_epi32(0xFF));
                
                // Сохраняем байты в массив и обновляем счетчики
                unsigned byte_arr[8] __attribute__((aligned(32)));
                _mm256_store_si256((__m256i*)byte_arr, bytes);
                
                for (int j = 0; j < 8; j++) {
                    count[byte_arr[j]]++;
                }
            } else {
                // Обработка хвоста
                for (size_t j = i; j < n; j++) {
                    unsigned byte = (src[j] >> shift) & 0xFF;
                    count[byte]++;
                }
            }
        }

        // Префиксная сумма с предвыборкой
        size_t sum = 0;
        for (int i = 0; i < 256; i++) {
            if (i + 4 < 256) {
                prefetch(count + i + 4);
            }
            size_t tmp = count[i];
            count[i] = sum;
            sum += tmp;
        }

        // Перестановка с векторизацией где возможно
        size_t positions[256];
        memcpy(positions, count, sizeof(positions));
        
        for (size_t i = 0; i < n; i++) {
            unsigned byte = (src[i] >> shift) & 0xFF;
            dst[positions[byte]++] = src[i];
        }

        // Меняем указатели
        unsigned *tmpPtr = src;
        src = dst;
        dst = tmpPtr;
    }

    // Если результат в temp, копируем обратно
    if (src != begin) {
        // Векторизованное копирование
        for (size_t i = 0; i < n; i += 8) {
            if (i + 8 <= n) {
                __m256i data = _mm256_load_si256((__m256i*)(src + i));
                _mm256_storeu_si256((__m256i*)(begin + i), data);
            } else {
                for (size_t j = i; j < n; j++) {
                    begin[j] = src[j];
                }
            }
        }
    }

    _mm_free(temp);
}