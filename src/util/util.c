#include "util.h"
#include <stdlib.h>

void add_space_infront(md_node *node) {
  if (node->data == NULL) {
    printf("Line %d: Node %s does not have any pieces of data",
           node->start_line, print_node_type(node->type));
    return;
  }
  node->data = (char *)realloc(node->data, node->len + 2 * sizeof(char));
  for (int i = node->len + 1; i > -1; i--) {
    node->data[i] = node->data[i - 1];
  }
  node->data[0] = ' ';
  node->len++;
}

void collate_children_text(md_node *node) {
  if (node->first_child == NULL) {
    printf("Line %d: Node %s does not have children to collate",
           node->start_line, print_node_type(node->type));
    return;
  }
  md_node *collate_node = create_empty_md_node(
      NODE_TEXT, node->first_child->start_line, node->last_child->end_line);

  md_node *cur = node->first_child;
  int collate_size = 0;
  for (cur = node->first_child; cur != NULL; cur = cur->next) {
    collate_size += (cur->len + 1);
  }
  // we need to remove the last '\n';
  collate_size--;
  collate_node->len = collate_size;
  collate_node->data = (char *)calloc(collate_size + 1, sizeof(char));
  collate_node->data[0] = '\0';

  collate_size = 0;
  for (cur = node->first_child; cur->next != NULL; cur = cur->next) {
    collate_size += cur->len;
    strncat(collate_node->data, cur->data, cur->len);
    collate_node->data[collate_size] = '\n';
    collate_size++;
    collate_node->data[collate_size] = '\0';
  }
  collate_size += cur->len;
  strncat(collate_node->data, cur->data, cur->len);
  collate_node->data[collate_size] = '\0';

  free_tree(node);
  node->first_child = collate_node;
  node->last_child = collate_node;
  collate_node->parent = node;
}
//
// void add_text_data(md_node *node, const char *line, int line_length,
//                    char delim) {
//
//   md_node *text_node;
//   if (node->first_child == NULL) {
//     text_node = create_empty_md_node(NODE_TEXT);
//     append_to_root(node, text_node);
//   } else {
//     text_node = node->first_child;
//   }
//
//   if (text_node->data == NULL) {
//     text_node->data = (char *)calloc(line_length + 1, sizeof(char));
//     text_node->len = line_length;
//     strncpy(text_node->data, line, line_length + 1);
//   } else {
//     text_node->data = (char *)realloc(
//         text_node->data, line_length + text_node->len + 2 * sizeof(char));
//
//     text_node->data[text_node->len] = delim;
//     text_node->data[text_node->len + 1] = '\0';
//     strncat(text_node->data, line, line_length + 1);
//     // 1 more character than usual since the '\n'
//     text_node->len += (line_length + 1);
//   }
// }

char *clean_escaped_characters(char *text, int text_length) {
  char *result = (char *)calloc(text_length + 1, sizeof(char));
  int i, j;

  for (i = 0, j = 0; i < text_length; i++) {
    if (i < text_length - 1 && text[i] == '\\') {
      result[j] = text[i + 1];
      i++;
    } else {
      result[j] = text[i];
    }
    j++;
  }
  result[j] = '\0';
  result = (char *)realloc(result, (j + 1) * sizeof(char));

  return result;
}
