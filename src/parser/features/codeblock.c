#include <stdio.h>
#include <string.h>
#include "codeblock.h"

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
  if (node->data == NULL) {
    node->data = (char *)calloc(line_length + 1, sizeof(char));
    node->len = line_length;
    strncpy(node->data, line, line_length + 1);
  } else {
    node->data =
        (char *)realloc(node->data, line_length + node->len + 2 * sizeof(char));

    node->data[node->len] = '\n';
    node->data[node->len + 1] = '\0';
    strncat(node->data, line, line_length + 1);
    // 1 more character than usual since the '\n'
    node->len += (line_length + 1);
  }
  printf("code issue:\n");
  printf("check null: %d\n", node->data[node->len]);
}
