#include "b_tree/funcs/funcs.h"
#include "b_tree/structs.h"
#include "b_tree/verification/verification.h"
#include "utils/utils.h"
#include "logger/src/logger.h"
//===================================CTOR DTOR======================================================

static enum BTreeError b_tree_node_ctor_(b_tree_node_t** node, size_t t, bool is_leaf);
static enum BTreeError b_tree_node_dtor_(b_tree_node_t* node);
static enum BTreeError b_tree_dtor_rec_ (b_tree_node_t* node);

enum BTreeError b_tree_ctor(b_tree_t* tree, size_t t) 
{
    lassert(!is_invalid_ptr(tree), "");
    lassert(t > 0, "");

    tree->t = t;
    B_TREE_ERROR_HANDLE(b_tree_node_ctor_(&tree->root, t, true));

    B_TREE_VERIFY_ASSERT(tree);

    return B_TREE_ERROR_SUCCESS;
}

enum BTreeError b_tree_dtor(b_tree_t* tree) 
{
    B_TREE_VERIFY_ASSERT(tree);

    B_TREE_ERROR_HANDLE(b_tree_dtor_rec_(tree->root));

    IF_DEBUG(tree->root = NULL;)
    IF_DEBUG(tree->t    = POISON;)
    
    return B_TREE_ERROR_SUCCESS;
}

static enum BTreeError b_tree_dtor_rec_(b_tree_node_t* node) 
{
    if (!node) 
    {
        return B_TREE_ERROR_SUCCESS;
    }

    lassert(!is_invalid_ptr(node), "");

    if (!node->is_leaf)
    {
        for (size_t child_ind = 0; child_ind < node->keys_cnt + 1; ++child_ind) 
        {
            B_TREE_ERROR_HANDLE(b_tree_dtor_rec_(node->children[child_ind]));
        }
    }


    B_TREE_ERROR_HANDLE(b_tree_node_dtor_(node));

    return B_TREE_ERROR_SUCCESS;
}

static enum BTreeError b_tree_node_ctor_(b_tree_node_t** node, size_t t, bool is_leaf)
{
    lassert(!is_invalid_ptr(node), "");
    lassert(t > 0, "");

    *node = calloc(1, sizeof(**node));

    if (!(*node)) {
        fprintf(stderr, "Can't calloc node\n");
        return B_TREE_ERROR_STANDARD_ERRNO;
    }
    
    (*node)->is_leaf = is_leaf;
    (*node)->keys_cnt = 0;
    
    const size_t max_keys_cnt = 2 * t - 1;
    (*node)->keys = calloc(max_keys_cnt, sizeof(*(*node)->keys));

    if (!(*node)->keys) {
        fprintf(stderr, "Can't calloc node->keys\n");
        return B_TREE_ERROR_STANDARD_ERRNO;
    }

    if (!is_leaf)
    {
        const size_t max_children_cnt = max_keys_cnt + 1;
        (*node)->children = calloc(max_children_cnt, sizeof(*(*node)->children));
        
        if (!(*node)->children) {
            fprintf(stderr, "Can't calloc node->children\n");
            return B_TREE_ERROR_STANDARD_ERRNO;
        }
    } 
    else 
    {
        (*node)->children = NULL;
    }


    return B_TREE_ERROR_SUCCESS;
}

static enum BTreeError b_tree_node_dtor_(b_tree_node_t* node) 
{
    lassert(!is_invalid_ptr(node), "");

    free(node->keys);     IF_DEBUG(node->keys     = NULL;)
    free(node->children); IF_DEBUG(node->children = NULL;)

    IF_DEBUG(node->keys_cnt = POISON;)

    free(node); IF_DEBUG(node = NULL;)

    return B_TREE_ERROR_SUCCESS;
}

//===================================INSERT=========================================================

static bool b_tree_node_is_full_(const b_tree_node_t* const node, size_t t);
static bool b_tree_node_insert_to_leaf_ (b_tree_node_t* const node, size_t key_ind, int key);
static enum BTreeError b_tree_insert_rec_(b_tree_node_t* node, b_tree_t* tree, int key);
static enum BTreeError b_tree_split_child_(b_tree_node_t* parent, size_t child_ind, size_t t);

enum BTreeError b_tree_insert(b_tree_t* tree, int key)
{
    B_TREE_VERIFY_ASSERT(tree);

    if (b_tree_node_is_full_(tree->root, tree->t))
    {
        b_tree_node_t* new_root = NULL;
        B_TREE_ERROR_HANDLE(b_tree_node_ctor_(&new_root, tree->t, false));
        new_root->children[0] = tree->root;
        tree->root = new_root;

        B_TREE_ERROR_HANDLE(b_tree_split_child_(new_root, 0, tree->t));
    }

    B_TREE_ERROR_HANDLE(b_tree_insert_rec_(tree->root, tree, key));

    B_TREE_VERIFY_ASSERT(tree);

    return B_TREE_ERROR_SUCCESS;
}

enum BTreeError b_tree_insert_rec_(b_tree_node_t* node, b_tree_t* tree, int key)
{
    B_TREE_VERIFY_ASSERT(tree);
    lassert(!is_invalid_ptr(node), "");

    if (node->is_leaf)
    {
        size_t key_ind = 0;
        while (key_ind < node->keys_cnt && node->keys[key_ind] < key)
            key_ind++;

        B_TREE_ERROR_HANDLE(b_tree_node_insert_to_leaf_(node, key_ind, key));
    }
    else
    {
        size_t child_ind = 0;
        while (child_ind < node->keys_cnt && key > node->keys[child_ind])
            child_ind++;

        if (b_tree_node_is_full_(node->children[child_ind], tree->t))
        {
            B_TREE_ERROR_HANDLE(b_tree_split_child_(node, child_ind, tree->t));

            if (key > node->keys[child_ind])
                child_ind++;
        }

        B_TREE_ERROR_HANDLE(b_tree_insert_rec_(node->children[child_ind], tree, key));
    }

    B_TREE_VERIFY_ASSERT(tree);
    return B_TREE_ERROR_SUCCESS;
}

static bool b_tree_node_is_full_(const b_tree_node_t* const node, size_t t)
{
    lassert(!is_invalid_ptr(node), "");
    lassert(t > 0, "");

    return node->keys_cnt >= 2 * t - 1;
}

static bool b_tree_node_insert_to_leaf_(b_tree_node_t* const node, size_t key_ind, int key)
{
    lassert(!is_invalid_ptr(node), "");
    lassert(node->is_leaf, "");

    for (size_t shift_ind = node->keys_cnt; shift_ind > key_ind; --shift_ind)
    {
        node->keys[shift_ind] = node->keys[shift_ind - 1];
    }
    node->keys[key_ind] = key;
    ++node->keys_cnt;

    return B_TREE_ERROR_SUCCESS;
}

static enum BTreeError b_tree_split_child_(b_tree_node_t* parent, size_t child_ind, size_t t)
{
    lassert(!is_invalid_ptr(parent), "");

    b_tree_node_t* full_child = parent->children[child_ind];
    lassert(full_child->keys_cnt == 2 * t - 1, "");

    b_tree_node_t* new_child = NULL;
    B_TREE_ERROR_HANDLE(b_tree_node_ctor_(&new_child, t, full_child->is_leaf));

    new_child->keys_cnt = t - 1;

    for (size_t key_ind = 0; key_ind < new_child->keys_cnt; ++key_ind) {
        new_child->keys[key_ind] = full_child->keys[key_ind + t];
    }

    if (!full_child->is_leaf)
    {
        for (size_t child_ptr_ind = 0; child_ptr_ind < new_child->keys_cnt + 1; ++child_ptr_ind) {
            new_child->children[child_ptr_ind] = full_child->children[child_ptr_ind + t];
        }
    }
    full_child->keys_cnt = t - 1;

    for (size_t shift_ind = parent->keys_cnt; shift_ind > child_ind; --shift_ind)
        parent->children[shift_ind + 1] = parent->children[shift_ind];
    parent->children[child_ind + 1] = new_child;


    for (size_t shift_ind = parent->keys_cnt; shift_ind > child_ind; --shift_ind)
        parent->keys[shift_ind] = parent->keys[shift_ind - 1];
    parent->keys[child_ind] = full_child->keys[t - 1];

    parent->keys_cnt++;

    return B_TREE_ERROR_SUCCESS;
}