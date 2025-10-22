#include "b_tree/verification/verification.h"

#define CASE_ENUM_TO_STRING_(error) case error: return #error
const char* b_tree_strerror(const enum BTreeError error)
{
    switch (error)
    {
        CASE_ENUM_TO_STRING_(B_TREE_ERROR_SUCCESS);
        CASE_ENUM_TO_STRING_(B_TREE_ERROR_STANDARD_ERRNO);
        CASE_ENUM_TO_STRING_(B_TREE_ERROR_TREE_IS_NULL);
        CASE_ENUM_TO_STRING_(B_TREE_ERROR_UNKNOWN);
        default:
            return "UNKNOWN_B_TREE_ERROR";
    }
    return "UNKNOWN_B_TREE_ERROR";
}
#undef CASE_ENUM_TO_STRING_

// TODO implement
enum BTreeError b_tree_verify(const b_tree_t* const tree)
{
    if (tree == NULL)
    {
        return B_TREE_ERROR_TREE_IS_NULL;
    }

    return B_TREE_ERROR_SUCCESS;
}