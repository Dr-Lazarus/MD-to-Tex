#include "parser.h"
#include "features/codeblock.h"
#include "features/headers.h"
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

// the idea of this function is based on the current node, we will figure out
// where to put the line with respect to the rest of the tree
void parse_line(md_node *root, const char *line, int line_length) {
  // first we check what content we have.
  if (line == NULL) {
    printf("Line should have something\n");
    abort();
  }

  md_node *new_child_node;
  // if its an empty line
  if (line_length == 0) {
    // we stop appending to the previous paragraph
    root->user_data = MODE_STARTNEW;
  } else if (is_header(line, line_length)) {
    // we are dealing with a header
    // we go back to root
    // we reset the mode to startnew
    root->user_data = MODE_STARTNEW;
    new_child_node = create_empty_md_node(NODE_HEADING);

    set_header_data(new_child_node, line, line_length);
    append_to_root(root, new_child_node);

  } else if (is_codeblock_indicator(line, line_length)) {

    switch (root->user_data) {
    case MODE_CODE:
      root->user_data = MODE_STARTNEW;
      break;
    case MODE_EMPTY:
    case MODE_STARTNEW:
      root->user_data = MODE_CODE;
      new_child_node = create_empty_md_node(NODE_CODE_BLOCK);
      set_code_language(new_child_node, line, line_length);
      append_to_root(root, new_child_node);
      break;
    case MODE_APPENDPARA:
      printf("why here\n");
      abort();
    default:
      printf("Unidentified mode to start code block %d\n", root->user_data);
      abort();
    }

  } else {
    if (root->user_data == MODE_CODE) {
      // means we want to add to the previous one
      set_code_data(root->last_child, line, line_length);
    } else {

      // currently assume its just text
      new_child_node = create_empty_md_node(NODE_PARAGRAPH);

      parse_paragraph_line(new_child_node, line, line_length);

      switch (root->user_data) {
      case MODE_STARTNEW:
      case MODE_EMPTY:
        append_to_root(root, new_child_node);
        break;
      case MODE_APPENDPARA:
        // we take the last_child of the last_child
        if (root->last_child->type != NODE_PARAGRAPH) {
          printf("last child is not paragraph, cannot append");
          abort();
        }
        link_children(root->last_child, new_child_node);
        break;
      default:
        break;
      }
      root->user_data = MODE_APPENDPARA;
    }
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

  while ((ptr = strstr(ptr, "\n")) != NULL) {
    line_length = ptr - start;
    if (line_size < line_length) {
      while (line_size < line_length) {
        line_size *= 2;
      }
      line = (char *)realloc(line, line_size * sizeof(char));
    }

    strncpy(line, start, line_length);
    line[line_length] = '\0';

    // Now we want to figure out how does this line fit in the Tree
    printf("parsing line: %d\n", line_number);
    printf("line: %s\n", line);
    parse_line(root, line, line_length);

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
