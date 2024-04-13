#ifndef NODE_H
#define NODE_H
#include "../../node.h"
#endif
#ifndef TREE_H
#define TREE_H
#include "../../tree.h"
#endif

#define INDENT_SPACE 2

int is_list_item(const char *line, int line_length);
void set_item_data(md_node *node, const char *line, int line_length);
void process_list_data(md_node *node);
ListType get_list_type(const char *line, int line_length);
