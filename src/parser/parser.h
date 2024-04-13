#ifndef NODE_H
#define NODE_H
#include "../node.h"
#endif

char *read_source_code(const char *filename);

void parse_line(md_node *root, const char *line, int line_length,
                int line_number, int debugging);

md_node *parse_source(char *file_name, int debugging);

LineType get_line_type(const char *line, int line_length);
