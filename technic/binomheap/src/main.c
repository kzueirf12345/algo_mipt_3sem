#include <endian.h>
#include <assert.h>
#include <inttypes.h>
#include <stdint.h>
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

static void swap_size_t(size_t* a, size_t* b) {
    size_t temp = *a;
    *a = *b;
    *b = temp;
}


static void swap_int64_t(int64_t* a, int64_t* b) {
    int64_t temp = *a;
    *a = *b;
    *b = temp;
}

enum Command {
    COMMAND_INSERT      = 0,
    COMMAND_TRANS_HEAP  = 1,
    COMMAND_ERASE       = 2,
    COMMAND_UPDATE      = 3,
    COMMAND_PRINT_MIN   = 4,
    COMMAND_ERASE_MIN   = 5,
};

typedef struct Heap {
    int64_t key;
    size_t ind;

    struct Heap* parent;
    struct Heap* child;
    struct Heap* sibling;
    
    size_t degree;
} heap_t;

typedef struct heapInfo {
    heap_t* node;
    heap_t** heap;
} heap_info_t;

heap_info_t heap_infos[1000005] = {};


int heaps_ctor(heap_t*** heaps, size_t size);
int heaps_dtor(heap_t*** heaps, size_t size);

int heap_ctor(heap_t** node, int64_t key, size_t ind, heap_t* parent, heap_t* child, heap_t* sibling, size_t degree);
int heap_dtor(heap_t* heap);

size_t heap_find_node(heap_t** heaps, size_t heaps_size, size_t ind, heap_t** found_node);

int heap_insert(heap_t** heap, int64_t val, size_t ind);
int heap_trans(heap_t** src, heap_t** dst);
int heap_erase(size_t ind);
int heap_update(size_t ind, int64_t val);
int heap_get_min(heap_t* heap, int64_t* min);
int heap_erase_min(heap_t** heap);

int main() 
{
    size_t heaps_size = 0;
    
    if (scanf("%lu", &heaps_size) != 1) {
        fprintf(stderr, "Can't scanf heaps_size\n");
        return EXIT_FAILURE;
    }

    heap_t** heaps = NULL;
    ERROR_HANDLE(heaps_ctor(&heaps, heaps_size));

    size_t commands_cnt = 0;
    
    if (scanf("%lu", &commands_cnt) != 1) {
        fprintf(stderr, "Can't scanf commands_cnt\n");
        return EXIT_FAILURE;
    }

    enum Command command_type = 0;

    size_t global_ind = 1;

    for (size_t command_num = 0; command_num < commands_cnt; ++command_num) 
    {
        if (scanf("%u", &command_type) != 1) {
            if (feof(stdin) && !ferror(stdin)) {
                break;
            }
            heaps_dtor(&heaps, heaps_size);
            fprintf(stderr, "Can't scanf command type\n");
            return EXIT_FAILURE;
        }

        switch (command_type) {
            case COMMAND_INSERT: {
                size_t heap_num = 0;
                int64_t val = 0;

                if (scanf("%lu %ld", &heap_num, &val) != 2) {
                    heaps_dtor(&heaps, heaps_size);
                    fprintf(stderr, "Can't scanf heap insert input\n");
                    return EXIT_FAILURE;
                }
                --heap_num;

                ERROR_HANDLE(
                    heap_insert(&heaps[heap_num], val, global_ind), 
                    heaps_dtor(&heaps, heaps_size);
                );
                ++global_ind;
                break;
            }
            
            case COMMAND_TRANS_HEAP: {
                size_t src_num = 0;
                size_t dst_num = 0;

                if (scanf("%lu %lu", &src_num, &dst_num) != 2) {
                    heaps_dtor(&heaps, heaps_size);
                    fprintf(stderr, "Can't scanf heap trans input\n");
                    return EXIT_FAILURE;
                }
                --src_num;
                --dst_num;

                ERROR_HANDLE(
                    heap_trans(&heaps[src_num], &heaps[dst_num]), 
                    heaps_dtor(&heaps, heaps_size);
                );
                break;
            }
            
            case COMMAND_ERASE: {
                size_t ind = 0;

                if (scanf("%lu", &ind) != 1) {
                    heaps_dtor(&heaps, heaps_size);
                    fprintf(stderr, "Can't scanf heap erase input\n");
                    return EXIT_FAILURE;
                }

                ERROR_HANDLE(
                    heap_erase(ind), 
                    heaps_dtor(&heaps, heaps_size);
                );
                break;
            }
            
            case COMMAND_UPDATE: {
                size_t ind = 0;
                int64_t val = 0;

                if (scanf("%lu %ld", &ind, &val) != 2) {
                    heaps_dtor(&heaps, heaps_size);
                    fprintf(stderr, "Can't scanf heap update input\n");
                    return EXIT_FAILURE;
                }

                ERROR_HANDLE(
                    heap_update(ind, val), 
                    heaps_dtor(&heaps, heaps_size);
                );
                break;
            }
            
            case COMMAND_PRINT_MIN: {
                size_t heap_num = 0;

                if (scanf("%lu", &heap_num) != 1) {
                    heaps_dtor(&heaps, heaps_size);
                    fprintf(stderr, "Can't scanf heap print min input\n");
                    return EXIT_FAILURE;
                }
                --heap_num;

                int64_t min = 0;

                ERROR_HANDLE(
                    heap_get_min(heaps[heap_num], &min), 
                    heaps_dtor(&heaps, heaps_size);
                );

                printf("%ld\n", min);
                break;
            }
            
            case COMMAND_ERASE_MIN: {
                size_t heap_num = 0;

                if (scanf("%lu", &heap_num) != 1) {
                    heaps_dtor(&heaps, heaps_size);
                    fprintf(stderr, "Can't scanf heap erase min input\n");
                    return EXIT_FAILURE;
                }
                --heap_num;

                ERROR_HANDLE(
                    heap_erase_min(&heaps[heap_num]), 
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

static void heap_dtor_recursive(heap_t* heap)
{
    if (!heap) return;

    heap_dtor_recursive(heap->child);
    heap_dtor_recursive(heap->sibling);

    heap_dtor(heap);
}

static void update_heap(heap_t* root, heap_t** heap)
{
    if (!root) return;

    heap_infos[root->ind].heap = heap;

    update_heap(root->child, heap);
    update_heap(root->sibling, heap);
}


int heaps_ctor(heap_t*** heaps, size_t size) 
{
    assert(heaps);
    assert(size);

    *heaps = calloc(size, sizeof(**heaps));

    return EXIT_SUCCESS;
}

int heaps_dtor(heap_t*** heaps, size_t size) 
{
    assert(heaps);

    for (size_t ind = 0; ind < size; ++ind) {
        heap_dtor_recursive((*heaps)[ind]);
    }

    free(*heaps); *heaps = NULL;

    return EXIT_SUCCESS;
}

int heap_ctor(heap_t** node, int64_t key, size_t ind, heap_t* parent, heap_t* child, heap_t* sibling, size_t degree) 
{
    assert(node);

    *node = calloc(1, sizeof(heap_t));

    (*node)->key = key;
    (*node)->ind = ind;
    (*node)->parent = parent;
    (*node)->child = child;
    (*node)->sibling = sibling;
    (*node)->degree = degree;

    return EXIT_SUCCESS;
}

int heap_dtor(heap_t* node) 
{
    free(node);
    return EXIT_SUCCESS;
}

int heap_insert(heap_t** heap, int64_t val, size_t ind) 
{
    assert(heap);

    heap_t* node = NULL;
    ERROR_HANDLE(
        heap_ctor(&node, val, ind, NULL, NULL, NULL, 0)
    );

    heap_infos[ind].node = node;
    heap_infos[ind].heap = heap;

    ERROR_HANDLE(
        heap_trans(&node, heap),
        heap_dtor(node);
    );

    return EXIT_SUCCESS;
}

int heap_trans(heap_t** src, heap_t** dst) 
{
    assert(src);
    assert(dst);

    heap_t* h1 = *src;
    heap_t* h2 = *dst;

    if (!h1) 
    {
        *src = NULL;
        return EXIT_SUCCESS;
    }
    if (!h2) 
    {
        *dst = h1;
        *src = NULL;
        update_heap(*dst, dst);
        return EXIT_SUCCESS;
    }

    heap_t* new_root = NULL;
    heap_t** last_ptr = &new_root;

    while (h1 && h2) 
    {
        if (h1->degree < h2->degree) 
        {
            *last_ptr = h1;
            h1 = h1->sibling;
        } 
        else 
        {
            *last_ptr = h2;
            h2 = h2->sibling;
        }
        last_ptr = &((*last_ptr)->sibling);
    }

    if (h1) 
    {
        *last_ptr = h1;
    }
    else 
    {
        *last_ptr = h2;
    }

    heap_t* prev = NULL;
    heap_t* cur  = new_root;
    heap_t* next = cur->sibling;

    while (next != NULL)
    {
        if (cur->degree != next->degree ||
            (next->sibling && next->sibling->degree == cur->degree))
        {
            prev = cur;
            cur  = next;
            next = next->sibling;
        }
        else
        {
            if (cur->key <= next->key)
            {
                cur->sibling = next->sibling;

                next->parent = cur;
                next->sibling = cur->child;
                cur->child = next;

                cur->degree++;
            }
            else
            {
                if (prev)
                {
                    prev->sibling = next;
                }
                else
                {
                    new_root = next;
                }

                cur->parent = next;
                cur->sibling = next->child;
                next->child = cur;

                next->degree++;

                cur  = next;
            }

            next = cur->sibling;
        }
    }

    *src = NULL;
    *dst = new_root;

    update_heap(*dst, dst);

    return EXIT_SUCCESS;
}


static int decrease_key(heap_t* node, int64_t val) {
    assert(node);
    assert(val <= node->key);

    heap_t* parent_node = node->parent;

    node->key = val;

    
    while (parent_node && node->key < parent_node->key) 
    {
        heap_infos[node->ind].node = parent_node;
        heap_infos[parent_node->ind].node = node;

        swap_int64_t(&node->key, &parent_node->key);
        swap_size_t(&node->ind, &parent_node->ind);

        node = parent_node;
        parent_node = node->parent;
    }

    return EXIT_SUCCESS;
}

static int erase_by_ptr(heap_t** heap, heap_t* node) 
{
    assert(heap);
    assert(node);

    ERROR_HANDLE(decrease_key(node, INT64_MIN));
    ERROR_HANDLE(heap_erase_min(heap));

    return EXIT_SUCCESS;
}

int heap_erase(size_t ind)
{

    ERROR_HANDLE(erase_by_ptr(heap_infos[ind].heap, heap_infos[ind].node));

    return EXIT_SUCCESS;
}

int heap_update(size_t ind, int64_t val) 
{

    heap_t* node = heap_infos[ind].node;

    if (val == node->key) 
    {
        return EXIT_SUCCESS;
    }

    if (val < node->key) {
        ERROR_HANDLE(
            decrease_key(node, val)
        );

        return EXIT_SUCCESS;
    }

    ERROR_HANDLE(erase_by_ptr(heap_infos[ind].heap, node));
    ERROR_HANDLE(heap_insert(heap_infos[ind].heap, val, ind));

    return EXIT_SUCCESS;
}


int heap_get_min(heap_t* heap, int64_t* min) 
{
    assert(heap);

    *min = heap->key;

    for (heap_t* cur_heap = heap->sibling; cur_heap != NULL; cur_heap = cur_heap->sibling) {
        if (cur_heap->key < *min) {
            *min = cur_heap->key;
        }
    }

    return EXIT_SUCCESS;
}

int heap_erase_min(heap_t** heap) 
{
    assert(heap);

    heap_t* h = *heap;

    heap_t* min_prev = NULL;
    heap_t* min_node = h;
    heap_t* prev = NULL;
    heap_t* cur  = h;

    while (cur) 
    {
        if (cur->key < min_node->key || (cur->key == min_node->key && cur->ind < min_node->ind)) 
        {
            min_node = cur;
            min_prev = prev;
        }
        prev = cur;
        cur  = cur->sibling;
    }

    if (min_prev)
    {
        min_prev->sibling = min_node->sibling;
    }
    else
    {
        *heap = min_node->sibling;
    }

    heap_t* child = min_node->child;
    heap_t* rev_children = NULL;

    while (child) {
        heap_t* next = child->sibling;

        child->sibling = rev_children;
        child->parent  = NULL;
        rev_children   = child;

        child = next;
    }

    heap_infos[min_node->ind].node = NULL;

    free(min_node);

    if (rev_children) {
        heap_trans(&rev_children, heap);
    }

    return EXIT_SUCCESS;
}