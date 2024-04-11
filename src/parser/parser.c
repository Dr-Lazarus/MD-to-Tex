#include "parser.h"
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
  } else if (is_list_item(line, line_length)) {
    return LINE_LISTITEM;
  } else {
    return LINE_TEXT;
  }
}

md_node *parse_source(char *file_name) {

  char *file_contents = read_source_code(file_name);

  md_node *root = init_tree();

  int line_number = 0;
  int char_so_far = 0;
  int line_length = 0;
  int line_size = MAX_LINE_LENGTH;
  char *line = (char *)calloc(line_size, sizeof(char));
  char *ptr = file_contents;
  char *start = file_contents;
  LineType current_line_type;
  NodeType prev_node_type;
  md_node *prev_node;
  md_node *new_child_node;

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

    // we get the line type first
    current_line_type = get_line_type(line, line_length);

    // now we want to figure out how it fits in the Tree
    // First we check what the last node is
    if (root->last_child == NULL) {
      prev_node_type = NODE_NONE;
    } else {
      prev_node_type = root->last_child->type;
    }
    prev_node = root->last_child;

    // Now we want to figure out how does this line fit in the Tree
    printf("parsing line: %d, type: %s, prev node: %s\n", line_number,
           print_line_type(current_line_type), print_node_type(prev_node_type));

    if (prev_node_type == NODE_CODE_BLOCK &&
        prev_node->user_data == MODE_APPEND) {

      // if the line is a delimiter
      if (current_line_type == LINE_CODE_DELIM) {
        // we terminate the current prev_node
        printf("ending code bloc\n");
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
    } else if (current_line_type == LINE_CODE_DELIM) {

      // so if the previous node is not in append mode, we should jump here
      // and create the new code block
      new_child_node =
          create_md_node(NODE_CODE_BLOCK, line_number, line_number, -1, -1);
      set_code_language(new_child_node, line, line_length);
      append_to_root(root, new_child_node);

    } else if (current_line_type == LINE_HEADER) {
      // if it is a header
      // it doesn't matter what the last node is
      // we will just mark previous node as processed.
      prev_node->user_data = MODE_PROCESSED;
      new_child_node =
          create_md_node(NODE_HEADING, line_number, line_number, -1, -1);
      new_child_node->user_data = MODE_PROCESSED;
      set_header_data(new_child_node, line, line_length);
      append_to_root(root, new_child_node);

    } else if (current_line_type == LINE_IMAGE) {

      prev_node->user_data = MODE_PROCESSED;
      new_child_node =
          create_md_node(NODE_IMAGE, line_number, line_number, -1, -1);
      new_child_node->user_data = MODE_PROCESSED;
      set_image_link(new_child_node, line, line_length);
      set_image_caption(new_child_node, line, line_length);
      append_to_root(root, new_child_node);

    } else if (current_line_type == LINE_EMPTY) {

      // helps terminate the previous paragraph if necessary
      if (prev_node_type == NODE_PARAGRAPH &&
          prev_node->user_data == MODE_APPEND) {
        prev_node->user_data = MODE_PROCESSED;
      }

    } else if (current_line_type == LINE_TEXT) {
      // if previous node is not a appending paragraph,
      // we need to create a new appending paragraph
      if (!(prev_node_type == NODE_PARAGRAPH &&
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
    } else if (current_line_type == LINE_LISTITEM) {

    } else {
      printf("Line %d: Ignoring line %s, unknown\n", line_number,
             print_line_type(current_line_type));
    }

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
  line_number++;
  line_length = 0;
  start = ptr;
  ptr++;

  free(line);
  return root;
}
// possible reference for line reading
// https://stackoverflow.com/questions/29576799/reading-an-unknown-length-line-from-stdin-in-c-with-fgets
