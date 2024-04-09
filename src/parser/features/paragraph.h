#ifndef NODE_H
#define NODE_H
#include "../../node.h"
#endif
#ifndef TREE_H
#define TREE_H
#include "../../tree.h"
#endif

void parse_paragraph_line(md_node *current_node, const char *line,
                          int line_length);
void set_paragraph_data(md_node *node, const char *line, int line_length);

void process_paragraph_data(md_node *node);

void set_text_data(md_node *node, const char *text, int text_length);
