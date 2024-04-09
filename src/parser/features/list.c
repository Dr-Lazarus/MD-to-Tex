#include "list.h"

int is_list_item(const char *line, int line_length) {}

void set_item_data(md_node *node, const char *line, int line_length) {
  md_node *text_node;
  if (node->first_child == NULL) {
    text_node = create_empty_md_node(NODE_TEXT);
    append_to_root(node, text_node);
  } else {
    text_node = node->first_child;
  }

  if (text_node->data == NULL) {
    text_node->data = (char *)calloc(line_length + 1, sizeof(char));
    text_node->len = line_length;
    strncpy(text_node->data, line, line_length + 1);
  } else {
    text_node->data = (char *)realloc(
        text_node->data, line_length + text_node->len + 2 * sizeof(char));

    text_node->data[text_node->len] = ' ';
    text_node->data[text_node->len + 1] = '\0';
    strncat(text_node->data, line, line_length + 1);
    // 1 more character than usual since the '\n'
    text_node->len += (line_length + 1);
  }
}

void process_list_data(md_node *node) {}
