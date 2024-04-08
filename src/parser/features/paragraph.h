#ifndef NODE_H
#define NODE_H
#include "../node.h"
#endif
#ifndef TREE_H
#define TREE_H
#include "../tree.h"
#endif

void parse_paragraph_line(md_node *current_node, const char *line,
                          int line_length);
