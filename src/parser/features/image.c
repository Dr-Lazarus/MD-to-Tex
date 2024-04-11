#include "image.h"
#include <regex.h>
#include <stdio.h>

int is_image_link(const char *line, int line_length) {

  regex_t image_regex;
  int value;
  // Creation of regEx
  // we are opinionated and need the space
  value = regcomp(&image_regex, "^!\\[(.*)\\]\\((.+)\\)\n$", REG_EXTENDED);
  if (value != 0) {
    printf("regex didn't compile\n");
  }
  value = regexec(&image_regex, line, 0, NULL, 0);

  return (value == 0);
}

void find_and_set_image(const char *line, int line_length, char **output,
                        int *len, char delim_start, char delim_end) {
  int i, start, end;
  end = -1;
  for (i = 0; i < line_length; i++) {
    if (line[i] == delim_start) {
      start = (i + 1);
    } else if (line[i] == delim_end) {
      end = (i - 1);
    }
    if (end != -1) {
      break;
    }
  }
  *len = end - start + 1;
  *output = (char *)calloc(*len + 1, sizeof(char));
  strncpy(*output, &line[start], *len);
  (*output)[*len] = '\0';
}
void set_image_link(md_node *node, const char *line, int line_length) {
  printf("set image\n");
  find_and_set_image(line, line_length, &(node->url), &(node->url_length), '(',
                     ')');
  printf("set image\n");
}

void set_image_caption(md_node *node, const char *line, int line_length) {
  printf("set image\n");
  find_and_set_image(line, line_length, &(node->title), &(node->title_length),
                     '[', ']');
  printf("set image\n");
}
