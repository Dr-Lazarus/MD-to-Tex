#include "codeblock.h"
#include <stdio.h>
#include <string.h>

int is_codeblock_indicator(const char *line, int line_length) {
  if (line_length < 3) {
    return 0;
  }
  return strstr(line, "```") != NULL && line[0] == '`';
}

void set_code_language(md_node *current_node, const char *line,
                       int line_length) {
  if (line_length <= 3) {
    current_node->code_language = NULL;
    return;
  }

  current_node->code_language = (char *)calloc(line_length - 2, sizeof(char));
  line += 3;
  printf("%s\n", line);
  strncpy(current_node->code_language, line, line_length - 2);
}

void set_code_data(md_node *node, const char *line, int line_length) {
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

    text_node->data[text_node->len] = '\n';
    text_node->data[text_node->len + 1] = '\0';
    strncat(text_node->data, line, line_length + 1);
    // 1 more character than usual since the '\n'
    text_node->len += (line_length + 1);
  }
  printf("code issue:\n");
  printf("check null: %d\n", text_node->data[text_node->len]);
}
