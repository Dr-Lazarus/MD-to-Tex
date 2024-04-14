#ifndef NODE_H
#define NODE_H
#include "../../node.h"
#endif
#ifndef TREE_H
#define TREE_H
#include "../../tree.h"
#endif
#ifndef UTIL_H
#define UTIL_H
#include "../../util/util.h"
#endif

void set_paragraph_data(md_node *node, const char *line, int line_length,
                        int line_number);

char *clean_escaped_characters(char *text, int text_length);
void process_paragraph_data(md_node *node);

void set_text_data(md_node *node, const char *text, int text_length,
                   int line_number);

void parse_new_paragraph_line(md_node *paragraph_node);
