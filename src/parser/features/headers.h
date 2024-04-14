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

int is_header(const char *line, int line_length);
int get_header_level(const char *line, int line_length);
void set_header_data(md_node *node, const char *line, int line_length);
