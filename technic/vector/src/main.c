#include <endian.h>
#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>

#define ERROR_HANDLE(call_func, ...)                                                                \
    do {                                                                                            \
        int error_handler = call_func;                                                              \
        if (error_handler)                                                                          \
        {                                                                                           \
            return error_handler;                                                                   \
        }                                                                                           \
    } while(0)

#define MAX(a, b) ((a) > (b) ? (a) : (b))
#define MIN(a, b) ((a) < (b) ? (a) : (b))

typedef struct vector {
    void* data;
    size_t elem_size;
    size_t capacity;
    size_t size;
} vector_t;

//LOCAl
int vector_resize_(vector_t* vector, size_t new_capacity);

// struct vector *vector_new(size_t elems, size_t elem_size);
// int vector_push(struct vector *v, void const *elem);
// int vector_pop (struct vector *v, void *elem); 
// int vector_empty(struct vector const *st); 
// struct vector *vector_delete(struct vector *v); 
// void vector_print(struct vector const *v, void (*pf)(void const *data)); 
// int vector_set(struct vector *v, size_t index, void const *elem); 
// int vector_get(struct vector const *v, size_t index, void *elem); 
// int vector_resize(struct vector *v, size_t new_size); 
// size_t vector_size(struct vector const *v); 

// static void print_int(const void *data) {
//    printf("%d", *(const int *)data);
// }

// int main() {
//     struct vector *v = vector_new(10, sizeof(int));
//     for (size_t i = 0; i < 10; i++) {
//         vector_set(v, i, &i);
//     }
//     // for (size_t i = 0; i < 10; i++) {
//     //     vector_push(v, &i);
//     // }
//     vector_print(v, print_int);
//     v = vector_delete(v);
// }

#define MIN_CAPACITY_ 16
vector_t* vector_new(size_t elems, size_t elem_size) {
    if (!elem_size) {
        return NULL;
    }

    vector_t* vector = (vector_t*)calloc(1, sizeof(vector_t));

    if (!vector) {
        return NULL;
    }

    const size_t capacity = MAX(elems, MIN_CAPACITY_);

    vector->elem_size    = elem_size;
    vector->capacity     = capacity; 
    vector->size         = elems;
    vector->data         = calloc(vector->capacity, vector->elem_size);

    if (!vector->data) {
        return NULL;
    }

    return vector;
}
#undef MIN_CAPACITY_

int vector_resize(vector_t* vector, size_t new_size) {
    if (!vector) {
        return EXIT_FAILURE;
    }

    if (new_size > vector->capacity) {
        ERROR_HANDLE(vector_resize_(vector, new_size));
    }

    vector->size = new_size;

    return EXIT_SUCCESS;
}

int vector_resize_(vector_t* vector, size_t new_capacity) {
    if (!vector) {
        return EXIT_FAILURE;
    }

    void* new_data = realloc(vector->data, new_capacity * vector->elem_size);

    if (!new_data) {
        return EXIT_FAILURE;
    }

    vector->data = new_data;
    vector->capacity = new_capacity;

    return EXIT_SUCCESS;
}

int vector_push(vector_t* vector, const void* elem) {
    if (!vector || !elem) {
        return EXIT_FAILURE;
    }

    if (vector->size + 1 > vector->capacity) {
        ERROR_HANDLE(vector_resize_(vector, 2 * vector->capacity));
    }

    memcpy((char*)vector->data + vector->size * vector->elem_size, elem, vector->elem_size);

    ++vector->size;

    return EXIT_SUCCESS;
}

int vector_empty(const vector_t* vector) {
    return vector->size == 0;
}

int vector_pop(vector_t* vector, void* elem) {
    if (!vector || !elem) {
        return EXIT_FAILURE;
    }

    if (vector_empty(vector)) {
        return EXIT_FAILURE;
    }


    memcpy(elem, (char*)vector->data + (vector->size - 1) * vector->elem_size, vector->elem_size);

    --vector->size;

    return EXIT_SUCCESS;
}

vector_t* vector_delete(vector_t* vector) {
    if (!vector) {
        return NULL;
    }

    free(vector->data);
    free(vector);

    return NULL;
}

void vector_print(const vector_t* vector, void (*pf)(const void* st)) {
    if (!vector || !pf) {
        return;
    }

    printf("[");

    size_t num = 0;
    for (; num + 1 < vector->size; ++num) {
        pf((char*)vector->data + num * vector->elem_size);
        printf(", ");
    }
    if (vector->size > 0) {
        pf((char*)vector->data + num * vector->elem_size);
    }
    printf("]\n");
}

int vector_set(vector_t* vector, size_t index, const void* elem)
{
    if (!vector || !elem) {
        return EXIT_FAILURE;
    }

    if (index >= vector->size) {
        return EXIT_FAILURE;
    }

    memcpy((char*)vector->data + index * vector->elem_size, elem, vector->elem_size);

    return EXIT_SUCCESS;
}

int vector_get(const vector_t* vector, size_t index, void* elem)
{
    if (!vector || !elem) {
        return EXIT_FAILURE;
    }

    if (index >= vector->size) {
        return EXIT_FAILURE;
    }

    memcpy(elem, (char*)vector->data + index * vector->elem_size, vector->elem_size);

    return EXIT_SUCCESS;
}

size_t vector_size(const vector_t* vector) {
    if (!vector) {
        return 0;
    }
    return vector->size;
}