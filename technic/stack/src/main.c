#include <endian.h>
#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>

#define ERROR_HANDLE(call_func, ...)                                                                \
    do {                                                                                            \
        int error_handler = call_func;                                                              \
        if (error_handler)                                                                          \
        {                                                                                           \
            fprintf(stderr, "Can't " #call_func". Errno: %d\n",                                     \
                            errno);                                                                 \
            __VA_ARGS__                                                                             \
            return error_handler;                                                                   \
        }                                                                                           \
    } while(0)

typedef struct vector {
    void* data;
    size_t elem_size;
    size_t capacity;
    size_t size;
} vector_t;

// stack_t*    stack_new   (size_t elem_size);
// int         stack_push  (stack_t* stack, const void* elem);
// int         stack_pop   (stack_t* stack, void* elem); 
// stack_t*    stack_delete(stack_t* stack); 
// void        stack_print (const stack_t* stack, void (*pf)(const void* st));
// int         stack_top   (const stack_t* stack, void* elem); 
// int         stack_empty (const stack_t*st);

// LOCAL
int         vector_resize_(vector_t* stack);

// static void print_double(const void *st) {
//    printf("%lg", *(const double *)st);
// }

// int main() {
//     struct stack *st = stack_new(sizeof(double));
//     if (!st) {
//         return EXIT_FAILURE;
//     }
//     for (int i = 0; i < 10; i++) {
//         double tmp = i * i;
//         ERROR_HANDLE(stack_push(st, &tmp), st = stack_delete(st););
//     }
//     stack_print(st, print_double);
//     while (!stack_empty(st)) {
//         double elem = 0;
//         ERROR_HANDLE(stack_top(st, &elem), st = stack_delete(st););
//         printf("top elem: %lg\n", elem);
//         ERROR_HANDLE(stack_pop(st, &elem), st = stack_delete(st););
//         stack_print(st, print_double);
//     }
//     st = stack_delete(st);
// }


#define START_CAPACITY_ 2
vector_t* vector_new(size_t elem_size) {
    assert(elem_size);

    vector_t* stack = (vector_t*)calloc(1, sizeof(vector_t));

    if (!stack) {
        return NULL;
    }

    stack->elem_size    = elem_size;
    stack->capacity     = START_CAPACITY_; 
    stack->size         = 0;
    stack->data         = calloc(stack->capacity, stack->elem_size);

    if (!stack->data) {
        return NULL;
    }

    return stack;
}
#undef START_CAPACITY_

int vector_push(vector_t* stack, const void* elem) {
    assert(stack);
    assert(elem);

    ERROR_HANDLE(vector_resize_(stack));

    if (!memcpy((char*)stack->data + stack->size * stack->elem_size, elem, stack->elem_size)) {
        return EXIT_FAILURE;
    }
    ++stack->size;

    return EXIT_SUCCESS;
}

int vector_top(vector_t* stack, void* elem) {
    assert(stack);
    assert(elem);

    if (!memcpy(elem, (char*)stack->data + (stack->size - 1) * stack->elem_size, stack->elem_size)) {
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

int vector_pop(vector_t* stack, void* elem) {
    assert(stack);
    assert(elem);

    ERROR_HANDLE(vector_top(stack, elem));
    --stack->size;

    return EXIT_SUCCESS;
}

int vector_empty(const vector_t* stack) {
    assert(stack);

    return stack->size == 0;
}

vector_t* vector_delete(vector_t* stack) {
    assert(stack);

    free(stack->data);
    free(stack);

    return NULL;
}

void vector_print(const vector_t* stack, void (*pf)(const void* st)) {
    assert(stack);
    assert(pf);

    printf("[");

    size_t num = 0;
    for (; num + 1 < stack->size; ++num) {
        pf((char*)stack->data + num * stack->elem_size);
        printf(", ");
    }
    if (stack->size > 0) {
        pf((char*)stack->data + num * stack->elem_size);
    }
    printf("]\n");
}

int vector_resize_(vector_t* stack) {
    assert(stack);

    if (stack->size == stack->capacity) {
        stack->capacity = 2 * stack->capacity;

        if (!(stack->data = realloc(stack->data, stack->capacity * stack->elem_size))) {
            return EXIT_FAILURE;
        }
    }

    return EXIT_SUCCESS;
}