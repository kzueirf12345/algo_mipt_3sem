#include <assert.h>
#include <stdlib.h>
#include <errno.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <stdint.h>

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

typedef struct SplayNode 
{
    int64_t key;
    int64_t sum;

    struct SplayNode* left;
    struct SplayNode* right;
    struct SplayNode* parent;
} splay_node_t;

void splay_insert(splay_node_t** root, int64_t key);
void splay_dtor(splay_node_t** root);
int64_t splay_sum(splay_node_t** root, int64_t left, int64_t right);

int main() 
{
    int64_t prev_res = 0;
    char prev_command = 0;
    char command = 0;

    splay_node_t* root = NULL;

    size_t query_cnt = 0;
    if (scanf("%zu", &query_cnt) != 1)
    {
        fprintf(stderr, "Can't scanf query_cnt\n");
        return EXIT_FAILURE;
    }

    for (size_t i = 0; i < query_cnt; ++i)
    {
        if (scanf(" %c", &command) != 1)
        {
            fprintf(stderr, "Can't scanf command\n");
            return EXIT_FAILURE;
        }

        if (command == '?') 
        {
            int64_t min = 0;
            int64_t max = 0;

            if (scanf("%ld %ld", &min, &max) != 2) 
            {
                fprintf(stderr, "Can't scanf ? input\n");
                return EXIT_FAILURE;
            }

            prev_res = splay_sum(&root, min, max);

            printf("%ld\n", prev_res);
        } 
        else if (command == '+' && prev_command != '?') 
        {
            int64_t key = 0;

            if (scanf("%ld", &key) != 1) 
            {
                fprintf(stderr, "Can't scanf key input\n");
                return EXIT_FAILURE;
            }

            splay_insert(&root, key);
        } 
        else if (command == '+' && prev_command == '?') 
        {
            int64_t key = 0;

            if (scanf("%ld", &key) != 1) 
            {
                fprintf(stderr, "Can't scanf key input\n");
                return EXIT_FAILURE;
            }

            key = (prev_res + (int64_t)key) % 1000000000ll;
            splay_insert(&root, key);
        } 
        else 
        {
            fprintf(stderr, "Unknown command\n");
            return EXIT_FAILURE;
        }

        prev_command = command;
    }

    splay_dtor(&root);

    return EXIT_SUCCESS;
}

static void splay_update(splay_node_t* node)
{
    if (!node) return;
    node->sum = node->key;
    if (node->left) node->sum += node->left->sum;
    if (node->right) node->sum += node->right->sum;
}

static void splay_left_rotate(splay_node_t* node)
{
    if (!node) return;

    splay_node_t* parent = node->parent;

    if (!parent) return;

    splay_node_t* grandparent = parent->parent;

    parent->right = node->left;
    if (node->left)
    {
        node->left->parent = parent;
    }

    node->left = parent;
    parent->parent = node;

    node->parent = grandparent;
    if (grandparent)
    {
        if      (grandparent->left  == parent) grandparent->left = node;
        else if (grandparent->right == parent) grandparent->right = node;
    }

    splay_update(parent);
    splay_update(node);
}

static void splay_right_rotate(splay_node_t* node)
{
    if (!node) return;

    splay_node_t* parent = node->parent;

    if (!parent) return;

    splay_node_t* grandparent = parent->parent;

    parent->left = node->right;
    if (node->right)
    {
        node->right->parent = parent;
    }

    node->right = parent;
    parent->parent = node;

    node->parent = grandparent;
    if (grandparent)
    {
        if      (grandparent->left  == parent) grandparent->left = node;
        else if (grandparent->right == parent) grandparent->right = node;
    }

    splay_update(parent);
    splay_update(node);
}

static splay_node_t* splay_splay(splay_node_t* node)
{
    if (!node) return NULL;

    while (node->parent)
    {
        splay_node_t* parent = node->parent;
        splay_node_t* grandparent = parent->parent;

        if (!grandparent)
        {
            if (parent->left == node) splay_right_rotate(node);
            else                      splay_left_rotate (node);
        }
        else if (grandparent->left == parent && parent->left == node)
        {
            splay_right_rotate(parent);
            splay_right_rotate(node);
        }
        else if (grandparent->right == parent && parent->right == node)
        {
            splay_left_rotate(parent);
            splay_left_rotate(node);
        }
        else if (grandparent->left == parent && parent->right == node)
        {
            splay_left_rotate(node);
            splay_right_rotate(node);
        }
        else
        {
            splay_right_rotate(node);
            splay_left_rotate(node);
        }
    }

    return node;
}

static splay_node_t* splay_find(splay_node_t* root, int64_t key)
{
    splay_node_t* node = root;
    splay_node_t* last = NULL;

    while (node)
    {
        last = node;
        if      (key < node->key) node = node->left;
        else if (key > node->key) node = node->right;
        else break;
    }

    return last;
}

void splay_insert(splay_node_t** root, int64_t key)
{
    if (!*root)
    {
        *root = calloc(1, sizeof(splay_node_t));
        (*root)->key = key;
        (*root)->sum = key;
        (*root)->parent = (*root)->left = (*root)->right = NULL;
        return;
    }

    splay_node_t* found = splay_find(*root, key);
    *root = splay_splay(found);
    
    if ((*root)->key == key) return;
    
    splay_node_t* new_node = calloc(1, sizeof(splay_node_t));
    new_node->key = key;
    new_node->sum = key;
    
    if (key < (*root)->key)
    {
        new_node->left = (*root)->left;
        if (new_node->left) new_node->left->parent = new_node;
        
        new_node->right = *root;
        (*root)->parent = new_node;
        
        (*root)->left = NULL;
        splay_update(*root);
    }
    else
    {
        new_node->right = (*root)->right;
        if (new_node->right) new_node->right->parent = new_node;
        
        new_node->left = *root;
        (*root)->parent = new_node;
        
        (*root)->right = NULL;
        splay_update(*root);
    }
    
    splay_update(new_node);
    *root = new_node;
}

static void splay_split(splay_node_t* root, int64_t key, splay_node_t** left, splay_node_t** right)
{
    if (!root)
    {
        *left = *right = NULL;
        return;
    }

    splay_node_t* found = splay_find(root, key);
    root = splay_splay(found);

    if (root->key <= key)
    {
        *left = root;
        *right = root->right;
        if (root->right) 
        {
            root->right->parent = NULL;
            root->right = NULL;
        }
        splay_update(*left);
    }
    else
    {
        *right = root;
        *left = root->left;
        if (root->left) 
        {
            root->left->parent = NULL;
            root->left = NULL;
        }
        splay_update(*right);
    }
    
}

static splay_node_t* splay_merge(splay_node_t* left, splay_node_t* right)
{
    if (!left) return right;
    if (!right) return left;

    splay_node_t* max_left = left;
    while (max_left->right) max_left = max_left->right;
    left = splay_splay(max_left);

    left->right = right;
    right->parent = left;
    splay_update(left);

    return left;
}

int64_t splay_sum(splay_node_t** root, int64_t left_key, int64_t right_key)
{
    if (!*root) return 0;
    
    splay_node_t* left, *mid_right;
    splay_split(*root, left_key - 1, &left, &mid_right);

    splay_node_t* mid, *right;
    splay_split(mid_right, right_key, &mid, &right);

    int64_t res = mid ? mid->sum : 0;

    *root = splay_merge(splay_merge(left, mid), right);

    return res;
}

void splay_dtor(splay_node_t** root)
{
    if (!*root) return;
    splay_dtor(&(*root)->left);
    splay_dtor(&(*root)->right);
    free(*root);
    *root = NULL;
}