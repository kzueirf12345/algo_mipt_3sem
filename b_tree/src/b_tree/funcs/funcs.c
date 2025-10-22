#include "b_tree/funcs/funcs.h"
#include "b_tree/structs.h"
#include "b_tree/verification/verification.h"
#include "utils/utils.h"
#include "logger/src/logger.h"

static enum BTreeError b_tree_dtor_rec_(b_tree_node_t* node);

enum BTreeError b_tree_ctor(b_tree_t* tree, size_t t) 
{
    lassert(!is_invalid_ptr(tree), "");
    lassert(t > 0, "");

    tree->t = t;
    tree->root = NULL;

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

    free(node); IF_DEBUG(node = NULL;)

    for (size_t child_ind = 0; child_ind < node->keys_cnt + 1; ++child_ind) 
    {
        B_TREE_ERROR_HANDLE(b_tree_dtor_rec_(node->children[child_ind]));
    }

    return B_TREE_ERROR_SUCCESS;
}