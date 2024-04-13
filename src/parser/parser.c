#include "parser.h"
#include "features/blockquote.h"
#include "features/codeblock.h"
#include "features/headers.h"
#include "features/image.h"
#include "features/list.h"
#include "features/mathblock.h"
#include "features/paragraph.h"
#ifndef TREE_H
#define TREE_H
#include "../tree.h"
#endif
#include <regex.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// first we need some code to read the file
char *read_source_code(const char *filename) {
  FILE *file = fopen(filename, "r");
  if (file == NULL) {
    fprintf(stderr, "Error opening file: %s\n", filename);
    exit(EXIT_FAILURE);
  }
  fseek(file, 0, SEEK_END);
  long file_size = ftell(file);
  rewind(file);
  char *source_code = (char *)malloc((file_size + 1) * sizeof(char));
  if (source_code == NULL) {
    fprintf(stderr, "Error allocating memory for source code\n");
    exit(EXIT_FAILURE);
  }
  size_t read_size = fread(source_code, sizeof(char), file_size, file);
  source_code[read_size] = '\0';
  fclose(file);
  return source_code;
}

LineType get_line_type(const char *line, int line_length) {
  if (line_length == 0) {
    return LINE_EMPTY;
  } else if (is_header(line, line_length)) {
    return LINE_HEADER;
  } else if (is_mathblock_indicator(line, line_length)) {
    return LINE_MATH_DELIM;
  } else if (is_codeblock_indicator(line, line_length)) {
    return LINE_CODE_DELIM;
  } else if (is_image_link(line, line_length)) {
    return LINE_IMAGE;
  } else if (is_blockquote(line, line_length)) {
    return LINE_BLOCKQUOTE;
  } else if (is_list_item(line, line_length)) {
    return LINE_LISTITEM;
  } else {
    return LINE_TEXT;
  }
}

void parse_line(md_node *root, const char *line, int line_length,
                int line_number, int debugging) {

  int i, indent, nested;
  LineType current_line_type;
  ListType list_type;
  md_node *prev_node;
  md_node *new_child_node;
  md_node *list_child;
  md_node *prev_list_item;
  char *delimiter;
  char *number_delim;
  char *blockquote_line;
  int list_number_delim;
  char char_delim;
  int delimiter_length;
  int initial;
  // we get the line type first
  current_line_type = get_line_type(line, line_length);

  // now we want to figure out how it fits in the Tree
  // First we check what the last node is
  prev_node = root->last_child;

  // Now we want to figure out how does this line fit in the Tree
  if (debugging) {
    printf("parsing line: %d, type: %s, prev node: %s\n", line_number,
           print_line_type(current_line_type),
           prev_node == NULL ? NULL : print_node_type(prev_node->type));
    printf("Line %d data: %s\n", line_number, line);
  }

  if (prev_node != NULL &&
      (prev_node->type == NODE_CODE_BLOCK ||
       prev_node->type == NODE_MATH_BLOCK) &&
      prev_node->user_data == MODE_APPEND) {

    // if the line is a delimiter
    if ((prev_node->type == NODE_CODE_BLOCK &&
         current_line_type == LINE_CODE_DELIM) ||
        (prev_node->type == NODE_MATH_BLOCK &&
         current_line_type == LINE_MATH_DELIM)) {
      // we terminate the current prev_node
      printf("ending block\n");
      collate_children_text(prev_node);
      prev_node->end_line = line_number;
      prev_node->user_data = MODE_PROCESSED;
    } else {
      prev_node->end_line = line_number;
      new_child_node =
          create_md_node(NODE_TEXT, line_number, line_number, 0, line_length);
      set_text_data(new_child_node, line, line_length, line_number);
      append_to_root(root->last_child, new_child_node);
    }
  } else if (current_line_type == LINE_CODE_DELIM ||
             current_line_type == LINE_MATH_DELIM) {

    // so if the previous node is not in append mode, we should jump here
    // and create the new code block
    new_child_node =
        create_md_node(current_line_type == LINE_CODE_DELIM ? NODE_CODE_BLOCK
                                                            : NODE_MATH_BLOCK,
                       line_number, line_number, -1, -1);
    set_code_language(new_child_node, line, line_length);
    append_to_root(root, new_child_node);

  } else if (current_line_type == LINE_HEADER) {
    // if it is a header
    // it doesn't matter what the last node is
    // we will just mark previous node as processed.
    if (prev_node != NULL) {
      prev_node->user_data = MODE_PROCESSED;
    }

    new_child_node =
        create_md_node(NODE_HEADING, line_number, line_number, -1, -1);
    new_child_node->user_data = MODE_PROCESSED;
    set_header_data(new_child_node, line, line_length);
    append_to_root(root, new_child_node);

  } else if (current_line_type == LINE_IMAGE) {

    if (prev_node != NULL) {
      prev_node->user_data = MODE_PROCESSED;
    }

    new_child_node =
        create_md_node(NODE_IMAGE, line_number, line_number, -1, -1);
    new_child_node->user_data = MODE_PROCESSED;
    set_image_link(new_child_node, line, line_length);
    set_image_caption(new_child_node, line, line_length);
    append_to_root(root, new_child_node);

  } else if (current_line_type == LINE_EMPTY) {

    // helps terminate the previous paragraph if necessary
    if (prev_node != NULL &&
        (prev_node->type == NODE_PARAGRAPH || prev_node->type == NODE_LIST) &&
        prev_node->user_data == MODE_APPEND) {
      prev_node->user_data = MODE_PROCESSED;
    }

  } else if (current_line_type == LINE_TEXT) {

    // if previous node is not a appending paragraph,
    // we need to create a new appending paragraph
    if (prev_node == NULL || !(prev_node->type == NODE_PARAGRAPH &&
                               prev_node->user_data == MODE_APPEND)) {

      new_child_node =
          create_md_node(NODE_PARAGRAPH, line_number, line_number, -1, -1);
      append_to_root(root, new_child_node);
      // make the new paragraph node the prev_node
      prev_node = new_child_node;
    }
    // create a new paragraph and then set it to mode append
    // we will append new stuff
    prev_node->end_line = line_number;
    new_child_node =
        create_md_node(NODE_TEXT, line_number, line_number, 0, line_length);
    set_text_data(new_child_node, line, line_length, line_number);
    // check if we need add an extra space in front of the text data
    // if there is another text node then we need to add a space infront
    if (prev_node->last_child != NULL) {
      add_space_infront(new_child_node);
    }
    append_to_root(root->last_child, new_child_node);
    parse_new_paragraph_line(prev_node);

  } else if (current_line_type == LINE_BLOCKQUOTE) {

    if (prev_node == NULL || !(prev_node->type == NODE_BLOCK_QUOTE &&
                               prev_node->user_data == MODE_APPEND)) {

      new_child_node =
          create_md_node(NODE_BLOCK_QUOTE, line_number, line_number, -1, -1);
      append_to_root(root, new_child_node);
      // make the new paragraph node the prev_node
      prev_node = new_child_node;
    }

    blockquote_line = strip_blockquote(line, line_length);
    parse_line(prev_node, blockquote_line, strlen(blockquote_line), line_number,
               debugging);

  } else if (current_line_type == LINE_LISTITEM) {

    // check indentation level
    for (indent = 0; indent < line_length; indent++) {
      if (line[indent] != ' ') {
        break;
      }
    }
    // check if indent is correct
    if (indent % INDENT_SPACE != 0) {
      printf(
          "Line %d: Inconsistent indentation with %d. Supposed to only be "
          "multiples of %d\n",
          line_number, indent, INDENT_SPACE);
      exit(EXIT_FAILURE);
    }

    // if the last child was not a list
    // we add if indent == 0
    // otherwise we throw an error
    if (prev_node != NULL && prev_node->type != NODE_LIST && indent != 0) {
      printf("Line %d: Not allowed to have nested list declared immediately\n",
             line_number);
      exit(EXIT_FAILURE);
    }

    nested = indent / INDENT_SPACE;

    // we go n - 1 for the depth.
    // so if its 1 nested, we try to go from root down once

    list_child = root;
    for (i = 0; i < nested; i++) {
      if (list_child->last_child != NULL &&
          list_child->last_child->type == NODE_LIST) {
        list_child = list_child->last_child;
      } else {
        printf(
            "Line %d: Indentation level should only be incremental. Jumping \n"
            "from depth of %d to depth of %d is not allowed\n",
            line_number, i, nested);
        exit(EXIT_FAILURE);
      }
    }
    // now we create the list node if the last node was not list
    // we should also create if the previous list is in MODE_PROCESSED
    // identify the type of list item first
    if (line[indent] >= '0' && line[indent] <= '9') {
      list_type = LIST_NUMBERED;
    } else {
      list_type = LIST_BULLET;
    }

    if (list_child->last_child == NULL ||
        list_child->last_child->type != NODE_LIST ||
        (list_child->last_child->type == NODE_LIST &&
         list_child->last_child->user_data == MODE_PROCESSED)) {
      new_child_node = create_md_node(NODE_LIST, line_number, line_number, 0,
                                      line_length - 1);
      // we also identify what kind of list is it
      new_child_node->list_type = list_type;

      append_to_root(list_child, new_child_node);
      prev_node = new_child_node;
    } else {
      prev_node = list_child->last_child;
    }

    // we should check all the other conditions for delimiter information

    // now we create the node itself with the data
    // we check for when we reach the next space
    delimiter = (char *)calloc(line_length, sizeof(char));
    delimiter_length = 0;
    for (initial = indent; indent < line_length; indent++) {
      if (line[indent] == ' ') {
        break;
      }

      delimiter[indent - initial] = line[indent];
      delimiter_length++;
    }
    delimiter[delimiter_length] = '\0';
    // we should do all the checks
    // first, we identify if the delimiter is correct
    if (prev_node->last_child != NULL) {
      prev_list_item = prev_node->last_child;
      while (prev_list_item != NULL && prev_list_item->type != NODE_ITEM) {
        prev_list_item = prev_list_item->prev;
      }
    } else {
      prev_list_item = NULL;
    }

    // now we need to check the delimiter
    // First we create it
    if (list_type == LIST_NUMBERED) {
      number_delim = (char *)calloc(delimiter_length, sizeof(char));
      strncpy(number_delim, delimiter, delimiter_length - 1);
      number_delim[delimiter_length - 1] = '\0';
      list_number_delim = atoi(number_delim);
      // this is in case;
      char_delim = ' ';
    } else if (list_type == LIST_BULLET) {
      char_delim = delimiter[0];
      // in case
      list_number_delim = 0;
    } else {
      char_delim = ' ';
      list_number_delim = 0;
    }

    // if the list types are not different
    if (list_type != prev_node->list_type) {
      printf(
          "Line %d: Do not mix list types together new line is %s while list "
          "is %s\n",
          line_number, list_type == LIST_NUMBERED ? "Numbered" : "Bullet",
          prev_node->list_type == LIST_NUMBERED ? "Numbered" : "Bullet");

    } else {
      // check for consistency
      if (prev_list_item != NULL) {
        // now we need to check the delimiter;
        if (prev_node->list_type == LIST_BULLET) {

          if (prev_list_item->delimiter != char_delim) {
            printf(
                "Line %d: Inconsistent delimiter for list (prev %c, next %c)\n",
                line_number, prev_list_item->delimiter, delimiter[0]);
          }
        } else if (prev_node->list_type == LIST_NUMBERED) {

          if (prev_list_item->list_number + 1 != list_number_delim) {
            printf(
                "Line %d: Inconsistent numbering of elements (prev %d, next "
                "%d)\n",
                line_number, prev_list_item->list_number, list_number_delim);
          }
        }
      } else if (prev_list_item == NULL &&
                 prev_node->list_type == LIST_NUMBERED) {
        if (list_number_delim != 1) {
          printf(
              "Line %d: Numbered list does not start with 1 (currently %d)\n",
              line_number, list_number_delim);
        }
      }
    }

    // we then ignore the space
    indent++;

    new_child_node = create_md_node(NODE_ITEM, line_number, line_number, indent,
                                    line_length - 1);
    // potentially messed up right now if the list type not compatible but it will be safe
    new_child_node->list_number = list_number_delim;
    new_child_node->delimiter = char_delim;

    append_to_root(prev_node, new_child_node);
    prev_node = new_child_node;
    new_child_node = create_md_node(NODE_TEXT, line_number, line_number, indent,
                                    line_length - 1);
    new_child_node->len = line_length - indent;
    new_child_node->data =
        (char *)calloc(new_child_node->len + 1, sizeof(char));
    strncpy(new_child_node->data, &(line[indent]), new_child_node->len);
    new_child_node->data[new_child_node->len] = '\0';
    append_to_root(prev_node, new_child_node);

    // now that we have checked through all the requirements,
  } else {
    printf("Line %d: Ignoring line %s, unknown\n", line_number,
           print_line_type(current_line_type));
  }
}

md_node *parse_source(char *file_name, int debugging) {

  char *file_contents = read_source_code(file_name);

  md_node *root = init_tree();

  int line_number = 0;
  int char_so_far = 0;
  int line_length = 0;
  int line_size = MAX_LINE_LENGTH;
  char *line = (char *)calloc(line_size, sizeof(char));
  char *ptr = file_contents;
  char *start = file_contents;

  while ((ptr = strstr(ptr, "\n")) != NULL) {
    line_length = ptr - start;
    // perform realloc if not sufficient length
    if (line_size < line_length) {
      while (line_size < line_length) {
        line_size *= 2;
      }
      line = (char *)realloc(line, line_size * sizeof(char));
    }

    strncpy(line, start, line_length);
    line[line_length] = '\0';

    parse_line(root, line, line_length, line_number, debugging);
    // terminating stuff
    line_number++;
    char_so_far += (line_length + 1);
    line_length = 0;
    ptr++;
    start = ptr;
  }
  // last line needs to be considered also
  line_length = strlen(file_contents) - char_so_far;
  if (line_size < line_length) {
    while (line_size < line_length) {
      line_size *= 2;
    }
    line = (char *)realloc(line, line_size * sizeof(char));
  }

  strncpy(line, start, line_length);
  line[line_length] = '\0';
  parse_line(root, line, line_length, line_number, debugging);
  line_number++;
  line_length = 0;
  start = ptr;
  ptr++;

  free(line);
  return root;
}
// possible reference for line reading
// https://stackoverflow.com/questions/29576799/reading-an-unknown-length-line-from-stdin-in-c-with-fgets
