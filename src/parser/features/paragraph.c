#include "paragraph.h"
#include <stdio.h>
#include <string.h>

void parse_paragraph_line(md_node *current_node, const char *line,
                          int line_length) {
  md_node *text_node = create_empty_md_node(NODE_TEXT);
  text_node->data = (char *)calloc(line_length + 1, sizeof(char));
  text_node->len = line_length;
  strncpy(text_node->data, line, line_length + 1);
  append_to_root(current_node, text_node);

  printf("check null: %d\n", text_node->data[text_node->len]);
}
