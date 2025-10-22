#ifndef B_TREE_SRC_B_TREE_B_TREE_STRUCTS_H
#define B_TREE_SRC_B_TREE_B_TREE_STRUCTS_H

#include <stdlib.h>
#include <stdbool.h>

typedef struct BTreeNode {
    bool is_leaf;

    size_t keys_cnt;

    int* keys;
    struct BTreeNode** children;
} b_tree_node_t;

typedef struct BTree {
    size_t t;

    b_tree_node_t* root;
} b_tree_t;

#endif /*B_TREE_SRC_B_TREE_B_TREE_STRUCTS_H*/