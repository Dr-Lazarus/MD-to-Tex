#include "codeblock.h"
#include <stdio.h>
#include <string.h>

int is_codeblock_indicator(const char *line, int line_length)
{
  if (line_length < 3)
  {
    return 0;
  }
  return strstr(line, "```") != NULL && line[0] == '`';
}

void set_code_language(md_node *current_node, const char *line,
                       int line_length)
{
  if (line_length <= 3)
  {
    current_node->code_language = NULL;
    return;
  }

  current_node->code_language = (char *)calloc(line_length - 2, sizeof(char));
  line += 3;
  printf("%s\n", line);
  strncpy(current_node->code_language, line, line_length - 2);
}

// void set_code_data(md_node *node, const char *line, int line_length) {
//   add_text_data(node, line, line_length, '\n');
// }
