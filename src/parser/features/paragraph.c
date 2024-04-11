#include "paragraph.h"
#include <stdio.h>
#include <string.h>

void set_paragraph_data(md_node *node, const char *line, int line_length)
{
  add_text_data(node, line, line_length, ' ');
}

void set_text_data(md_node *node, const char *text, int text_length)
{
  md_node *text_node;
  text_node = create_empty_md_node(NODE_TEXT);
  append_to_root(node, text_node);
  text_node->data = (char *)calloc(text_length + 1, sizeof(char));
  text_node->len = text_length;
  strncpy(text_node->data, text, text_length + 1);
}

void process_paragraph_data(md_node *node)
{
  char *text_data = node->first_child->data;
  int text_length = node->first_child->len;
  node->first_child = NULL;
  node->last_child = NULL;

  // now we reconstruct the tree based on the data
  int i, start, end;
  md_node *new_child_node;
  char *temp_text;
  char *matching;
  char *close_link;
  int temp_length;
  start = 0;
  for (i = 0; i < text_length; i++)
  {
    switch (text_data[i])
    {
    case '*':
      if (i > 0 && text_data[i - 1] == '\\')
      {
        break;
      }

      if ((i == 0) || ((i > 0 && text_data[i - 1] == ' ') &&
                       (i < text_length - 1 && text_data[i + 1] != ' ')))
      {

        end = i - 1;
        temp_length = end - start + 1;
        temp_text = (char *)calloc(temp_length + 1, sizeof(char));
        strncpy(temp_text, &text_data[start], temp_length);
        temp_text[temp_length] = '\0';
        set_text_data(node, temp_text, temp_length);

        // add new emphasis node
        start = i + 1;
      }
      else if (((i > 0 && text_data[i - 1] != ' ') &&
                (i < text_length - 1 && text_data[i + 1] == ' ')) ||
               (i == text_length - 1))
      {
        end = i - 1;
        // copy the text data
        new_child_node = create_empty_md_node(NODE_STRONG);
        append_to_root(node, new_child_node);
        temp_length = end - start + 1;
        temp_text = (char *)calloc(temp_length + 1, sizeof(char));
        strncpy(temp_text, &text_data[start], temp_length);
        temp_text[temp_length] = '\0';
        set_text_data(node->last_child, temp_text, temp_length);
        start = i + 1;
      }
      break;
    case '_':
      if (i > 0 && text_data[i - 1] == '\\')
      {
        continue;
      }

      if ((i == 0) || ((i > 0 && text_data[i - 1] == ' ') &&
                       (i < text_length - 1 && text_data[i + 1] != ' ')))
      {

        end = i - 1;
        temp_length = end - start + 1;
        temp_text = (char *)calloc(temp_length + 1, sizeof(char));
        strncpy(temp_text, &text_data[start], temp_length);
        temp_text[temp_length] = '\0';
        set_text_data(node, temp_text, temp_length);

        // add new emphasis node
        start = i + 1;
      }
      else if (((i > 0 && text_data[i - 1] != ' ') &&
                (i < text_length - 1 && text_data[i + 1] == ' ')) ||
               (i == text_length - 1))
      {
        end = i - 1;
        // copy the text data
        new_child_node = create_empty_md_node(NODE_EMPH);
        append_to_root(node, new_child_node);
        temp_length = end - start + 1;
        temp_text = (char *)calloc(temp_length + 1, sizeof(char));
        strncpy(temp_text, &text_data[start], temp_length);
        temp_text[temp_length] = '\0';
        set_text_data(node->last_child, temp_text, temp_length);
        start = i + 1;
      }
      break;
    case '[':
      // we now want to find the matching
      matching = strstr(&text_data[i], "]");
      if (matching == NULL)
      {
        printf("no matching '] found\n'");
        break;
      }

      // if we found, we want the next one to be "("
      if (*(matching + 1) != '(')
      {
        printf("No start of '(' found\n");
        break;
      }

      if ((close_link = strstr(matching, ")")) == NULL)
      {
        printf("No matching ')' found\n");
        break;
      }

      // now all criteria fulfilled we want to copy the data

      end = i - 1;
      temp_length = end - start + 1;
      temp_text = (char *)calloc(temp_length + 1, sizeof(char));
      strncpy(temp_text, &text_data[start], temp_length);
      temp_text[temp_length] = '\0';
      set_text_data(node, temp_text, temp_length);

      // we start copying URL data
      new_child_node = create_empty_md_node(NODE_LINK);
      append_to_root(node, new_child_node);

      // we copy the text to replace the link
      temp_length = matching - &text_data[i] - 1;
      temp_text = (char *)calloc(temp_length + 1, sizeof(char));
      strncpy(temp_text, &text_data[i] + 1, temp_length);
      temp_text[temp_length] = '\0';
      printf("%d, %s\n", temp_length, temp_text);
      set_text_data(node->last_child, temp_text, temp_length);

      i += (temp_length + 2);
      // now we want to set the URL
      temp_length = close_link - (matching + 1) - 1;
      temp_text = (char *)calloc(temp_length + 1, sizeof(char));
      strncpy(temp_text, matching + 2, temp_length);
      temp_text[temp_length] = '\0';
      new_child_node->url_length = temp_length;
      new_child_node->url =
          (char *)calloc(new_child_node->url_length + 1, sizeof(char));
      printf("%d, %s\n", temp_length, temp_text);
      strncpy(new_child_node->url, temp_text, temp_length + 1);

      // we need to set i to move forward
      i += (temp_length + 2);
      start = i;

      break;
    default:
      break;
    }
  }
  end = text_length - 1;
  if (start != end)
  {
    temp_length = end - start + 1;
    temp_text = (char *)calloc(temp_length + 1, sizeof(char));
    strncpy(temp_text, &text_data[start], temp_length);
    temp_text[temp_length] = '\0';
    set_text_data(node, temp_text, temp_length);
  }
}
// void set_paragraph_data(md_node *node, const char *line, int line_length,
//                         int line_number) {
//   add_text_data(node, line, line_length, ' ');
// }

void set_text_data(md_node *node, const char *text, int text_length,
                   int line_number) {
  node->data = (char *)calloc(text_length + 1, sizeof(char));
  node->len = text_length;
  strncpy(node->data, text, text_length + 1);
}

void parse_new_paragraph_line(md_node *paragraph_node) { return; }

// void process_paragraph_data(md_node *node) {
//   char *text_data = node->first_child->data;
//   int text_length = node->first_child->len;
//   node->first_child = NULL;
//   node->last_child = NULL;
//
//   // now we reconstruct the tree based on the data
//   int i, start, end;
//   md_node *new_child_node;
//   char *temp_text;
//   char *matching;
//   char *close_link;
//   int temp_length;
//   start = 0;
//   for (i = 0; i < text_length; i++) {
//     switch (text_data[i]) {
//     case '*':
//       if (i > 0 && text_data[i - 1] == '\\') {
//         break;
//       }
//
//       if ((i == 0) || ((i > 0 && text_data[i - 1] == ' ') &&
//                        (i < text_length - 1 && text_data[i + 1] != ' '))) {
//
//         end = i - 1;
//         temp_length = end - start + 1;
//         temp_text = (char *)calloc(temp_length + 1, sizeof(char));
//         strncpy(temp_text, &text_data[start], temp_length);
//         temp_text[temp_length] = '\0';
//         set_text_data(node, temp_text, temp_length);
//
//         // add new emphasis node
//         start = i + 1;
//
//       } else if (((i > 0 && text_data[i - 1] != ' ') &&
//                   (i < text_length - 1 && text_data[i + 1] == ' ')) ||
//                  (i == text_length - 1)) {
//         end = i - 1;
//         // copy the text data
//         new_child_node = create_empty_md_node(NODE_STRONG);
//         append_to_root(node, new_child_node);
//         temp_length = end - start + 1;
//         temp_text = (char *)calloc(temp_length + 1, sizeof(char));
//         strncpy(temp_text, &text_data[start], temp_length);
//         temp_text[temp_length] = '\0';
//         set_text_data(node->last_child, temp_text, temp_length);
//         start = i + 1;
//       }
//       break;
//     case '_':
//       if (i > 0 && text_data[i - 1] == '\\') {
//         continue;
//       }
//
//       if ((i == 0) || ((i > 0 && text_data[i - 1] == ' ') &&
//                        (i < text_length - 1 && text_data[i + 1] != ' '))) {
//
//         end = i - 1;
//         temp_length = end - start + 1;
//         temp_text = (char *)calloc(temp_length + 1, sizeof(char));
//         strncpy(temp_text, &text_data[start], temp_length);
//         temp_text[temp_length] = '\0';
//         set_text_data(node, temp_text, temp_length);
//
//         // add new emphasis node
//         start = i + 1;
//
//       } else if (((i > 0 && text_data[i - 1] != ' ') &&
//                   (i < text_length - 1 && text_data[i + 1] == ' ')) ||
//                  (i == text_length - 1)) {
//         end = i - 1;
//         // copy the text data
//         new_child_node = create_empty_md_node(NODE_EMPH);
//         append_to_root(node, new_child_node);
//         temp_length = end - start + 1;
//         temp_text = (char *)calloc(temp_length + 1, sizeof(char));
//         strncpy(temp_text, &text_data[start], temp_length);
//         temp_text[temp_length] = '\0';
//         set_text_data(node->last_child, temp_text, temp_length);
//         start = i + 1;
//       }
//       break;
//     case '[':
//       // we now want to find the matching
//       // make custom strstr to ignore '\'
//       matching = strstr(&text_data[i], "]");
//       if (matching == NULL) {
//         printf("no matching '] found\n'");
//         break;
//       }
//
//       // if we found, we want the next one to be "("
//       if (*(matching + 1) != '(') {
//         printf("No start of '(' found\n");
//         break;
//       }
//
//       if ((close_link = strstr(matching, ")")) == NULL) {
//         printf("No matching ')' found\n");
//         break;
//       }
//
//       // now all criteria fulfilled we want to copy the data
//
//       end = i - 1;
//       temp_length = end - start + 1;
//       temp_text = (char *)calloc(temp_length + 1, sizeof(char));
//       strncpy(temp_text, &text_data[start], temp_length);
//       temp_text[temp_length] = '\0';
//       set_text_data(node, temp_text, temp_length);
//
//       // we start copying URL data
//       new_child_node = create_empty_md_node(NODE_LINK);
//       append_to_root(node, new_child_node);
//
//       // we copy the text to replace the link
//       temp_length = matching - &text_data[i] - 1;
//       temp_text = (char *)calloc(temp_length + 1, sizeof(char));
//       strncpy(temp_text, &text_data[i] + 1, temp_length);
//       temp_text[temp_length] = '\0';
//       printf("%d, %s\n", temp_length, temp_text);
//       set_text_data(node->last_child, temp_text, temp_length);
//
//       i += (temp_length + 2);
//       // now we want to set the URL
//       temp_length = close_link - (matching + 1) - 1;
//       temp_text = (char *)calloc(temp_length + 1, sizeof(char));
//       strncpy(temp_text, matching + 2, temp_length);
//       temp_text[temp_length] = '\0';
//       new_child_node->url_length = temp_length;
//       new_child_node->url =
//           (char *)calloc(new_child_node->url_length + 1, sizeof(char));
//       printf("%d, %s\n", temp_length, temp_text);
//       strncpy(new_child_node->url, temp_text, temp_length + 1);
//
//       // we need to set i to move forward
//       i += (temp_length + 2);
//       start = i;
//
//       break;
//     default:
//       break;
//     }
//   }
//   end = text_length - 1;
//   if (start != end) {
//     temp_length = end - start + 1;
//     temp_text = (char *)calloc(temp_length + 1, sizeof(char));
//     strncpy(temp_text, &text_data[start], temp_length);
//     temp_text[temp_length] = '\0';
//     set_text_data(node, temp_text, temp_length);
//   }
// }
