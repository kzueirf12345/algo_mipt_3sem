#ifndef B_TREE_SRC_B_TREE_FUNCS_FUNCS_H
#define B_TREE_SRC_B_TREE_FUNCS_FUNCS_H

#include "b_tree/structs.h"
#include "b_tree/verification/verification.h"

enum BTreeError b_tree_ctor(b_tree_t* tree, size_t t);
enum BTreeError b_tree_dtor(b_tree_t* tree);



#endif /*B_TREE_SRC_B_TREE_FUNCS_FUNCS_H*/