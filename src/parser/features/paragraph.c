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

void set_paragraph_data(md_node *node, const char *line, int line_length) {
  md_node *text_node;
  if (node->first_child == NULL) {
    text_node = create_empty_md_node(NODE_TEXT);
    append_to_root(node, text_node);
  } else {
    text_node = node->first_child;
  }

  if (text_node->data == NULL) {
    printf("copying\n");
    text_node->data = (char *)calloc(line_length + 1, sizeof(char));
    text_node->len = line_length;
    strncpy(text_node->data, line, line_length + 1);
  } else {
    printf("concatenating\n");
    text_node->data = (char *)realloc(
        text_node->data, line_length + text_node->len + 2 * sizeof(char));

    text_node->data[text_node->len] = ' ';
    text_node->data[text_node->len + 1] = '\0';
    strncat(text_node->data, line, line_length + 1);
    // 1 more character than usual since the '\n'
    text_node->len += (line_length + 1);
  }
  printf("code issue:\n");
  printf("check null: %d\n", text_node->data[text_node->len]);
}

void process_paragraph_data(md_node *node) {
  char *text_data = node->first_child->data;
  int text_length = node->first_child->len;

  // now we reconstruct the tree based on the data
  int i, start, end;
  md_node *new_child_node;
  char *temp_text;
  int temp_length;
  start = 0;
  for (i = 0; i < text_length; i++) {
    switch (text_data[i]) {
    case '*':
      if (i > 0 && text_data[i - 1] != '\\') {
        continue;
      }

      if ((i == 0 || (i > 0 && (text_data[i - 1] == ' '))) &&
          (i < text_length - 1 && text_data[i + 1] != ' ')) {

        end = i - 1;
        temp_length = end - start + 1;
        temp_text = (char *)calloc(temp_length + 1, sizeof(char));
        strncpy(temp_text, &text_data[start], temp_length);
        temp_text[temp_length] = '\0';
        set_paragraph_data(node, temp_text, temp_length);

        // add new emphasis node
        start = i + 1;

      } else if ((i > 0 && text_data[i - 1] != ' ') &&
                 ((i < text_length - 1 && text_data[i + 1] == ' ') ||
                  i == text_length - 1)) {
        end = i - 1;
        // copy the text data
        new_child_node = create_empty_md_node(NODE_EMPH);
        append_to_root(node, new_child_node);
        temp_length = end - start + 1;
        temp_text = (char *)calloc(temp_length + 1, sizeof(char));
        strncpy(temp_text, &text_data[start], temp_length);
        temp_text[temp_length] = '\0';
        set_paragraph_data(node->last_child, temp_text, temp_length);
      }
      break;
    default:
      break;
    }
  }
  end = text_length - 1;
  if (start != end) {
    temp_length = end - start + 1;
    temp_text = (char *)calloc(temp_length + 1, sizeof(char));
    strncpy(temp_text, &text_data[start], temp_length);
    temp_text[temp_length] = '\0';
    set_paragraph_data(node, temp_text, temp_length);
  }
}
