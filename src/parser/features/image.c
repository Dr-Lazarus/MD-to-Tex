#include "image.h"
#include <regex.h>
#include <stdio.h>

int is_image_link(const char *line, int line_length) {

  regex_t image_regex;
  int value;
  // Creation of regEx
  // we are opinionated and need the space
  value = regcomp(&image_regex, "^!\\[(.*)\\]\\((.+)\\)$", REG_EXTENDED);
  if (value != 0) {
    printf("regex didn't compile\n");
  }
  value = regexec(&image_regex, line, 0, NULL, 0);

  return (value == 0);
}

void set_image_link(md_node *node, const char *line, int line_length) {

  int i, start, end;
  end = -1;
  for (i = 0; i < line_length; i++) {
    switch (line[i]) {
    case '(':
      start = (i + 1);
      break;
    case ')':
      end = (i - 1);
      break;
    default:
      break;
    }
    // early exiting
    if (end != -1) {
      break;
    }
  }
  node->url_length = end - start + 1;
  node->url = (char *)calloc(node->url_length + 1, sizeof(char));
  strncpy(node->url, &line[start], node->url_length);
  node->url[node->url_length] = '\0';
}

void set_image_caption(md_node *node, const char *line, int line_length) {
  int i, start, end;
  end = -1;
  for (i = 0; i < line_length; i++) {
    switch (line[i]) {
    case '[':
      start = (i + 1);
      break;
    case ']':
      end = (i - 1);
      break;
    default:
      break;
    }
    // early exiting
    if (end != -1) {
      break;
    }
  }
  node->title_length = end - start + 1;
  node->title = (char *)calloc(node->title_length + 1, sizeof(char));
  strncpy(node->title, &line[start], node->title_length);
  node->title[node->title_length] = '\0';
}
