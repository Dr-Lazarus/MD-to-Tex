#include "list.h"
#include <regex.h>
#include <stdio.h>

int is_list_item(const char *line, int line_length) {
 regex_t list_regex;
 int value;
 // Creation of regEx
 // we are opinionated and need the space
 value = regcomp(&list_regex, "^(\t| )*(\\*|-|\\+)(\t| )(.*)$", REG_EXTENDED);
 if (value != 0) {
  printf("regex didn't compile\n");
 }
 value = regexec(&list_regex, line, 0, NULL, 0);
 if (value == 0) {
  return 1;
 }

 // check numbered list
 value =
     regcomp(&list_regex, "^(\t| )*([[:digit:]]+\\.)(\t| )(.*)$", REG_EXTENDED);
 if (value != 0) {
  printf("regex didn't compile\n");
 }
 value = regexec(&list_regex, line, 0, NULL, 0);

 return (value == 0);
}

// void set_item_data(md_node *node, const char *line, int line_length) {
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
//     text_node->data[text_node->len] = '\n';
//     text_node->data[text_node->len + 1] = '\0';
//     strncat(text_node->data, line, line_length + 1);
//     // 1 more character than usual since the '\n'
//     text_node->len += (line_length + 1);
//   }
// }
//
// int check_punctuation(char *text, int text_length) {
//   char *new_line_char;
//   char initial_punc = text[0];
//   if (initial_punc >= '0' && initial_punc <= '9') {
//     // it is numbered list
//     // we don't check
//     return 1;
//   }
//
//   new_line_char = text;
//   while ((new_line_char = strstr(new_line_char, "\n")) != NULL) {
//     new_line_char++;
//     // skip if it is space or tab
//     while (new_line_char[0] == ' ' || new_line_char[0] == '\t') {
//       new_line_char++;
//     }
//
//     // if it doesnt match the initial then we ignore
//     if (new_line_char[0] != initial_punc) {
//       return 0;
//     }
//   }
//
//   return 1;
// }
//
// // TODO to implement to check for consistent white space indentation
// int check_whitespace_consistency(char *text, int text_length) {
//
//   char *new_line_char;
//   char *start;
//   char *end;
//   int indentation_level = 0;
//   char indent = '\0';
//   start = text;
//   while ((end = strstr(start, "\n")) != NULL) {
//     if (*start != ' ' && *start != '\t') {
//       start = (end + 1);
//       continue;
//     }
//     if (indent == '\0') {
//       indent = *start;
//     }
//
//     // keep progressing if it is a type of indent and its equivalent
//     while (*start == indent && (*start == ' ' || *start == '\t')) {
//       start++;
//     }
//
//     // check why we bailed on the while loop
//     // if it is either of these, we did not hit the delimiter and theres an
//     // inconsistency
//     if (*start == ' ' || *start == '\t') {
//       return 0;
//     }
//     start = (end + 1);
//   }
//
//   return 1;
// }
//
// void process_list_data(md_node *node) {
//   char *text_data = node->first_child->data;
//   int text_length = node->first_child->len;
//   node->first_child = NULL;
//   node->last_child = NULL;
//
//   if (text_data[0] == ' ' || text_data[0] == '\t') {
//     printf("List cannot start with an empty space\n");
//     abort();
//   }
//
//   // figure out the type of list
//   if (text_data[0] == '-' || text_data[0] == '+' || text_data[0] == '*') {
//     node->list_type = LIST_BULLET;
//   } else {
//     node->list_type = LIST_NUMBERED;
//   }
//
//   // now we construct the list
//   // check that the punctuation is all the same
//   // Only checks for unnumbered list
//   if (!check_punctuation(text_data, text_length)) {
//     printf("Inconsistent punctuation in unnumbered list\n");
//     abort();
//   }
//   if (!check_whitespace_consistency(text_data, text_length)) {
//     printf("Inconsistent indentation\n");
//     abort();
//   }
//   // we cannot handle nested currently
//   char *temp_text;
//   int temp_length;
//   char *new_line_char;
//   int i;
//   char *start = text_data;
//   char *end;
//   int white_space;
//   md_node *new_list_node;
//   md_node *new_text_node;
//
//   while ((end = strstr(start, "\n")) != NULL) {
//     // we want before the \n character
//     end--;
//
//     // we skip white space, this will be helpful later
//     white_space = 0;
//     while (start[0] == ' ' || start[0] == '\t') {
//       white_space++;
//       start++;
//     }
//
//     // now we want to skip the delimiter;
//     while (start[0] != ' ' && start[0] != '\t') {
//       start++;
//     }
//     // move one forward to skip the ' ' or '\t'
//     start++;
//     temp_length = end - start + 1;
//     temp_text = (char *)calloc(temp_length + 1, sizeof(char));
//     // create the new list node
//     new_list_node = create_empty_md_node(NODE_ITEM);
//     append_to_root(node, new_list_node);
//
//     // create the text node that belongs to the list item
//     new_text_node = create_empty_md_node(NODE_TEXT);
//     append_to_root(new_list_node, new_text_node);
//
//     // copy the text data
//     new_text_node->len = end - start + 1;
//     new_text_node->data = (char *)calloc(new_text_node->len + 1,
//     sizeof(char)); strncpy(new_text_node->data, start, new_text_node->len);
//     new_text_node->data[new_text_node->len] = '\0';
//
//     // we want start to skip the '\n' char
//     start = end + 2;
//   }
//   end = &text_data[text_length - 1];
//   // we skip white space, this will be helpful later
//   white_space = 0;
//   while (start[0] == ' ' || start[0] == '\t') {
//     white_space++;
//     start++;
//   }
//
//   // now we want to skip the delimiter;
//   while (start[0] != ' ' && start[0] != '\t') {
//     start++;
//   }
//   // move one forward to skip the ' ' or '\t'
//   start++;
//   temp_length = end - start + 1;
//   temp_text = (char *)calloc(temp_length + 1, sizeof(char));
//   // create the new list node
//   new_list_node = create_empty_md_node(NODE_ITEM);
//   append_to_root(node, new_list_node);
//
//   // create the text node that belongs to the list item
//   new_text_node = create_empty_md_node(NODE_TEXT);
//   append_to_root(new_list_node, new_text_node);
//
//   // copy the text data
//   new_text_node->len = end - start + 1;
//   new_text_node->data = (char *)calloc(new_text_node->len + 1, sizeof(char));
//   strncpy(new_text_node->data, start, new_text_node->len);
//   new_text_node->data[new_text_node->len] = '\0';
//   printf("processing list\n");
// }
