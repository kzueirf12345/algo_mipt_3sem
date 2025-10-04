#include <endian.h>
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <sys/types.h>

#define ERROR_HANDLE(call_func, ...)                                                                \
    do {                                                                                            \
        int error_handler = call_func;                                                              \
        if (error_handler)                                                                          \
        {                                                                                           \
            return error_handler;                                                                   \
        }                                                                                           \
    } while(0)

#define MAX(a, b)  ((a) > (b) ? (a) : (b))
#define MIN(a, b)  ((a) < (b) ? (a) : (b))
#define CEIL(a, b) (((a) + (b) - 1) / (b))

typedef struct set {
    size_t   capacity;
    size_t   elems;
    size_t   data_size;
    uint8_t* data;
} set_t;

// LOCAl
ssize_t first_byte_in_bit_(uint8_t byte, size_t byte_ind, size_t start_bit_ind);

// struct set *set_new(size_t capacity);
// int set_insert(struct set *s, size_t elem);
// int set_erase(struct set *s, size_t elem); 
// int set_find(struct set const *s, size_t elem);
// struct set *set_delete(struct set *s); 
// int set_empty(struct set const *s); 
// ssize_t set_findfirst(struct set const *s, size_t start); 
// size_t set_size(struct set const *s); 
// void set_print(struct set const *s);

// int main() {
//     struct set *s = set_new(10);
//     assert(set_insert(s, 1) == 0);
//     assert(set_insert(s, 2) == 0);
//     assert(set_insert(s, 100) != 0);
//     assert(set_size(s) == 2);
//     set_print(s);
//     assert(set_find(s,1) != 0);
//     assert(set_find(s,100) == 0);
//     assert(set_insert(s, 5) == 0);
//     assert(set_erase(s,2) == 0);
//     assert(set_findfirst(s, 2) == 5);
//     assert(set_findfirst(s, 10) == -1);
//     assert(set_delete(s) == NULL);
// }

struct set *set_new(size_t capacity) {
    set_t* set = (set_t*)calloc(1, sizeof(set_t));

    if (!set) {
        return NULL;
    }

    set->elems = 0;
    set->capacity = capacity;
    set->data_size = CEIL(capacity, 8);

    set->data = (uint8_t*)calloc(set->data_size, sizeof(uint8_t));
    if (!set->data) {
        return NULL;
    }

    return set;
}

int set_insert(struct set *s, size_t elem) {
    if (!s || elem >= s->capacity) {
        return EXIT_FAILURE;
    }

    const size_t byte_ind = elem / 8;
    const size_t bit_ind  = elem % 8;
    const uint8_t shifted_bit = (1u << bit_ind);

    s->elems += !(s->data[byte_ind] &  shifted_bit);
                  s->data[byte_ind] |= shifted_bit;
    
    return EXIT_SUCCESS;
}

int set_erase(struct set *s, size_t elem) {
    if (!s || elem >= s->capacity) {
        return EXIT_FAILURE;
    }

    const size_t byte_ind = elem / 8;
    const size_t bit_ind  = elem % 8;
    const uint8_t shifted_bit = (1u << bit_ind);

    s->elems -= !!(s->data[byte_ind] &   shifted_bit);
                   s->data[byte_ind] &= ~shifted_bit;

    return EXIT_SUCCESS;
}

int set_find(struct set const *s, size_t elem) {
    if (!s || elem >= s->capacity) {
        return 0;
    }

    const size_t byte_ind = elem / 8;
    const size_t bit_ind  = elem % 8;
    const uint8_t shifted_bit = (1u << bit_ind);

    return !!(s->data[byte_ind] & shifted_bit);
}

struct set *set_delete(struct set *s) {
    if (!s) {
        return NULL;
    }

    free(s->data);
    free(s);

    return NULL;
}

int set_empty(struct set const *s) {
    if (!s) {
        return 0;
    }

    return s->elems != 0;
}

ssize_t set_findfirst(struct set const *s, size_t start) {
    if (!s || start >= s->capacity) {
        return -1;
    }

    const size_t start_byte_ind = start / 8;
    const size_t start_bit_ind  = start % 8;

    const uint8_t start_byte_mask = (0xFFu << start_bit_ind);

    if (s->data[start_byte_ind] & start_byte_mask) {
        return first_byte_in_bit_(s->data[start_byte_ind], start_byte_ind, start_bit_ind);
    }

    for (size_t byte_ind = start_byte_ind + 1; byte_ind < s->data_size; ++byte_ind)
    {
        if (s->data[byte_ind])
        {
            return first_byte_in_bit_(s->data[byte_ind], byte_ind, 0);
        }
    }

    return -1;
}

ssize_t first_byte_in_bit_(uint8_t byte, size_t byte_ind, size_t start_bit_ind)
{
    for (size_t i = start_bit_ind; i < 8; ++i)
    {
        if (byte & (1u << i))
        {
            return (ssize_t)(byte_ind * 8 + i);
        }
    }

    return -1;
}

size_t set_size(struct set const *s) {
    if (!s) {
        return 0;
    }

    return s->elems;
}

void set_print(struct set const *s) {
    if (!s) {
        return;
    }

    printf("[");

    bool first = true;
    for (size_t num = 0; num < s->capacity; ++num) {
        if (set_find(s, num))
        {
            if (!first) {
                printf(", ");
            }
            printf("%zu", num);
            first = false;
        }
    }

    printf("]\n");
}