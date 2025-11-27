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

enum NodeColor {
    NODE_COLOR_BLACK    = 0,
    NODE_COLOR_RED      = 1
};

typedef struct RBTNode 
{
    char key[INPUT_STR_SIZE_MAX];
    char val[INPUT_STR_SIZE_MAX];

    enum NodeColor color;

    struct RBTNode* left;
    struct RBTNode* right;
    struct RBTNode* parent;
} rbt_node_t;

void rbt_insert(rbt_node_t** node_ptr, char* key, char* val);
void rbt_erase(rbt_node_t** node_ptr, char* key);
void rbt_find(rbt_node_t** node_ptr, char* key, char** res);

void rbt_dtor(rbt_node_t** node_ptr);

int main() {

    char command[INPUT_STR_SIZE_MAX] = {};

    rbt_node_t* root = NULL;

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

            rbt_insert(&root, key, val);
        } 
        else if (strcmp(command, "erase") == 0) 
        {
            char key[INPUT_STR_SIZE_MAX] = {};

            if (scanf("%s", key) != 1) 
            {
                fprintf(stderr, "Can't scanf erase input\n");
                return EXIT_FAILURE;
            }

            rbt_erase(&root, key);
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

            rbt_find(&root, key, &found_val);

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

    rbt_dtor(&root);

    return EXIT_SUCCESS;
}

static inline void rbt_small_left_rotate(rbt_node_t* node)
{
    rbt_node_t* a = node;
    if (!a) return;
    rbt_node_t* b = a->right;
    if (!b) return;

    rbt_node_t* parent = a->parent;
    
    a->right = b->left;
    if (b->left) 
    {
        b->left->parent = a;
    }
    
    b->left = a;
    a->parent = b;
    
    b->parent = parent;
    
    if (parent) {
        if (parent->left == a) {
            parent->left = b;
        } else {
            parent->right = b;
        }
    }
}

static inline void rbt_small_right_rotate(rbt_node_t* node)
{
    rbt_node_t* a = node;
    if (!a) return;
    rbt_node_t* b = a->left;
    if (!b) return;

    rbt_node_t* parent = a->parent;
    
    a->left = b->right;
    if (b->right) 
    {
        b->right->parent = a;
    }
    
    b->right = a;
    a->parent = b;
    
    b->parent = parent;
    
    if (parent) {
        if (parent->left == a) {
            parent->left = b;
        } else {
            parent->right = b;
        }
    }
    
}

static inline void rbt_fix_insertion(rbt_node_t* node)
{
    while (node->parent && node->parent->color == NODE_COLOR_RED) 
    {
        rbt_node_t* parent = node->parent;
        rbt_node_t* grandparent = parent->parent;
        
        if (parent == grandparent->left) 
        {
            rbt_node_t* uncle = grandparent->right;
            
            if (uncle && uncle->color == NODE_COLOR_RED)
            {
                parent->color = NODE_COLOR_BLACK;
                grandparent->color = NODE_COLOR_RED;
                uncle->color = NODE_COLOR_BLACK;
                node = grandparent;
            }
            else 
            {
                if (node == parent->right) 
                {
                    node = parent;
                    rbt_small_left_rotate(node);
                    parent = node->parent;
                    grandparent = parent->parent;
                }
                parent->color = NODE_COLOR_BLACK;
                grandparent->color = NODE_COLOR_RED;
                rbt_small_right_rotate(grandparent);
            }
        }
        else 
        {
            rbt_node_t* uncle = grandparent->left;
            
            if (uncle && uncle->color == NODE_COLOR_RED)
            {
                parent->color = NODE_COLOR_BLACK;
                grandparent->color = NODE_COLOR_RED;
                uncle->color = NODE_COLOR_BLACK;
                node = grandparent;
            }
            else 
            {
                if (node == parent->left) 
                {
                    node = parent;
                    rbt_small_right_rotate(node);
                    parent = node->parent;
                    grandparent = parent->parent;
                }
                parent->color = NODE_COLOR_BLACK;
                grandparent->color = NODE_COLOR_RED;
                rbt_small_left_rotate(grandparent);
            }
        }
    }
}

void rbt_insert(rbt_node_t** node_ptr, char* key, char* val) 
{
    assert(node_ptr);
    assert(key);
    assert(val);

    rbt_node_t* node = *node_ptr;

    rbt_node_t* insertion_node = calloc(1, sizeof(rbt_node_t));
    insertion_node->color = NODE_COLOR_RED;
    memcpy(insertion_node->key, key, INPUT_STR_SIZE_MAX - 1);
    insertion_node->key[INPUT_STR_SIZE_MAX - 1] = '\0';
    memcpy(insertion_node->val, val, INPUT_STR_SIZE_MAX - 1);
    insertion_node->val[INPUT_STR_SIZE_MAX - 1] = '\0';

    if (!node) {
        insertion_node->color = NODE_COLOR_BLACK;
        *node_ptr = insertion_node;
        return;
    }

    rbt_node_t* cur = NULL;

    int cmp = 0;

    while (node) 
    {
        cur = node;
        cmp = strncmp(key, node->key, INPUT_STR_SIZE_MAX - 1);

        if (cmp > 0)
        {
            node = node->right;            
        }
        else if (cmp < 0)
        {
            node = node->left;
        }
        else 
        {
            memcpy(node->val, val, INPUT_STR_SIZE_MAX - 1);
            node->val[INPUT_STR_SIZE_MAX - 1] = '\0';
            free(insertion_node);
            return;
        }
    }
    
    insertion_node->parent = cur;

    if (cmp > 0)
    {
        cur->right = insertion_node;       
    }
    else if (cmp < 0)
    {
        cur->left = insertion_node;   
    }

    rbt_fix_insertion(insertion_node);

    rbt_node_t* root = insertion_node;
    while (root->parent) 
    {
        root = root->parent;
    }
    *node_ptr = root;
    (*node_ptr)->color = NODE_COLOR_BLACK;

    return;
}

static void rbt_fix_erase(rbt_node_t* node)
{
    while (node->parent && node->color == NODE_COLOR_BLACK)
    {
        if (node == node->parent->left)
        {
            rbt_node_t* s = node->parent->right;

            if (s->color == NODE_COLOR_RED)
            {
                s->color = NODE_COLOR_BLACK;
                node->parent->color = NODE_COLOR_RED;
                rbt_small_left_rotate(node->parent);
                s = node->parent->right;
            }

            if ((!s->left  || s->left->color  == NODE_COLOR_BLACK) &&
                (!s->right || s->right->color == NODE_COLOR_BLACK))
            {
                s->color = NODE_COLOR_RED;
                node = node->parent;
            }
            else
            {
                if (!s->right || s->right->color == NODE_COLOR_BLACK)
                {
                    if (s->left) s->left->color = NODE_COLOR_BLACK;
                    s->color = NODE_COLOR_RED;
                    rbt_small_right_rotate(s);
                    s = node->parent->right;
                }

                s->color = node->parent->color;
                node->parent->color = NODE_COLOR_BLACK;
                if (s->right) s->right->color = NODE_COLOR_BLACK;
                rbt_small_left_rotate(node->parent);
                break;
            }
        }
        else
        {
            rbt_node_t* s = node->parent->left;

            if (s->color == NODE_COLOR_RED)
            {
                s->color = NODE_COLOR_BLACK;
                node->parent->color = NODE_COLOR_RED;
                rbt_small_right_rotate(node->parent);
                s = node->parent->left;
            }

            if ((!s->left  || s->left->color  == NODE_COLOR_BLACK) &&
                (!s->right || s->right->color == NODE_COLOR_BLACK))
            {
                s->color = NODE_COLOR_RED;
                node = node->parent;
            }
            else
            {
                if (!s->left || s->left->color == NODE_COLOR_BLACK)
                {
                    if (s->right) s->right->color = NODE_COLOR_BLACK;
                    s->color = NODE_COLOR_RED;
                    rbt_small_left_rotate(s);
                    s = node->parent->left;
                }

                s->color = node->parent->color;
                node->parent->color = NODE_COLOR_BLACK;
                if (s->left) s->left->color = NODE_COLOR_BLACK;
                rbt_small_right_rotate(node->parent);
                break;
            }
        }
    }

    node->color = NODE_COLOR_BLACK;
}

static rbt_node_t* rbt_min(rbt_node_t* node)
{
    while (node->left) node = node->left;
    return node;
}

void rbt_erase(rbt_node_t** node_ptr, char* key)
{
    rbt_node_t* root = *node_ptr;
    rbt_node_t* node = root;

    int cmp = -1;

    while (node)
    {
        cmp = strncmp(key, node->key, INPUT_STR_SIZE_MAX - 1);

        if      (cmp > 0) node = node->right;
        else if (cmp < 0) node = node->left;
        else break;
    }

    if (!node) return;

    rbt_node_t* y = node;
    rbt_node_t* x;
    enum NodeColor yOriginalColor = y->color;

    if (!node->left)
    {
        x = node->right;
        if (x) 
        {
            x->parent = node->parent;
        }

        if (!node->parent)                      *node_ptr           = x;
        else if (node == node->parent->left)    node->parent->left  = x;
        else                                    node->parent->right = x;
    }
    else if (!node->right)
    {
        x = node->left;
        if (x) 
        {
            x->parent = node->parent;
        }

        if (!node->parent)                      *node_ptr           = x;
        else if (node == node->parent->left)    node->parent->left  = x;
        else                                    node->parent->right = x;
    }
    else
    {
        y = rbt_min(node->right);
        yOriginalColor = y->color;
        x = y->right;

        if (x) x->parent = y;

        if (y->parent != node)
        {
            if (y->parent->left == y)   y->parent->left  = x;
            else                        y->parent->right = x;

            y->right = node->right;

            if (y->right) 
            {
                y->right->parent = y;
            }
        }

        if (!node->parent)                      *node_ptr = y;
        else if (node->parent->left == node)    node->parent->left = y;
        else                                    node->parent->right = y;

        y->parent = node->parent;
        y->left = node->left;
        if (y->left) 
        {
            y->left->parent = y;
        }
        y->color = node->color;
    }

    free(node);

    if (yOriginalColor == NODE_COLOR_BLACK)
    {
        if      (x)         rbt_fix_erase(x);
        else if (*node_ptr) rbt_fix_erase(*node_ptr);
    }
}


void rbt_find(rbt_node_t** node_ptr, char* key, char** res) 
{
    assert(node_ptr);
    assert(key);
    assert(res);

    rbt_node_t* cur = *node_ptr;

    while (cur)
    {
        int cmp = strncmp(key, cur->key, INPUT_STR_SIZE_MAX - 1);
        if (cmp > 0)
        {
            cur = cur->right;
        }
        else if (cmp < 0)
        {
            cur = cur->left;
        }
        else
        {
            *res = cur->val;
            return;
        }
    }

    *res = NULL;
}


void rbt_dtor(rbt_node_t** node_ptr) 
{
    assert(node_ptr);

    rbt_node_t* const node = *node_ptr;

    if (!node) 
    {
        return;
    }

    rbt_dtor(&node->left);
    rbt_dtor(&node->right);

    free(node);
    
    *node_ptr = NULL;
    
    return;
}
