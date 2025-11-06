#include "b_tree/verification/verification.h"
#include "b_tree/structs.h"

#include <stdio.h>

#define CASE_ENUM_TO_STRING_(error) case error: return #error
const char* b_tree_strerror(const enum BTreeError error)
{
    switch (error)
    {
        CASE_ENUM_TO_STRING_(B_TREE_ERROR_SUCCESS);
        CASE_ENUM_TO_STRING_(B_TREE_ERROR_STANDARD_ERRNO);
        CASE_ENUM_TO_STRING_(B_TREE_ERROR_TREE_IS_NULL);
        CASE_ENUM_TO_STRING_(B_TREE_ERROR_ROOT_IS_NULL);
        CASE_ENUM_TO_STRING_(B_TREE_ERROR_T_IS_ZERO);
        CASE_ENUM_TO_STRING_(B_TREE_ERROR_UNKNOWN);
        CASE_ENUM_TO_STRING_(B_TREE_ERROR_SMALL_NODE_SIZE);
        CASE_ENUM_TO_STRING_(B_TREE_ERROR_BIG_NODE_SIZE);
        CASE_ENUM_TO_STRING_(B_TREE_ERROR_NODE_KEYS_UNSORTED);
        CASE_ENUM_TO_STRING_(B_TREE_ERROR_NODE_IS_NULL);
        CASE_ENUM_TO_STRING_(B_TREE_ERROR_LEAF_WITH_CHILDREN);

        default:
            return "UNKNOWN_B_TREE_ERROR";
    }
    return "UNKNOWN_B_TREE_ERROR";
}
#undef CASE_ENUM_TO_STRING_

static enum BTreeError b_tree_verify_rec_ (const b_tree_node_t* const node, const b_tree_t* const tree);
static enum BTreeError b_tree_node_verify_(const b_tree_node_t* const node, const b_tree_t* const tree);

enum BTreeError b_tree_verify(const b_tree_t* const tree)
{
    if (tree == NULL)
    {
        return B_TREE_ERROR_TREE_IS_NULL;
    }

    if (tree->t <= 0) 
    {
        return B_TREE_ERROR_T_IS_ZERO;
    }

    if (tree->root == NULL) 
    {
        return B_TREE_ERROR_SUCCESS;
    }

    const enum BTreeError rec_veify_error = b_tree_verify_rec_(tree->root, tree);
    if (rec_veify_error) 
    {
        return rec_veify_error;
    }

    return B_TREE_ERROR_SUCCESS;
}

static enum BTreeError b_tree_verify_rec_(const b_tree_node_t* const node, const b_tree_t* const tree) 
{
    const enum BTreeError node_error = b_tree_node_verify_(node, tree);
    if (node_error) 
    {
        return node_error;
    }

    if (node->is_leaf) {
        return B_TREE_ERROR_SUCCESS;
    }
    
    
    for (size_t child_ind = 0; child_ind < node->keys_cnt + 1; ++child_ind) 
    {
        const enum BTreeError child_node_error = b_tree_verify_rec_(node->children[child_ind], tree);
        if (child_node_error) 
        {
            return child_node_error;
        }
    }
    
    return B_TREE_ERROR_SUCCESS;
}

static enum BTreeError b_tree_node_verify_(const b_tree_node_t* const node, const b_tree_t* const tree)
{
    if (node == NULL)
    {
        return B_TREE_ERROR_NODE_IS_NULL;
    }
    
    if (node->is_leaf && node->children) {
        return B_TREE_ERROR_LEAF_WITH_CHILDREN;
    }
    
    if (node->keys_cnt > 2 * tree->t - 1) {
        return B_TREE_ERROR_BIG_NODE_SIZE;
    }
    
    if (tree->root != node && node->keys_cnt < tree->t - 1) {
        return B_TREE_ERROR_SMALL_NODE_SIZE;
    }
    
    if (tree->root == node && !node->is_leaf && node->keys_cnt < 1) {
        return B_TREE_ERROR_SMALL_NODE_SIZE;
    }
    
    for (size_t key_ind = 1; key_ind < node->keys_cnt; ++key_ind)
    {
        if (node->keys[key_ind - 1] > node->keys[key_ind]) {
            return B_TREE_ERROR_NODE_KEYS_UNSORTED;
        }
    }
    

    return B_TREE_ERROR_SUCCESS;
}