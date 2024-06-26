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

int is_codeblock_indicator(const char *line, int line_length);
void set_code_language(md_node *current_node, const char *line,
                       int line_length);
void set_code_data(md_node *node, const char *line, int line_length);
