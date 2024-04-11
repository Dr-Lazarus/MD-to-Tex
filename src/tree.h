#ifndef NODE_H
#define NODE_H
#include "node.h"
#endif

md_node *init_tree(void);
void print_tree_data(md_node *node, int indent_level);
void append_to_root(md_node *root, md_node *new_child);
md_node *remove_last_child(md_node *node);
void link_children(md_node *dest, md_node *source);
void free_tree(md_node *root);
