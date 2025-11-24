#include <endian.h>
#include <inttypes.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

static void swap_size_t(size_t* a, size_t* b)
{
    size_t temp = *a;
    *a = *b;
    *b = temp;
}

static void swap_int64_t(int64_t* a, int64_t* b)
{
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
    struct Heap** owner;
} heap_t;

typedef struct heapInfo {
    heap_t* node;
} heap_info_t;

#define MAX_HEAP_INFOS 1000005
static heap_info_t heap_infos[MAX_HEAP_INFOS];

int heaps_ctor(heap_t*** heaps, size_t size);
int heaps_dtor(heap_t** heaps, size_t size);
int heap_ctor(heap_t** node, int64_t key, size_t ind, heap_t* parent, 
              heap_t* child, heap_t* sibling, size_t degree);
int heap_dtor(heap_t* node);
int heap_trans(heap_t** src, heap_t** dst);
int heap_insert(heap_t** heap, int64_t val, size_t ind);
int heap_erase(size_t ind);
int heap_update(size_t ind, int64_t val);
int heap_get_min(heap_t* heap, int64_t* min);
static int heap_erase_min(heap_t** heap);

int heaps_ctor(heap_t*** heaps, size_t size)
{
    *heaps = calloc(size, sizeof(**heaps));
    return 0;
}

int heaps_dtor(heap_t** heaps, size_t size)
{   
    for (size_t i = 0; i < size; ++i) {
        heap_t* root = (heaps)[i];
        if (!root) continue;
        
        heap_t* cur = root;
        while (cur) {
            if (cur->child) {
                heap_t* child_heap = cur->child;
                heaps_dtor(&child_heap, 1);
            }
            heap_t* next = cur->sibling;
            free(cur);
            cur = next;
        }
    }
    return 0;
}

int heap_ctor(heap_t** node, int64_t key, size_t ind, heap_t* parent, 
              heap_t* child, heap_t* sibling, size_t degree)
{
    *node = calloc(1, sizeof(heap_t));
    (*node)->key = key;
    (*node)->ind = ind;
    (*node)->parent = parent;
    (*node)->child = child;
    (*node)->sibling = sibling;
    (*node)->degree = degree;
    (*node)->owner = NULL;
    return 0;
}

int heap_dtor(heap_t* node)
{
    free(node);
    return 0;
}

static inline heap_t** get_owner_from_node(heap_t* node)
{
    while (node && node->parent) node = node->parent;
    return node ? node->owner : NULL;
}

static void set_owner_for_roots(heap_t* root_list, heap_t** owner_slot)
{
    heap_t* cur = root_list;
    while (cur) {
        cur->owner = owner_slot;
        cur = cur->sibling;
    }
}

int heap_trans(heap_t** src, heap_t** dst)
{
    heap_t* h1 = *src;
    heap_t* h2 = *dst;
    
    if (!h1) {
        *src = NULL;
        return 0;
    }
    
    if (!h2) {
        *dst = h1;
        *src = NULL;
        set_owner_for_roots(*dst, dst);
        return 0;
    }
    
    heap_t* new_root = NULL;
    heap_t** last_ptr = &new_root;
    
    while (h1 && h2) {
        if (h1->degree < h2->degree) {
            *last_ptr = h1;
            h1 = h1->sibling;
        } else {
            *last_ptr = h2;
            h2 = h2->sibling;
        }
        last_ptr = &((*last_ptr)->sibling);
    }
    
    if (h1) *last_ptr = h1;
    else *last_ptr = h2;
    
    heap_t* prev = NULL;
    heap_t* cur = new_root;
    heap_t* next = cur ? cur->sibling : NULL;
    
    while (cur && next != NULL) {
        if (cur->degree != next->degree || 
            (next->sibling && next->sibling->degree == cur->degree)) {
            prev = cur;
            cur = next;
            next = next->sibling;
        } else {
            if (cur->key <= next->key) {
                cur->sibling = next->sibling;
                next->parent = cur;
                next->sibling = cur->child;
                cur->child = next;
                cur->degree++;
            } else {
                if (prev) prev->sibling = next;
                else new_root = next;
                cur->parent = next;
                cur->sibling = next->child;
                next->child = cur;
                next->degree++;
                cur = next;
            }
            next = cur->sibling;
        }
    }
    
    *src = NULL;
    *dst = new_root;
    set_owner_for_roots(*dst, dst);
    return 0;
}

int heap_insert(heap_t** heap, int64_t val, size_t ind)
{
    heap_t* node = NULL;
    heap_ctor(&node, val, ind, NULL, NULL, NULL, 0);
    heap_infos[ind].node = node;
    heap_trans(&node, heap);
    return 0;
}

static int decrease_key(heap_t* node, int64_t val)
{
    heap_t* parent_node = node->parent;
    node->key = val;
    
    while (parent_node && node->key < parent_node->key) {
        heap_infos[node->ind].node = parent_node;
        heap_infos[parent_node->ind].node = node;
        
        swap_int64_t(&node->key, &parent_node->key);
        swap_size_t(&node->ind, &parent_node->ind);
        node = parent_node;
        parent_node = node->parent;
    }
    return 0;
}

static int heap_erase_min(heap_t** heap)
{
    heap_t* h = *heap;
    heap_t* min_prev = NULL;
    heap_t* min_node = h;
    heap_t* prev = NULL;
    heap_t* cur = h;
    
    while (cur) {
        if (cur->key < min_node->key || 
            (cur->key == min_node->key && cur->ind < min_node->ind)) {
            min_node = cur;
            min_prev = prev;
        }
        prev = cur;
        cur = cur->sibling;
    }
    
    if (min_prev) min_prev->sibling = min_node->sibling;
    else *heap = min_node->sibling;
    
    heap_t* child = min_node->child;
    heap_t* rev_children = NULL;
    
    while (child) {
        heap_t* next = child->sibling;
        child->sibling = rev_children;
        child->parent = NULL;
        rev_children = child;
        child = next;
    }
    
    heap_infos[min_node->ind].node = NULL;
    
    free(min_node);
    
    if (rev_children) heap_trans(&rev_children, heap);
    return 0;
}

static int erase_by_ptr(heap_t** heap, heap_t* node)
{
    decrease_key(node, INT64_MIN);
    heap_erase_min(heap);
    return 0;
}

int heap_erase(size_t ind)
{
    heap_t* node = heap_infos[ind].node;
    
    heap_t** owner = get_owner_from_node(node);
    
    erase_by_ptr(owner, node);
    return 0;
}

int heap_update(size_t ind, int64_t val)
{
    heap_t* node = heap_infos[ind].node;
    
    if (val == node->key) return 0;
    
    if (val < node->key) {
        decrease_key(node, val);
        return 0;
    }
    
    heap_t** owner = get_owner_from_node(node);
    
    erase_by_ptr(owner, node);
    heap_insert(owner, val, ind);
    return 0;
}

int heap_get_min(heap_t* heap, int64_t* min)
{
    *min = heap->key;
    for (heap_t* cur_heap = heap->sibling; cur_heap != NULL; cur_heap = cur_heap->sibling) {
        if (cur_heap->key < *min) *min = cur_heap->key;
    }
    return 0;
}

int main()
{
    size_t heaps_size;
    if (scanf("%lu", &heaps_size) != 1) return 0;
    
    heap_t** heaps = NULL;
    heaps_ctor(&heaps, heaps_size);
    
    size_t commands_cnt;
    if (scanf("%lu", &commands_cnt) != 1) {
        heaps_dtor(heaps, heaps_size);
        return 0;
    }
    
    size_t global_ind = 1;
    
    for (size_t command_num = 0; command_num < commands_cnt; ++command_num) {
        int command_type;
        if (scanf("%d", &command_type) != 1) break;
        
        if (command_type == COMMAND_INSERT) {
            size_t heap_num;
            int64_t val;
            if (scanf("%lu %ld", &heap_num, &val) == 2) {
                if (heap_num > 0 && heap_num <= heaps_size && global_ind < MAX_HEAP_INFOS) {
                    --heap_num;
                    heap_insert(&heaps[heap_num], val, global_ind);
                    ++global_ind;
                }
            }
        } else if (command_type == COMMAND_TRANS_HEAP) {
            size_t src_num, dst_num;
            if (scanf("%lu %lu", &src_num, &dst_num) == 2) {
                if (src_num > 0 && src_num <= heaps_size && 
                    dst_num > 0 && dst_num <= heaps_size) {
                    --src_num; --dst_num;
                    heap_trans(&heaps[src_num], &heaps[dst_num]);
                }
            }
        } else if (command_type == COMMAND_ERASE) {
            size_t ind;
            if (scanf("%lu", &ind) == 1) {
                heap_erase(ind);
            }
        } else if (command_type == COMMAND_UPDATE) {
            size_t ind;
            int64_t val;
            if (scanf("%lu %ld", &ind, &val) == 2) {
                heap_update(ind, val);
            }
        } else if (command_type == COMMAND_PRINT_MIN) {
            size_t heap_num;
            if (scanf("%lu", &heap_num) == 1) {
                if (heap_num > 0 && heap_num <= heaps_size && heaps[heap_num - 1]) {
                    int64_t min;
                    heap_get_min(heaps[heap_num - 1], &min);
                    printf("%ld\n", min);
                }
            }
        } else if (command_type == COMMAND_ERASE_MIN) {
            size_t heap_num;
            if (scanf("%lu", &heap_num) == 1) {
                if (heap_num > 0 && heap_num <= heaps_size && heaps[heap_num - 1]) {
                    heap_erase_min(&heaps[heap_num - 1]);
                }
            }
        }
    }
    
    heaps_dtor(heaps, heaps_size);
    free(heaps);
    return 0;
}