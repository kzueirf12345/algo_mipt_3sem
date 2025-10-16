#include <endian.h>
#include <assert.h>
#include <stdlib.h>
#include <errno.h>
#include <stdio.h>
#include <stdbool.h>

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

static void swap_int(int* a, int* b) {
    int temp = *a;
    *a = *b;
    *b = temp;
}

enum Command {
    COMMAND_CTOR        = 0,
    COMMAND_INSERT      = 1,
    COMMAND_PRINT_ROOT  = 2,
    COMMAND_ERASE_ROOT  = 3,
    COMMAND_PRINT_SIZE  = 4,
    COMMAND_MERGE       = 5,
    COMMAND_DTOR        = 6,
};

enum HeapType {
    HEAP_TYPE_MIN = 0,
    HEAP_TYPE_MAX = 1,
};

typedef struct Heap {
    enum HeapType type;

    size_t capacity;
    size_t size;
    int* data;
} heap_t;

#define HEAP_TYPE_IF(min, max) (heap->type == HEAP_TYPE_MIN ? (min) : (max))

int heaps_ctor(heap_t** heaps, size_t size);
int heaps_dtor(heap_t** heaps, size_t size);

int heap_ctor(heap_t* heap, size_t capacity, enum HeapType type);
int heap_dtor(heap_t* heap);

int heap_sift_down(heap_t* heap, size_t ind);
int heap_sift_up  (heap_t* heap, size_t ind);

int heap_insert(heap_t* heap, int elem);
int heap_erase_root(heap_t* heap);

int heap_merge(heap_t* src, heap_t* dst);

int main() {
    size_t heaps_size = 0;
    
    if (scanf("%lu", &heaps_size) != 1) {
        fprintf(stderr, "Can't scanf heaps_size\n");
        return EXIT_FAILURE;
    }

    heap_t* heaps = NULL;
    ERROR_HANDLE(heaps_ctor(&heaps, heaps_size));

    enum Command command_type = 0;

    while (true) {
        if (scanf("%u", &command_type) != 1) {
            if (feof(stdin) && !ferror(stdin)) {
                break;
            }
            heaps_dtor(&heaps, heaps_size);
            fprintf(stderr, "Can't scanf command type\n");
            return EXIT_FAILURE;
        }

        switch (command_type) {
            case COMMAND_CTOR: {
                size_t heap_num = 0;
                size_t capacity = 0;
                enum HeapType type = 0;

                if (scanf("%lu %lu %u", &heap_num, &capacity, &type) != 3) {
                    heaps_dtor(&heaps, heaps_size);
                    fprintf(stderr, "Can't scanf heap ctor input\n");
                    return EXIT_FAILURE;
                }

                ERROR_HANDLE(
                    heap_ctor(&heaps[heap_num], capacity, type), 
                    heaps_dtor(&heaps, heaps_size);
                );
                break;
            }
            
            case COMMAND_INSERT: {
                size_t heap_num = 0;
                int elem = 0;

                if (scanf("%lu %d", &heap_num, &elem) != 2) {
                    heaps_dtor(&heaps, heaps_size);
                    fprintf(stderr, "Can't scanf heap insert input\n");
                    return EXIT_FAILURE;
                }

                ERROR_HANDLE(
                    heap_insert(&heaps[heap_num], elem), 
                    heaps_dtor(&heaps, heaps_size);
                );
                break;
            }
            
            case COMMAND_PRINT_ROOT: {
                size_t heap_num = 0;

                if (scanf("%lu", &heap_num) != 1) {
                    heaps_dtor(&heaps, heaps_size);
                    fprintf(stderr, "Can't scanf heap print root input\n");
                    return EXIT_FAILURE;
                }

                printf("%d\n", heaps[heap_num].data[0]);
                break;
            }
            
            case COMMAND_ERASE_ROOT: {
                size_t heap_num = 0;

                if (scanf("%lu", &heap_num) != 1) {
                    heaps_dtor(&heaps, heaps_size);
                    fprintf(stderr, "Can't scanf heap erase root input\n");
                    return EXIT_FAILURE;
                }

                ERROR_HANDLE(
                    heap_erase_root(&heaps[heap_num]), 
                    heaps_dtor(&heaps, heaps_size);
                );
                break;
            }
            
            case COMMAND_PRINT_SIZE: {
                size_t heap_num = 0;

                if (scanf("%lu", &heap_num) != 1) {
                    heaps_dtor(&heaps, heaps_size);
                    fprintf(stderr, "Can't scanf heap print size input\n");
                    return EXIT_FAILURE;
                }

                printf("%lu\n", heaps[heap_num].size);
                break;
            }
            
            case COMMAND_MERGE: {
                size_t heap_src_num = 0;
                size_t heap_dst_num = 0;

                if (scanf("%lu %lu", &heap_src_num, &heap_dst_num) != 2) {
                    heaps_dtor(&heaps, heaps_size);
                    fprintf(stderr, "Can't scanf heap erase root input\n");
                    return EXIT_FAILURE;
                }

                ERROR_HANDLE(
                    heap_merge(&heaps[heap_src_num], &heaps[heap_dst_num]), 
                    heaps_dtor(&heaps, heaps_size);
                );
                break;
            }
            
            case COMMAND_DTOR: {
                size_t heap_num = 0;

                if (scanf("%lu", &heap_num) != 1) {
                    heaps_dtor(&heaps, heaps_size);
                    fprintf(stderr, "Can't scanf heap dtor input\n");
                    return EXIT_FAILURE;
                }

                ERROR_HANDLE(
                    heap_dtor(&heaps[heap_num]), 
                    heaps_dtor(&heaps, heaps_size);
                );
                
                break;
            }

            default:
                fprintf(stderr, "Unknown command type\n");
                heaps_dtor(&heaps, heaps_size);
                return EXIT_FAILURE;
        }
    }

    ERROR_HANDLE(heaps_dtor(&heaps, heaps_size));

    return EXIT_SUCCESS;
}


int heaps_ctor(heap_t** heaps, size_t size) {
    assert(heaps);
    assert(size);

    *heaps = calloc(size, sizeof(heap_t));

    if (!heaps) {
        fprintf(stderr, "Can't calloc heaps\n");
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

int heaps_dtor(heap_t** heaps, size_t size) {
    assert(heaps);

    for (size_t ind = 0; ind < size; ++ind) {
        ERROR_HANDLE(heap_dtor(&(*heaps)[ind]));
    }

    free(*heaps); heaps = NULL;

    return EXIT_SUCCESS;
}

int heap_ctor(heap_t* heap, size_t capacity, enum HeapType type) {
    assert(heap);
    assert(capacity);

    heap->data = calloc(capacity, sizeof(*heap->data));

    if (!heap->data) {
        fprintf(stderr, "Can't calloc heap->data\n");
        return EXIT_FAILURE;
    }

    heap->capacity = capacity;
    heap->type = type;
    heap->size = 0;

    return EXIT_SUCCESS;
}

int heap_dtor(heap_t* heap) {
    assert(heap);

    free(heap->data); heap->data = NULL;

    return EXIT_SUCCESS;
}

int heap_sift_down(heap_t* heap, size_t ind) {
    assert(heap);

    while (2 * ind + 1 < heap->size) {
        size_t  left_ind = 2 * ind + 1;
        size_t right_ind = 2 * ind + 2;
        size_t next_ind = left_ind;

        if (right_ind < heap->size 
         && HEAP_TYPE_IF(
                heap->data[right_ind] < heap->data[left_ind],
                heap->data[right_ind] > heap->data[left_ind]
            )
        ) {
            next_ind = right_ind;
        }

        if (HEAP_TYPE_IF(
                heap->data[ind] <= heap->data[next_ind],
                heap->data[ind] >= heap->data[next_ind]
            )
        ) {
            break;
        }

        swap_int(&heap->data[ind], &heap->data[next_ind]);
        ind = next_ind;
    }

    return EXIT_SUCCESS;
}

int heap_sift_up(heap_t* heap, size_t ind) {
    assert(heap);

    while(ind != 0 
       && HEAP_TYPE_IF(
            heap->data[ind] < heap->data[(ind - 1) / 2], 
            heap->data[ind] > heap->data[(ind - 1) / 2]
        )
    ) {
        swap_int(&heap->data[ind], &heap->data[(ind - 1) / 2]);
        ind = (ind - 1) / 2;
    }

    return EXIT_SUCCESS;
}

int heap_insert(heap_t* heap, int elem) {
    assert(heap);

    heap->data[heap->size] = elem;
    ++heap->size;
    ERROR_HANDLE(heap_sift_up(heap, heap->size - 1));

    return EXIT_SUCCESS;
}

int heap_erase_root(heap_t* heap) {
    assert(heap);

    if (heap->size == 0) {
        fprintf(stderr, "Haven't root\n");
        return EXIT_FAILURE;
    }

    heap->data[0] = heap->data[heap->size - 1];
    --heap->size;
    ERROR_HANDLE(heap_sift_down(heap, 0));

    return EXIT_SUCCESS;
}

int heap_merge(heap_t* src, heap_t* dst) {
    assert(src);
    assert(dst);

    while (dst->size) {
        ERROR_HANDLE(heap_insert(src, dst->data[dst->size - 1]));
        --dst->size;
    }

    return EXIT_SUCCESS;
}
