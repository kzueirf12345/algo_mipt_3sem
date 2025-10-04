#include <endian.h>
#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>

#pragma GCC optimize("O3,unroll-loops")
#pragma GCC target("avx2")

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

typedef struct stack {
    void* data;
    size_t elem_size;
    size_t capacity;
    size_t size;
} stack_t;

typedef struct queue {
    stack_t* stack1;
    stack_t* stack2;
} queue_t;

// queue_t*    queue_new   (size_t elem_size);
// int         queue_push  (queue_t* q, const void* elem);
// int         queue_pop   (queue_t* q, void* elem);
// int         queue_empty (const queue_t* q); 
// queue_t*    queue_delete(queue_t* q);
// void        queue_print (const queue_t* q, void (*pf)(const void* data)); 

// static void print_double(const void* q) {
//    printf("%lg", *(const double*)q);
// }

// int main() {
//     struct queue *q = queue_new(sizeof(double));
//     for (int i = 0; i < 10; i++) {
//         double tmp = i * i;
//         queue_push(q, &tmp);
//     }
//     queue_print(q, print_double);
//     while (!queue_empty(q)) {
//         double tmp;
//         queue_pop(q, &tmp);
//         printf("%lg\n", tmp);
//     }                                                                                                                                                                                        
 
//     q = queue_delete(q);
// }


// LOCAL
int         stack_resize_(stack_t* stack);
stack_t*    stack_new    (size_t elem_size);
int         stack_push   (stack_t* stack, const void* elem);
int         stack_pop    (stack_t* stack, void* elem); 
stack_t*    stack_delete (stack_t* stack); 
void        stack_print  (const stack_t* stack, void (*pf)(const void* st));
int         stack_top    (stack_t* stack, void* elem); 
int         stack_empty  (const stack_t* st);


#define START_CAPACITY_ 2
stack_t* stack_new(size_t elem_size) {
    assert(elem_size);

    stack_t* stack = (stack_t*)calloc(1, sizeof(stack_t));

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

int stack_push(stack_t* stack, const void* elem) {
    assert(stack);
    assert(elem);

    ERROR_HANDLE(stack_resize_(stack));

    if (!memcpy((char*)stack->data + stack->size * stack->elem_size, elem, stack->elem_size)) {
        return EXIT_FAILURE;
    }
    ++stack->size;

    return EXIT_SUCCESS;
}

int stack_empty(const stack_t* stack) {
    assert(stack);

    return stack->size == 0;
}

int stack_top(stack_t* stack, void* elem) {
    assert(stack);
    assert(elem);

    if (stack_empty(stack)) {
        return EXIT_FAILURE;
    }

    if (!memcpy(elem, (char*)stack->data + (stack->size - 1) * stack->elem_size, stack->elem_size)) {
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

int stack_pop(stack_t* stack, void* elem) {
    assert(stack);
    assert(elem);

    if (stack_empty(stack)) {
        return EXIT_FAILURE;
    }

    ERROR_HANDLE(stack_top(stack, elem));
    --stack->size;

    return EXIT_SUCCESS;
}

stack_t* stack_delete(stack_t* stack) {
    assert(stack);

    free(stack->data);
    free(stack);

    return NULL;
}

void stack_print(const stack_t* stack, void (*pf)(const void* st)) {
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

int stack_resize_(stack_t* stack) {
    assert(stack);

    if (stack->size == stack->capacity) {
        stack->capacity = 2 * stack->capacity;

        if (!(stack->data = realloc(stack->data, stack->capacity * stack->elem_size))) {
            return EXIT_FAILURE;
        }
    } else if (2 * stack->size < stack->capacity && stack->capacity > 1) {
        stack->capacity = stack->capacity / 2;

        if (!(stack->data = realloc(stack->data, stack->capacity * stack->elem_size))) {
            return EXIT_FAILURE;
        }
    }

    return EXIT_SUCCESS;
}

queue_t* queue_new(size_t elem_size) {
    assert(elem_size);

    queue_t* queue = (queue_t*)calloc(1, sizeof(queue_t));

    if (!queue) {
        return NULL;
    }

    queue->stack1 = stack_new(elem_size);

    if (!queue->stack1) {
        return NULL;
    }

    queue->stack2 = stack_new(elem_size);

    if (!queue->stack2) {
        return NULL;
    }

    return queue;
}

int queue_swap_stacks_(queue_t* q);

int queue_swap_stacks_(queue_t* q) {
    assert(q);

    void* swap_elem = calloc(1, q->stack1->elem_size);
    while (!stack_empty(q->stack1)) {
        ERROR_HANDLE(stack_pop (q->stack1, swap_elem));
        ERROR_HANDLE(stack_push(q->stack2, swap_elem));
    }
    free(swap_elem);

    return EXIT_SUCCESS;
}

int queue_push(queue_t* q, const void* elem) {
    assert(q);
    assert(elem);

    ERROR_HANDLE(stack_push(q->stack1, elem));

    return EXIT_SUCCESS;
}

int queue_pop(queue_t* q, void* elem) {
    assert(q);

    if (!stack_empty(q->stack2)) {
        ERROR_HANDLE(stack_pop(q->stack2, elem));

        return EXIT_SUCCESS;
    }

    ERROR_HANDLE(queue_swap_stacks_(q));

    if (stack_empty(q->stack2)) {
        return EXIT_FAILURE;
    }
    
    ERROR_HANDLE(stack_pop(q->stack2, elem));

    return EXIT_SUCCESS;
}

int queue_empty(const queue_t* q) {
    return stack_empty(q->stack1) && stack_empty(q->stack2);
}

void queue_print(const queue_t* q, void (*pf)(const void* data)) {
    assert(q);
    assert(pf);

    queue_swap_stacks_((queue_t*)q);

    stack_print(q->stack2, pf);
}

queue_t* queue_delete(queue_t* q) {
    assert(q);

    stack_delete(q->stack1);
    stack_delete(q->stack2);
    free(q);

    return NULL;
}