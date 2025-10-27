#ifndef B_TREE_SRC_B_B_TREE_DUMB_DUMB_H
#define B_TREE_SRC_B_B_TREE_DUMB_DUMB_H

#include <assert.h>

#include "b_tree/structs.h"
#include "utils/utils.h"

enum BTreeDumbError
{
    B_TREE_DUMB_ERROR_SUCCESS = 0,
    B_TREE_DUMB_ERROR_FAILURE = 1
};
static_assert(B_TREE_DUMB_ERROR_SUCCESS == 0, "");

const char* b_tree_dumb_strerror(const enum BTreeDumbError error);

#define B_TREE_DUMB_ERROR_HANDLE(call_func, ...)                                                      \
    do {                                                                                            \
        enum BTreeDumbError error_handler = call_func;                                               \
        if (error_handler)                                                                          \
        {                                                                                           \
            fprintf(stderr, "Can't " #call_func". Error: %s\n",                                     \
                            b_tree_dumb_strerror(error_handler));                                     \
            __VA_ARGS__                                                                             \
            return error_handler;                                                                   \
        }                                                                                           \
    } while(0)

enum BTreeDumbError b_tree_dumb_ctor(void);
enum BTreeDumbError b_tree_dumb_dtor(void);

enum BTreeDumbError b_tree_dumb_set_out_file(char* const filename);

void b_tree_dumb (const b_tree_t* const tree, elem_to_str_t elem_to_str);

#endif /*B_TREE_SRC_B_B_TREE_DUMB_DUMB_H*/