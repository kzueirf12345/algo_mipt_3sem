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

static rbt_node_t* NIL = NULL;

static void ensure_nil()
{
    if (NIL) return;
    NIL = calloc(1, sizeof(rbt_node_t));
    if (!NIL) {
        fprintf(stderr, "Can't allocate NIL node\n");
        exit(EXIT_FAILURE);
    }
    NIL->color = NODE_COLOR_BLACK;
    NIL->left = NIL->right = NIL->parent = NIL;
    NIL->key[0] = '\0';
    NIL->val[0] = '\0';
}

static rbt_node_t* new_node(const char* key, const char* val)
{
    ensure_nil();
    rbt_node_t* n = calloc(1, sizeof(rbt_node_t));
    if (!n) return NULL;
    memcpy(n->key, key, INPUT_STR_SIZE_MAX - 1);
    n->key[INPUT_STR_SIZE_MAX - 1] = '\0';
    memcpy(n->val, val, INPUT_STR_SIZE_MAX - 1);
    n->val[INPUT_STR_SIZE_MAX - 1] = '\0';
    n->color = NODE_COLOR_RED;
    n->left = NIL;
    n->right = NIL;
    n->parent = NIL;
    return n;
}

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


static inline void rbt_small_left_rotate(rbt_node_t* a)
{
    if (!a || a == NIL) return;
    rbt_node_t* b = a->right;
    if (!b || b == NIL) return;

    rbt_node_t* parent = a->parent;

    a->right = b->left;
    if (b->left != NIL)
    {
        b->left->parent = a;
    }

    b->left = a;
    a->parent = b;

    b->parent = parent;

    if (parent != NIL && parent != NULL) 
    {
        if (parent->left == a) parent->left = b;
        else parent->right = b;
    }
}

static inline void rbt_small_right_rotate(rbt_node_t* a)
{
    if (!a || a == NIL) return;
    rbt_node_t* b = a->left;
    if (!b || b == NIL) return;

    rbt_node_t* parent = a->parent;

    a->left = b->right;
    if (b->right != NIL)
    {
        b->right->parent = a;
    }

    b->right = a;
    a->parent = b;

    b->parent = parent;

    if (parent != NIL && parent != NULL) 
    {
        if (parent->left == a) parent->left = b;
        else parent->right = b;
    }
}

static inline void rbt_fix_insertion(rbt_node_t* node)
{
    ensure_nil();
    while (node->parent != NIL && node->parent->color == NODE_COLOR_RED)
    {
        rbt_node_t* parent = node->parent;
        rbt_node_t* grandparent = parent->parent;

        if (parent == grandparent->left)
        {
            rbt_node_t* uncle = grandparent->right;

            if (uncle != NIL && uncle->color == NODE_COLOR_RED)
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

            if (uncle != NIL && uncle->color == NODE_COLOR_RED)
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

    ensure_nil();

    rbt_node_t* root = *node_ptr;
    if (!root)
    {
        rbt_node_t* node = new_node(key, val);
        if (!node) 
        {
            fprintf(stderr, "Can't allocate node\n");
            return;
        }
        node->color = NODE_COLOR_BLACK;
        node->parent = NIL;
        node->left = NIL;
        node->right = NIL;
        *node_ptr = node;
        return;
    }

    rbt_node_t* cur = root;

    while (cur != NIL)
    {
        int cmp = strncmp(key, cur->key, INPUT_STR_SIZE_MAX - 1);
        if (cmp == 0)
        {
            memcpy(cur->val, val, INPUT_STR_SIZE_MAX - 1);
            cur->val[INPUT_STR_SIZE_MAX - 1] = '\0';
            return;
        }
        else if (cmp < 0)
        {
            if (cur->left == NIL) break;
            cur = cur->left;
        }
        else
        {
            if (cur->right == NIL) break;
            cur = cur->right;
        }
    }

    rbt_node_t* insertion_node = new_node(key, val);
    if (!insertion_node) 
    {
        fprintf(stderr, "Can't allocate node\n");
        return;
    }

    insertion_node->parent = cur;
    if (cur == NIL) 
    {
        *node_ptr = insertion_node;
        insertion_node->color = NODE_COLOR_BLACK;
        insertion_node->parent = NIL;
        return;
    } 
    else 
    {
        int cmp = strncmp(insertion_node->key, cur->key, INPUT_STR_SIZE_MAX - 1);
        if (cmp < 0) cur->left = insertion_node;
        else cur->right = insertion_node;
    }

    rbt_fix_insertion(insertion_node);

    rbt_node_t* top = insertion_node;
    while (top->parent != NIL) top = top->parent;
    *node_ptr = top;
    (*node_ptr)->color = NODE_COLOR_BLACK;
}

static rbt_node_t* rbt_min(rbt_node_t* node)
{
    ensure_nil();
    while (node->left != NIL) node = node->left;
    return node;
}

static void rbt_fix_erase(rbt_node_t* x)
{
    ensure_nil();
    while (x != NIL && x->parent != NIL && x->color == NODE_COLOR_BLACK)
    {
        if (x == x->parent->left)
        {
            rbt_node_t* w = x->parent->right;
            
            if (w->color == NODE_COLOR_RED)
            {
                w->color = NODE_COLOR_BLACK;
                x->parent->color = NODE_COLOR_RED;
                rbt_small_left_rotate(x->parent);
                w = x->parent->right;
            }

            if (w->left->color == NODE_COLOR_BLACK && w->right->color == NODE_COLOR_BLACK)
            {
                w->color = NODE_COLOR_RED;
                x = x->parent;
            }
            else
            {
                if (w->right->color == NODE_COLOR_BLACK)
                {
                    w->left->color = NODE_COLOR_BLACK;
                    w->color = NODE_COLOR_RED;
                    rbt_small_right_rotate(w);
                    w = x->parent->right;
                }
                w->color = x->parent->color;
                x->parent->color = NODE_COLOR_BLACK;
                w->right->color = NODE_COLOR_BLACK;
                rbt_small_left_rotate(x->parent);
                x = NIL;
            }
        }
        else
        {
            rbt_node_t* w = x->parent->left;

            if (w->color == NODE_COLOR_RED)
            {
                w->color = NODE_COLOR_BLACK;
                x->parent->color = NODE_COLOR_RED;
                rbt_small_right_rotate(x->parent);
                w = x->parent->left;
            }

            if (w->right->color == NODE_COLOR_BLACK && w->left->color == NODE_COLOR_BLACK)
            {
                w->color = NODE_COLOR_RED;
                x = x->parent;
            }
            else
            {
                if (w->left->color == NODE_COLOR_BLACK)
                {
                    w->right->color = NODE_COLOR_BLACK;
                    w->color = NODE_COLOR_RED;
                    rbt_small_left_rotate(w);
                    w = x->parent->left;
                }
                w->color = x->parent->color;
                x->parent->color = NODE_COLOR_BLACK;
                w->left->color = NODE_COLOR_BLACK;
                rbt_small_right_rotate(x->parent);
                x = NIL;
            }
        }
    }

    if (x != NIL) x->color = NODE_COLOR_BLACK;
}

void rbt_erase(rbt_node_t** node_ptr, char* key)
{
    assert(node_ptr);
    assert(key);

    ensure_nil();

    rbt_node_t* root = *node_ptr;
    if (!root) return;

    rbt_node_t* z = root;
    while (z != NIL)
    {
        int cmp = strncmp(key, z->key, INPUT_STR_SIZE_MAX - 1);
        if (cmp == 0) break;
        else if (cmp < 0) z = z->left;
        else z = z->right;
    }

    if (z == NIL) return;

    rbt_node_t* y = z;
    enum NodeColor y_original_color = y->color;
    rbt_node_t* x = NIL;

    if (z->left == NIL)
    {
        x = z->right;
        if (z->parent == NIL) 
        {
            *node_ptr = x == NIL ? NULL : x;
        } 
        else if 
        (z == z->parent->left) 
        {
            z->parent->left = z->right;
        } 
        else 
        {
            z->parent->right = z->right;
        }

        if (z->right != NIL) z->right->parent = z->parent;
    }
    else if (z->right == NIL)
    {
        x = z->left;
        if (z->parent == NIL) 
        {
            *node_ptr = x == NIL ? NULL : x;
        } 
        else if (z == z->parent->left) 
        {
            z->parent->left = z->left;
        } 
        else 
        {
            z->parent->right = z->left;
        }

        if (z->left != NIL) z->left->parent = z->parent;
    }
    else
    {
        y = rbt_min(z->right);
        y_original_color = y->color;
        x = y->right;

        if (y->parent == z)
        {
            x->parent = y;
        }
        else
        {
            if (y->parent->left == y) y->parent->left = y->right;
            else                      y->parent->right = y->right;

            if (y->right != NIL) y->right->parent = y->parent;

            y->right = z->right;

            if (y->right != NIL) y->right->parent = y;
        }

        if (z->parent == NIL) 
        {
            *node_ptr = y;
        } 
        else if (z == z->parent->left) 
        {
            z->parent->left = y;
        } 
        else 
        {
            z->parent->right = y;
        }
        y->parent = z->parent;

        y->left = z->left;

        if (y->left != NIL) y->left->parent = y;

        y->color = z->color;
    }

    free(z);

    if (y_original_color == NODE_COLOR_BLACK) 
    {
        rbt_fix_erase(x);
    }

    if (*node_ptr != NULL) 
    {
        while ((*node_ptr)->parent != NIL) 
        {
            *node_ptr = (*node_ptr)->parent;
        }
    }
    
    if (*node_ptr == NIL) 
    {
        *node_ptr = NULL;
    }
}

void rbt_find(rbt_node_t** node_ptr, char* key, char** res)
{
    assert(node_ptr);
    assert(key);
    assert(res);

    ensure_nil();

    rbt_node_t* cur = *node_ptr;
    if (!cur) 
    {
        *res = NULL;
        return;
    }

    while (cur != NIL)
    {
        int cmp = strncmp(key, cur->key, INPUT_STR_SIZE_MAX - 1);
        if (cmp > 0) cur = cur->right;
        else if (cmp < 0) cur = cur->left;
        else 
        {
            *res = cur->val;
            return;
        }
    }

    *res = NULL;
}

static void rbt_free_nodes_recursive(rbt_node_t* node)
{
    if (!node || node == NIL) return;
    rbt_free_nodes_recursive(node->left);
    rbt_free_nodes_recursive(node->right);
    free(node);
}

void rbt_dtor(rbt_node_t** node_ptr)
{
    assert(node_ptr);
    ensure_nil();

    rbt_node_t* root = *node_ptr;
    if (root && root != NIL) {
        rbt_free_nodes_recursive(root);
    }

    if (NIL) {
        free(NIL);
        NIL = NULL;
    }

    *node_ptr = NULL;
}