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

int is_blockquote(const char *line, int line_length);
char *strip_blockquote(const char *text, int text_length);
