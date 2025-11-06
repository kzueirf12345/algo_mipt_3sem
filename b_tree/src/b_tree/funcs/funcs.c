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
static enum BTreeError b_tree_node_insert_to_leaf_ (b_tree_node_t* const node, size_t key_ind, int key);
static enum BTreeError b_tree_insert_rec_(b_tree_node_t* node, b_tree_t* tree, int key);
static enum BTreeError b_tree_split_child_(b_tree_node_t* parent, size_t child_ind, size_t t);

enum BTreeError b_tree_insert(b_tree_t* tree, int key)
{
    B_TREE_VERIFY_ASSERT(tree);

    if (!tree->root) {
        B_TREE_ERROR_HANDLE(b_tree_node_ctor_(&tree->root, tree->t, true));
        tree->root->keys[0] = key;
        tree->root->keys_cnt = 1;
        return B_TREE_ERROR_SUCCESS;
    }

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

static enum BTreeError b_tree_node_insert_to_leaf_(b_tree_node_t* const node, size_t key_ind, int key)
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

//===================================DELETE=========================================================

static enum BTreeError b_tree_delete_from_node(b_tree_node_t* node, b_tree_t* tree, int key);
static enum BTreeError b_tree_delete_from_leaf(b_tree_node_t* node, size_t key_ind);
static enum BTreeError b_tree_delete_from_internal(b_tree_node_t* node, size_t key_ind, b_tree_t* tree);
static enum BTreeError b_tree_merge_children(b_tree_node_t* parent, size_t child_ind);
static enum BTreeError b_tree_borrow_from_left(b_tree_node_t* parent, size_t child_ind);
static enum BTreeError b_tree_borrow_from_right(b_tree_node_t* parent, size_t child_ind);
static enum BTreeError b_tree_fill_child(b_tree_node_t* parent, size_t child_ind, size_t t);
static int b_tree_get_predecessor(b_tree_node_t* node, size_t key_ind);
static int b_tree_get_successor(b_tree_node_t* node, size_t key_ind);

enum BTreeError b_tree_delete(b_tree_t* tree, int key)
{
    B_TREE_VERIFY_ASSERT(tree);
    
    if (!tree->root) {
        return B_TREE_ERROR_SUCCESS;
    }
    
    B_TREE_ERROR_HANDLE(b_tree_delete_from_node(tree->root, tree, key));
    
    if (tree->root->keys_cnt == 0) {
        b_tree_node_t* old_root = tree->root;
        
        if (tree->root->is_leaf) {
            tree->root = NULL;
        } else {
            tree->root = tree->root->children[0];
        }
        
        B_TREE_ERROR_HANDLE(b_tree_node_dtor_(old_root));
    }
    
    B_TREE_VERIFY_ASSERT(tree);
    return B_TREE_ERROR_SUCCESS;
}

static enum BTreeError b_tree_delete_from_node(b_tree_node_t* node, b_tree_t* tree, int key)
{
    lassert(!is_invalid_ptr(node), "");
    
    size_t key_ind = 0;
    while (key_ind < node->keys_cnt && node->keys[key_ind] < key) {
        key_ind++;
    }
    
    if (key_ind < node->keys_cnt && node->keys[key_ind] == key) {
        if (node->is_leaf) {
            return b_tree_delete_from_leaf(node, key_ind);
        } else {
            return b_tree_delete_from_internal(node, key_ind, tree);
        }
    } else {
        if (node->is_leaf) {
            return B_TREE_ERROR_SUCCESS;
        }
        
        bool is_last_child = (key_ind == node->keys_cnt);
        
        if (node->children[key_ind]->keys_cnt < tree->t) {
            B_TREE_ERROR_HANDLE(b_tree_fill_child(node, key_ind, tree->t));
        }
        
        if (is_last_child && key_ind > node->keys_cnt) {
            return b_tree_delete_from_node(node->children[key_ind - 1], tree, key);
        } else {
            return b_tree_delete_from_node(node->children[key_ind], tree, key);
        }
    }
}

static enum BTreeError b_tree_delete_from_leaf(b_tree_node_t* node, size_t key_ind)
{
    lassert(!is_invalid_ptr(node), "");
    lassert(node->is_leaf, "");
    lassert(key_ind < node->keys_cnt, "");
    
    for (size_t i = key_ind; i < node->keys_cnt - 1; i++) {
        node->keys[i] = node->keys[i + 1];
    }
    node->keys_cnt--;
    
    return B_TREE_ERROR_SUCCESS;
}

static enum BTreeError b_tree_delete_from_internal(b_tree_node_t* node, size_t key_ind, b_tree_t* tree)
{
    lassert(!is_invalid_ptr(node), "");
    lassert(!node->is_leaf, "");
    lassert(key_ind < node->keys_cnt, "");
    
    int key = node->keys[key_ind];
    b_tree_node_t* left_child = node->children[key_ind];
    b_tree_node_t* right_child = node->children[key_ind + 1];
    
    if (left_child->keys_cnt >= tree->t) {
        int predecessor = b_tree_get_predecessor(node, key_ind);
        node->keys[key_ind] = predecessor;
        return b_tree_delete_from_node(left_child, tree, predecessor);
    }

    else if (right_child->keys_cnt >= tree->t) {
        int successor = b_tree_get_successor(node, key_ind);
        node->keys[key_ind] = successor;
        return b_tree_delete_from_node(right_child, tree, successor);
    }

    else {
        B_TREE_ERROR_HANDLE(b_tree_merge_children(node, key_ind));
        return b_tree_delete_from_node(left_child, tree, key);
    }
}

static enum BTreeError b_tree_fill_child(b_tree_node_t* parent, size_t child_ind, size_t t)
{
    lassert(!is_invalid_ptr(parent), "");
    lassert(!parent->is_leaf, "");
    lassert(child_ind < parent->keys_cnt + 1, "");

    
    if (child_ind > 0 && parent->children[child_ind - 1]->keys_cnt >= t) {
        B_TREE_ERROR_HANDLE(b_tree_borrow_from_left(parent, child_ind));
    }
    else if (child_ind < parent->keys_cnt && parent->children[child_ind + 1]->keys_cnt >= t) {
        B_TREE_ERROR_HANDLE(b_tree_borrow_from_right(parent, child_ind));
    }
    else {
        if (child_ind > 0) {
            B_TREE_ERROR_HANDLE(b_tree_merge_children(parent, child_ind - 1));
        } else {
            B_TREE_ERROR_HANDLE(b_tree_merge_children(parent, child_ind));
        }
    }
    
    return B_TREE_ERROR_SUCCESS;
}

static enum BTreeError b_tree_merge_children(b_tree_node_t* parent, size_t child_ind)
{
    lassert(!is_invalid_ptr(parent), "");
    lassert(!parent->is_leaf, "");
    lassert(child_ind < parent->keys_cnt, "");
    
    b_tree_node_t* left_child = parent->children[child_ind];
    b_tree_node_t* right_child = parent->children[child_ind + 1];
    
    left_child->keys[left_child->keys_cnt] = parent->keys[child_ind];
    left_child->keys_cnt++;

    for (size_t i = 0; i < right_child->keys_cnt; i++) {
        left_child->keys[left_child->keys_cnt + i] = right_child->keys[i];
    }
    
    if (!left_child->is_leaf) {
        for (size_t i = 0; i <= right_child->keys_cnt; i++) {
            left_child->children[left_child->keys_cnt + i] = right_child->children[i];
        }
    }
    
    left_child->keys_cnt += right_child->keys_cnt;
    
    for (size_t i = child_ind; i < parent->keys_cnt - 1; i++) {
        parent->keys[i] = parent->keys[i + 1];
    }
    
    for (size_t i = child_ind + 1; i < parent->keys_cnt; i++) {
        parent->children[i] = parent->children[i + 1];
    }
    
    parent->keys_cnt--;
    
    B_TREE_ERROR_HANDLE(b_tree_node_dtor_(right_child));
    
    return B_TREE_ERROR_SUCCESS;
}

static enum BTreeError b_tree_borrow_from_left(b_tree_node_t* parent, size_t child_ind)
{
    lassert(!is_invalid_ptr(parent), "");
    lassert(!parent->is_leaf, "");
    lassert(child_ind > 0, "");
    
    b_tree_node_t* child = parent->children[child_ind];
    b_tree_node_t* left_sibling = parent->children[child_ind - 1];
    
    for (size_t i = child->keys_cnt; i > 0; i--) {
        child->keys[i] = child->keys[i - 1];
    }
    
    if (!child->is_leaf) {
        for (size_t i = child->keys_cnt + 1; i > 0; i--) {
            child->children[i] = child->children[i - 1];
        }
    }
    
    child->keys[0] = parent->keys[child_ind - 1];
    child->keys_cnt++;
    
    if (!child->is_leaf) {
        child->children[0] = left_sibling->children[left_sibling->keys_cnt];
    }
    
    parent->keys[child_ind - 1] = left_sibling->keys[left_sibling->keys_cnt - 1];
    
    left_sibling->keys_cnt--;
    
    return B_TREE_ERROR_SUCCESS;
}

static enum BTreeError b_tree_borrow_from_right(b_tree_node_t* parent, size_t child_ind)
{
    lassert(!is_invalid_ptr(parent), "");
    lassert(!parent->is_leaf, "");
    lassert(child_ind < parent->keys_cnt, "");
    
    b_tree_node_t* child = parent->children[child_ind];
    b_tree_node_t* right_sibling = parent->children[child_ind + 1];
    
    child->keys[child->keys_cnt] = parent->keys[child_ind];
    child->keys_cnt++;
    
    if (!child->is_leaf) {
        child->children[child->keys_cnt] = right_sibling->children[0];
    }
    
    parent->keys[child_ind] = right_sibling->keys[0];
    
    for (size_t i = 0; i < right_sibling->keys_cnt - 1; i++) {
        right_sibling->keys[i] = right_sibling->keys[i + 1];
    }
    
    if (!right_sibling->is_leaf) {
        for (size_t i = 0; i < right_sibling->keys_cnt; i++) {
            right_sibling->children[i] = right_sibling->children[i + 1];
        }
    }
    
    right_sibling->keys_cnt--;
    
    return B_TREE_ERROR_SUCCESS;
}

static int b_tree_get_predecessor(b_tree_node_t* node, size_t key_ind)
{
    lassert(!is_invalid_ptr(node), "");
    lassert(!node->is_leaf, "");
    
    b_tree_node_t* current = node->children[key_ind];
    while (!current->is_leaf) {
        current = current->children[current->keys_cnt];
    }
    
    return current->keys[current->keys_cnt - 1];
}

static int b_tree_get_successor(b_tree_node_t* node, size_t key_ind)
{
    lassert(!is_invalid_ptr(node), "");
    lassert(!node->is_leaf, "");
    
    b_tree_node_t* current = node->children[key_ind + 1];
    while (!current->is_leaf) {
        current = current->children[0];
    }
    
    return current->keys[0];
}