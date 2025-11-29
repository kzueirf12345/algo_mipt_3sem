#include <endian.h>
#include <assert.h>
#include <stdlib.h>
#include <errno.h>
#include <stdio.h>
#include <stdbool.h>
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

#define INPUT_STR_SIZE_MAX 21

typedef struct AVLNode 
{
    char key[INPUT_STR_SIZE_MAX];
    char val[INPUT_STR_SIZE_MAX];

    size_t h;
    ssize_t diff_h;

    struct AVLNode* left;
    struct AVLNode* right;
} avl_node_t;

void avl_insert(avl_node_t** node_ptr, char* key, char* val);
void avl_erase(avl_node_t** node_ptr, char* key);
void avl_find(avl_node_t** node_ptr, char* key, char** res);

void avl_dtor(avl_node_t** node_ptr);

int main() {

    char command[INPUT_STR_SIZE_MAX] = {};

    avl_node_t* root = NULL;

    while (true) 
    {
        if (scanf("%s", command) != 1) 
        {
            break;
        }

        if (strcmp(command, "insert") == 0) 
        {
            char key[INPUT_STR_SIZE_MAX] = {};
            char val[INPUT_STR_SIZE_MAX] = {};

            if (scanf("%s %s", key, val) != 2) 
            {
                fprintf(stderr, "Can't scanf insert input\n");
                return EXIT_FAILURE;
            }

            avl_insert(&root, key, val);
        } 
        else if (strcmp(command, "erase") == 0) 
        {
            char key[INPUT_STR_SIZE_MAX] = {};

            if (scanf("%s", key) != 1) 
            {
                fprintf(stderr, "Can't scanf erase input\n");
                return EXIT_FAILURE;
            }

            avl_erase(&root, key);
        } 
        else if (strcmp(command, "find") == 0) 
        {
            char key[INPUT_STR_SIZE_MAX] = {};

            if (scanf("%s", key) != 1) 
            {
                fprintf(stderr, "Can't scanf find input\n");
                return EXIT_FAILURE;
            }

            char* found_val = NULL;

            avl_find(&root, key, &found_val);

            if (!found_val) 
            {
                printf("MISSING\n");
            } 
            else 
            {
                printf("%s\n", found_val);
            }
        } 
        else 
        {
            fprintf(stderr, "Unknown command\n");
            return EXIT_FAILURE;
        }
    }

    avl_dtor(&root);

    return EXIT_SUCCESS;
}

static inline void avl_update_height(avl_node_t* node)
{
    size_t lh = node->left  ? node->left->h  : 0;
    size_t rh = node->right ? node->right->h : 0;

    node->h = (lh > rh ? lh : rh) + 1;
    node->diff_h = (ssize_t)lh - (ssize_t)rh;
}
static inline void avl_small_left_rotate(avl_node_t** node_ptr)
{
    avl_node_t* a = *node_ptr;
    avl_node_t* b = a->right;

    a->right = b->left;
    b->left = a;

    avl_update_height(a);
    avl_update_height(b);

    *node_ptr = b;
}

static inline void avl_small_right_rotate(avl_node_t** node_ptr)
{
    avl_node_t* a = *node_ptr;
    avl_node_t* b = a->left;

    a->left = b->right;
    b->right = a;

    avl_update_height(a);
    avl_update_height(b);

    *node_ptr = b;
}

static inline void avl_big_left_rotate(avl_node_t** node_ptr)
{
    avl_small_right_rotate(&(*node_ptr)->right);
    avl_small_left_rotate(node_ptr);
}

static inline void avl_big_right_rotate(avl_node_t** node_ptr)
{
    avl_small_left_rotate(&(*node_ptr)->left);
    avl_small_right_rotate(node_ptr);
}


static inline void avl_balance(avl_node_t** node_ptr)
{
    avl_node_t* n = *node_ptr;
    if (!n) return;

    avl_update_height(n);

    if (n->diff_h == 2)
    {
        if (n->left->diff_h < 0)
            avl_big_right_rotate(node_ptr);
        else
            avl_small_right_rotate(node_ptr);
    }
    else if (n->diff_h == -2)
    {
        if (n->right->diff_h > 0)
            avl_big_left_rotate(node_ptr);
        else
            avl_small_left_rotate(node_ptr);
    }
}


void avl_insert(avl_node_t** node_ptr, char* key, char* val) 
{
    assert(node_ptr);
    assert(key);
    assert(val);

    avl_node_t* const node = *node_ptr;

    if (!node) 
    {
        *node_ptr = calloc(1, sizeof(**node_ptr));

        if (!*node_ptr) 
        {
            fprintf(stderr, "Can't calloc new node\n");
            return;    
        }

        memcpy((*node_ptr)->key, key, INPUT_STR_SIZE_MAX - 1);
        (*node_ptr)->key[INPUT_STR_SIZE_MAX - 1] = '\0';

        memcpy((*node_ptr)->val, val, INPUT_STR_SIZE_MAX - 1);
        (*node_ptr)->val[INPUT_STR_SIZE_MAX - 1] = '\0';

        (*node_ptr)->h = 1;

        return;
    }

    int cmp = strcmp(key, node->key);

    if (cmp < 0) {
        avl_insert(&node->left, key, val);
        avl_balance(node_ptr);
        return;
    } 
    
    if (cmp > 0)
    {
        avl_insert(&node->right, key, val);
        avl_balance(node_ptr);
        return;
    } 

    memcpy(node->val, val, INPUT_STR_SIZE_MAX - 1);
    node->val[INPUT_STR_SIZE_MAX - 1] = '\0';
    
    return;
}

static inline avl_node_t* avl_remove_min(avl_node_t** node_ptr)
{
    avl_node_t* n = *node_ptr;

    if (n->left == NULL)
    {
        *node_ptr = n->right;
        return n;
    } 

    avl_node_t* res = avl_remove_min(&n->left);
    avl_balance(node_ptr);
    return res;
}

void avl_erase(avl_node_t** node_ptr, char* key)
{
    avl_node_t* n = *node_ptr;
    if (!n) return;

    int cmp = strcmp(key, n->key);

    if (cmp < 0)
        avl_erase(&n->left, key);
    else if (cmp > 0)
        avl_erase(&n->right, key);
    else
    {
        if (!n->left)
        {
            avl_node_t* r = n->right;
            free(n);
            *node_ptr = r;
            return;
        }
        else if (!n->right)
        {
            avl_node_t* l = n->left;
            free(n);
            *node_ptr = l;
            return;
        }

        avl_node_t* min = avl_remove_min(&n->right);

        memcpy(n->key, min->key, INPUT_STR_SIZE_MAX);
        memcpy(n->val, min->val, INPUT_STR_SIZE_MAX);

        free(min);
    }

    avl_balance(node_ptr);
}

void avl_find(avl_node_t** node_ptr, char* key, char** res) 
{
    assert(node_ptr);
    assert(key);
    assert(res);

    avl_node_t* cur = *node_ptr;

    while (cur) 
    {
        int cmp = strcmp(key, cur->key);
        if (cmp < 0) 
        {
            cur = cur->left;
        } 
        else if (cmp > 0) 
        {
            cur = cur->right;
        } 
        else 
        {
            *res = cur->val;
            return;
        }
    }

    *res = NULL;
    return;
}

void avl_dtor(avl_node_t** node_ptr) 
{
    assert(node_ptr);

    avl_node_t* const node = *node_ptr;

    if (!node) 
    {
        return;
    }

    avl_dtor(&node->left);
    avl_dtor(&node->right);

    free(node);
    
    *node_ptr = NULL;
    
    return;
}
