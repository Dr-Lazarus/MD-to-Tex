#include "parser.h"
#include "features/codeblock.h"
#include "features/headers.h"
#include "features/image.h"
#include "features/list.h"
#include "features/paragraph.h"
#ifndef TREE_H
#define TREE_H
#include "../tree.h"
#endif
#include <regex.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int in_mermaid_block = 0;
static char *mermaid_code = NULL;

// first we need some code to read the file
char *read_source_code(const char *filename)
{
  FILE *file = fopen(filename, "r");
  if (file == NULL)
  {
    fprintf(stderr, "Error opening file: %s\n", filename);
    exit(EXIT_FAILURE);
  }
  fseek(file, 0, SEEK_END);
  long file_size = ftell(file);
  rewind(file);
  char *source_code = (char *)malloc((file_size + 1) * sizeof(char));
  if (source_code == NULL)
  {
    fprintf(stderr, "Error allocating memory for source code\n");
    exit(EXIT_FAILURE);
  }
  size_t read_size = fread(source_code, sizeof(char), file_size, file);
  source_code[read_size] = '\0';
  fclose(file);
  return source_code;
}

LineType get_line_type(const char *line, int line_length)
{
  if (line_length == 0)
  {
    return LINE_EMPTY;
  }
  else if (is_header(line, line_length))
  {
    return LINE_HEADER;
    // } else if (is_mathblock_indicator(line, line_length)) {
    //   return LINE_MATH_DELIM;
  }
  else if (is_codeblock_indicator(line, line_length))
  {
    return LINE_CODE_DELIM;
  }
  else if (is_image_link(line, line_length))
  {
    return LINE_IMAGE;
  }
  else if (is_list_item(line, line_length))
  {
    printf("is list\n");
    return LINE_LISTITEM;
  }
  else
  {
    return LINE_TEXT;
  }
}

void process_last_node(md_node *node)
{
  if (node == NULL || node->user_data == MODE_PROCESSED)
  {
    return;
  }

  switch (node->type)
  {
  case NODE_LIST:
    process_list_data(node);
    break;
  case NODE_PARAGRAPH:
    process_paragraph_data(node);
  default:
    break;
  }
  node->user_data = MODE_PROCESSED;
}

// the idea of this function is based on the current node, we will figure out
// where to put the line with respect to the rest of the tree
void parse_line(md_node *root, const char *line, int line_length)
{
  // first we check what content we have.
  if (line == NULL)
  {
    printf("Line should have something\n");
    abort();
  }

  md_node *new_child_node;
  // if its an empty line
  printf("type: ");
  LineType prev_line_type = root->prev_line_type;
  LineType current_line_type = get_line_type(line, line_length);

  switch (current_line_type)
  {
  case LINE_EMPTY:
    if (root->last_child != NULL && root->last_child->type == NODE_CODE_BLOCK &&
        root->last_child->user_data != MODE_PROCESSED)
    {

      // we don't want to process last and add the code data
      set_code_data(root->last_child, line, line_length);
    }
    else
    {
      process_last_node(root->last_child);
    }
    break;

  case LINE_CODE_DELIM:
    // check if previous one was code block
    if (root->last_child != NULL && root->last_child->type == NODE_CODE_BLOCK &&
        root->last_child->user_data != MODE_PROCESSED)
    {
      root->last_child->user_data = MODE_PROCESSED;

      printf("end code block\n");
    }
    else
    {
      // start new node
      process_last_node(root->last_child);

      printf("start code block\n");
      new_child_node = create_empty_md_node(NODE_CODE_BLOCK);
      set_code_language(new_child_node, line, line_length);
      append_to_root(root, new_child_node);
    }
    break;

  case LINE_LISTITEM:

    if (root->last_child == NULL || (root->last_child->type != NODE_LIST) ||
        (root->last_child->user_data == MODE_PROCESSED))
    {

      new_child_node = create_empty_md_node(NODE_LIST);
      append_to_root(root, new_child_node);
    }

    switch (root->last_child->type)
    {
    case NODE_LIST:
      set_item_data(root->last_child, line, line_length);
      break;
    default:
      printf("should not reach\n");
      break;
    }
    break;

  case LINE_HEADER:

    process_last_node(root->last_child);

    new_child_node = create_empty_md_node(NODE_HEADING);
    set_header_data(new_child_node, line, line_length);
    append_to_root(root, new_child_node);
    break;

  case LINE_IMAGE:
    process_last_node(root->last_child);

    new_child_node = create_empty_md_node(NODE_IMAGE);
    set_image_link(new_child_node, line, line_length);
    set_image_caption(new_child_node, line, line_length);
    append_to_root(root, new_child_node);
    break;

  case LINE_TEXT:
    if (root->last_child != NULL && root->last_child->type != NODE_PARAGRAPH &&
        root->last_child->type != NODE_CODE_BLOCK)
    {
      process_last_node(root->last_child);
    }
    if (root->last_child == NULL ||
        root->last_child->user_data == MODE_PROCESSED)
    {
      new_child_node = create_empty_md_node(NODE_PARAGRAPH);
      append_to_root(root, new_child_node);
    }

    switch (root->last_child->type)
    {
    case NODE_PARAGRAPH:
      set_paragraph_data(root->last_child, line, line_length);
      break;
    case NODE_CODE_BLOCK:
      set_code_data(root->last_child, line, line_length);
      break;
    default:
      printf("should be unreachable\n");
      break;
    }
    break;

  default:
    printf("unknown type %d\n", current_line_type);
    break;
  }

  root->prev_line_type = current_line_type;
}

md_node *parse_source(char *file_name)
{

  char *file_contents = read_source_code(file_name);

  md_node *root = init_tree();

  int line_number = 0;
  int char_so_far = 0;
  int line_length = 0;
  int line_size = MAX_LINE_LENGTH;
  char *line = (char *)calloc(line_size, sizeof(char));
  char *ptr = file_contents;
  char *start = file_contents;

  while ((ptr = strstr(ptr, "\n")) != NULL)
  {
    line_length = ptr - start;
    if (line_size < line_length)
    {
      while (line_size < line_length)
      {
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
  if (line_size < line_length)
  {
    while (line_size < line_length)
    {
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
