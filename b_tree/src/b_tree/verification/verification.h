#ifndef HASH_TABLE_SRC_B_TREE_VERIFICATION_H
#define HASH_TABLE_SRC_B_TREE_VERIFICATION_H

#include <assert.h>

#include "b_tree/structs.h"

enum BTreeError
{
    B_TREE_ERROR_SUCCESS                 = 0,
    B_TREE_ERROR_STANDARD_ERRNO          = 1,
    B_TREE_ERROR_TREE_IS_NULL            = 2,
    B_TREE_ERROR_UNKNOWN                 = 32,
};
static_assert(B_TREE_ERROR_SUCCESS == 0, "");

const char* b_tree_strerror(const enum BTreeError error);

#define B_TREE_ERROR_HANDLE(call_func, ...)                                                         \
    do {                                                                                            \
        enum BTreeError error_handler = call_func;                                                  \
        if (error_handler)                                                                          \
        {                                                                                           \
            fprintf(stderr, "Can't " #call_func". Error: %s\n",                                     \
                            b_tree_strerror(error_handler));                                        \
            __VA_ARGS__                                                                             \
            return error_handler;                                                                   \
        }                                                                                           \
    } while(0)

#define B_TREE_INT_ERROR_HANDLE(call_func, ...)                                                     \
    do {                                                                                            \
        enum BTreeError error_handler = call_func;                                                  \
        if (error_handler)                                                                          \
        {                                                                                           \
            fprintf(stderr, "Can't " #call_func". Error: %s\n",                                     \
                            b_tree_strerror(error_handler));                                        \
            __VA_ARGS__                                                                             \
            return (int)error_handler;                                                              \
        }                                                                                           \
    } while(0)


enum BTreeError b_tree_verify(const b_tree_t* const tree);

#ifndef NDEBUG

// TODO DUMB
#define B_TREE_VERIFY_ASSERT(tree)                                                                  \
        do {                                                                                        \
            const enum BTreeError error = b_tree_verify(tree);                                      \
            if (error)                                                                              \
            {                                                                                       \
                lassert(false, "BTree error: %s", b_tree_strerror(error));                          \
            }                                                                                       \
        } while(0)

#else /*NDEBUG*/

#define B_TREE_VERIFY_ASSERT(map) do {} while(0)

#endif /*NDEBUG*/

#endif /*HASH_TABLE_SRC_B_TREE_VERIFICATION_H*/