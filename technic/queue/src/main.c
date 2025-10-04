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

typedef struct queue {
    void*   data;
    size_t  elem_size;
    size_t  head;
    size_t  tail;
    size_t  size;
    size_t  capacity;
} queue_t;

// LOCAL
int queue_realloc_(queue_t* queue, size_t new_capacity);

// struct queue *queue_new(size_t elem_size);
// int queue_push(struct queue *q, const void *elem);
// int queue_pop (struct queue *q, void *elem); 
// int queue_empty(struct queue const *q); 
// struct queue *queue_delete(struct queue *q); 
// void queue_print(struct queue const *q, void (*pf)(void const *data)); 

// static void print_double(void const *data) {
//    printf("%lg", *(const double *)data);
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

#define MIN_CAPACITY_ 8
queue_t* queue_new(size_t elem_size) {
    if (!elem_size) {
        return NULL;
    }

    queue_t* queue = (queue_t*)calloc(1, sizeof(queue_t));

    if (!queue) {
        return NULL;
    }

    queue->elem_size = elem_size;
    queue->head      = 0;
    queue->tail      = 0;
    queue->size      = 0;
    queue->capacity  = MIN_CAPACITY_;

    queue->data = calloc(queue->capacity, queue->elem_size);

    if (!queue->data) {
        free(queue);
        return NULL;
    }

    return queue;
}
#undef MIN_CAPACITY_

int queue_realloc_(queue_t* queue, size_t new_capacity) {
    if (!queue) {
        return EXIT_FAILURE;
    }

    void* new_data = calloc(new_capacity, queue->elem_size);

    if (!new_data) {
        return EXIT_FAILURE;
    }

    for (size_t i = 0; i < queue->size; ++i) {
        size_t src_index = (queue->head + i) % queue->capacity;
        char* dst = (char*)new_data + i * queue->elem_size;
        char* src = (char*)queue->data + src_index * queue->elem_size;
        memcpy(dst, src, queue->elem_size);
    }

    free(queue->data);
    queue->data     = new_data;
    queue->capacity = new_capacity;
    queue->head     = 0;
    queue->tail     = queue->size;

    return EXIT_SUCCESS;
}

int queue_push(queue_t* queue, const void* elem) {
    if (!queue || !elem) {
        return EXIT_FAILURE;
    }

    if (queue->size >= queue->capacity) {
        ERROR_HANDLE(queue_realloc_(queue, 2 * queue->capacity));
    }

    memcpy((char*)queue->data + queue->tail * queue->elem_size, elem, queue->elem_size);

    queue->tail = (queue->tail + 1) % queue->capacity;
    ++queue->size;

    return EXIT_SUCCESS;
}

int queue_pop(queue_t* queue, void* elem) {
    if (!queue || !elem) {
        return EXIT_FAILURE;
    }

    if (queue->size == 0) {
        return EXIT_FAILURE;
    }

    memcpy(elem, (char*)queue->data + queue->head * queue->elem_size, queue->elem_size);

    queue->head = (queue->head + 1) % queue->capacity;
    --queue->size;

    return EXIT_SUCCESS;
}

int queue_empty(const queue_t* queue) {
    return queue->size == 0;
}

queue_t* queue_delete(queue_t* queue) {
    if (!queue) {
        return NULL;
    }

    free(queue->data);
    free(queue);

    return NULL;
}

void queue_print(const queue_t* queue, void (*pf)(const void* data)) {
    if (!queue || !pf) {
        printf("[]\n");
        return;
    }

    printf("[");

    for (size_t i = 0; i < queue->size; ++i) {
        size_t index = (queue->tail - 1 - i + queue->capacity) % queue->capacity;   
        char* elem_ptr = (char*)queue->data + index * queue->elem_size;
        pf(elem_ptr);
        
        if (i < queue->size - 1) {
            printf(", ");
        }
    }
    
    printf("]\n");
}