#include "paragraph.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void set_text_data(md_node *node, const char *text, int text_length,
                   int line_number) {
  node->data = (char *)calloc(text_length + 1, sizeof(char));
  node->len = text_length;
  strncpy(node->data, text, text_length);
  node->data[node->len] = '\0';
}

char *identify_special_char(char *ptr, char *text, int text_length) {
  int i;
  for (i = ptr - text; i < text_length; i++) {
    if (ptr[0] == '\\') {
      ptr++;
      i++;
    } else if (ptr[0] == '*' || ptr[0] == '_' || ptr[0] == '[' ||
               ptr[0] == '$' || ptr[0] == '`') {
      return ptr;
    }
    ptr++;
  }
  return NULL;
}

char *identify_special_str(char *ptr, char *text, int text_length, char *target,
                           int target_length) {
  int i;
  for (i = ptr - text; i < text_length; i++) {
    if (ptr[0] == '\\') {
      ptr++;
      i++;
    } else if (ptr[0] == target[0]) {
      if (i + target_length - 1 < text_length &&
          strncmp(ptr, target, target_length) == 0) {
        return ptr;
      }
    }
    ptr++;
  }
  return NULL;
}

char *identify_char(char *ptr, char *text, int text_length, char target) {
  int i;
  for (i = ptr - text; i < text_length; i++) {
    if (ptr[0] == '\\') {
      ptr++;
      i++;
    } else if (ptr[0] == target) {
      return ptr;
    }
    ptr++;
  }
  return NULL;
}

void copy_text_to_paragraph(md_node *paragraph_node, char *text, char *start,
                            char *end, int start_line, int end_line) {
  md_node *new_child_node;
  new_child_node =
      create_md_node(NODE_TEXT, start_line, end_line, start - text, end - text);
  new_child_node->len = end - start + 1;
  if (new_child_node->len == 0) {
    free(new_child_node);
    return;
  }
  new_child_node->data = (char *)calloc(new_child_node->len + 1, sizeof(char));
  strncpy(new_child_node->data, start, new_child_node->len);
  new_child_node->data[new_child_node->len] = '\0';
  new_child_node->data =
      clean_escaped_characters(new_child_node->data, new_child_node->len);
  new_child_node->len = strlen(new_child_node->data);
  append_to_root(paragraph_node, new_child_node);
}

void parse_new_paragraph_line(md_node *paragraph_node) {
  /* we take the last child */
  md_node *aborted = remove_last_child(paragraph_node);
  if (aborted == NULL) {
    printf("No last child to parse on paragraph node %d",
           paragraph_node->end_line);
  }

  /* now we need to check how to parse the text; */
  md_node *new_child_node;
  md_node *new_last_child;
  md_node *new_link_node;
  NodeType node_type;
  char *text = aborted->data;
  int text_length = aborted->len;
  int max_length;
  char *store = (char *)calloc(4, sizeof(char));
  char *matching;
  char *close_paren;
  int store_length = 0;
  char *ptr = text;
  char *start = text;
  char *end;

  while ((ptr = identify_special_char(ptr, text, text_length)) != NULL) {
    printf("Line %d: Identified special char %c at postion %ld\n",
           aborted->start_line, ptr[0], ptr - text);
    store_length = 0;
    store[store_length] = '\0';
    switch (ptr[0]) {
      case '`':
      case '$':
      case '*':
      case '_':
        max_length = (ptr[0] == '$' || ptr[0] == '`') ? 1 : 3;
        for (int i = 0; i < max_length; i++) {
          /* check that it has not exceeded */
          if ((&ptr[i]) - text >= text_length) {
            break;
          }

          /* if we are still matching the initial token, we go to next */
          if (ptr[i] == ptr[0]) {
            store[i] = ptr[i];
            store_length++;
          } else {
            break;
          }
        }
        store[store_length] = '\0';

        /* now we try to find the matching */
        matching = identify_special_str(ptr + store_length, text, text_length,
                                        store, store_length);

        if (matching == NULL) {
          printf("Line %d: Matching close bracket not found for %s\n",
                 aborted->start_line, store);
          break;
        }

        end = ptr - 1;
        copy_text_to_paragraph(paragraph_node, text, start, end,
                               aborted->start_line, aborted->end_line);

        switch (store_length) {
          case 1:
            switch (ptr[0]) {
              case '`':
                node_type = NODE_CODE;
                break;
              case '$':
                node_type = NODE_MATH;
                break;
              default:
                node_type = NODE_EMPH;
                break;
            }
            new_child_node = create_md_node(
                node_type, aborted->start_line, aborted->end_line,
                (ptr + store_length) - text, (matching - 1) - text);
            append_to_root(paragraph_node, new_child_node);
            new_last_child = new_child_node;
            break;
          case 2:
            new_child_node = create_md_node(
                NODE_STRONG, aborted->start_line, aborted->end_line,
                (ptr + store_length) - text, (matching - 1) - text);
            append_to_root(paragraph_node, new_child_node);
            new_last_child = new_child_node;
            break;
          case 3:
            new_child_node = create_md_node(
                NODE_EMPH, aborted->start_line, aborted->end_line,
                (ptr + store_length) - text, (matching - 1) - text);
            append_to_root(paragraph_node, new_child_node);
            new_child_node = create_md_node(
                NODE_STRONG, aborted->start_line, aborted->end_line,
                (ptr + store_length) - text, (matching - 1) - text);
            append_to_root(paragraph_node->last_child, new_child_node);
            new_last_child = new_child_node;
            break;
          default:
            break;
        }

        new_child_node =
            create_md_node(NODE_TEXT, aborted->start_line, aborted->end_line,
                           (ptr + store_length) - text, (matching - 1) - text);
        new_child_node->len = (matching - 1) - (ptr + store_length) + 1;
        new_child_node->data =
            (char *)calloc(new_child_node->len + 1, sizeof(char));
        strncpy(new_child_node->data, (ptr + store_length),
                new_child_node->len);
        new_child_node->data[new_child_node->len] = '\0';
        new_child_node->data =
            clean_escaped_characters(new_child_node->data, new_child_node->len);
        new_child_node->len = strlen(new_child_node->data);
        append_to_root(new_last_child, new_child_node);
        ptr = matching + store_length - 1;

        start = matching + store_length;
        break;

      case '[':
        matching = identify_char(ptr, text, text_length, ']');
        if (matching == NULL) {
          printf("Line %d: Matching close bracket not found for %c\n",
                 aborted->start_line, ']');
          break;
        }

        if ((matching + 1)[0] != '(') {
          printf(
              "Line %d: Matching open bracket %c not found for in-line "
              "link\n",
              aborted->start_line, '(');
          break;
        }
        close_paren = identify_char(matching + 1, text, text_length, ')');
        if (close_paren == NULL) {
          printf("Line %d: Matching close bracket not found for %c\n",
                 aborted->start_line, ')');
          break;
        }

        end = ptr - 1;
        copy_text_to_paragraph(paragraph_node, text, start, end,
                               aborted->start_line, aborted->end_line);

        /* set the link node */
        new_link_node =
            create_md_node(NODE_LINK, aborted->start_line, aborted->end_line,
                           ptr - text, close_paren - text);
        append_to_root(paragraph_node, new_link_node);
        new_link_node->url_length = (close_paren - 1) - (matching + 2) + 1;
        new_link_node->url =
            (char *)calloc(new_link_node->url_length, sizeof(char));
        strncpy(new_link_node->url, (matching + 2), new_link_node->url_length);
        new_link_node->url[new_link_node->url_length] = '\0';
        new_link_node->url = clean_escaped_characters(
            new_link_node->url, new_link_node->url_length);
        new_link_node->url_length = strlen(new_link_node->url);

        /* set the alternate text */
        new_child_node =
            create_md_node(NODE_TEXT, aborted->start_line, aborted->end_line,
                           (ptr + 1) - text, (matching - 1) - text);
        new_child_node->len = (matching - 1) - (ptr + 1) + 1;
        new_child_node->data =
            (char *)calloc(new_child_node->len + 1, sizeof(char));
        strncpy(new_child_node->data, (ptr + 1), new_child_node->len);
        new_child_node->data[new_child_node->len] = '\0';
        new_child_node->data =
            clean_escaped_characters(new_child_node->data, new_child_node->len);
        new_child_node->len = strlen(new_child_node->data);
        append_to_root(new_link_node, new_child_node);

        start = close_paren + 1;
        break;
      default:
        printf("Unknown symbol detected in special character detection\n");
        break;
    }
    ptr++;
  }

  /* we need to create a text node to put whatever is left */
  end = text + text_length;
  copy_text_to_paragraph(paragraph_node, text, start, end, aborted->start_line,
                         aborted->end_line);

  return;
}
