#ifndef NODE_H
#define NODE_H
#include "../node.h"
#endif
#ifndef TREE_H
#define TREE_H
#include "../tree.h"
#endif

void collate_children_text(md_node *node);
void add_space_infront(md_node *node);
char *clean_escaped_characters(char *text, int text_length);
