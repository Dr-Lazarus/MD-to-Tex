#ifndef NODE_H
#define NODE_H
#include "../../node.h"
#endif
#ifndef TREE_H
#define TREE_H
#include "../../tree.h"
#endif

int is_image_link(const char *line, int line_length);
void set_image_link(md_node *node, const char *line, int line_length);
void set_image_caption(md_node *node, const char *line, int line_length);
void find_and_set_image(const char *line, int line_length, char **output,
                        int *len, char delim_start, char delim_end);
