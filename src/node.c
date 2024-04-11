#include "node.h"
#include <stdio.h>
#include <stdlib.h>

void print_node(md_node *node, int indent_level)
{
  for (int i = 0; i < indent_level * 2; i++)
  {
    printf(" ");
  }
  if (node->data != NULL)
  {
    printf("data: %s\n", node->data);
  }
  else
  {
    printf("data: None\n");
  }

  switch (node->type)
  {
  case NODE_HEADING:
    for (int i = 0; i < indent_level * INDENTATION; i++)
    {
      printf(" ");
    }
    printf("level: %d\n", node->heading_level);
    break;
  case NODE_CODE:
    for (int i = 0; i < indent_level * 2; i++)
    {
      printf(" ");
    }
    printf("lang: %s\n", node->code_language);
    break;
  default:
    break;
  }
}

void print_type(md_node *node, int indent_level)
{
  for (int i = 0; i < indent_level * INDENTATION; i++)
  {
    printf(" ");
  }
  switch (node->type)
  {
  case NODE_ROOT:
    printf("root node\n");
    break;
  case NODE_PARAGRAPH:
    printf("paragraph node\n");
    break;
  case NODE_TEXT:
    printf("text node\n");
    break;
  case NODE_HEADING:
    printf("header node\n");
    break;
  case NODE_CODE_BLOCK:
    printf("code node\n");
    break;
  case NODE_MERMAID_DIAGRAM:
    printf("mermaid node\n");
    break;
  case NODE_IMAGE:
    printf("image node\n");
    break;
  case NODE_LIST:
    printf("list node\n");
    break;
  default:
    printf("unknown node\n");
    break;
  }
}

md_node *create_md_node(NodeType type, char *data, int len, md_node *next,
                        md_node *prev, md_node *parent, md_node *first_child,
                        md_node *last_child, Mode user_data)
{
  md_node *node = (md_node *)malloc(sizeof(md_node));
  node->type = type;
  node->next = next;
  node->prev = prev;
  node->parent = parent;
  node->first_child = first_child;
  node->last_child = last_child;
  // node->data = data;
  // node->len = len;
  node->user_data = user_data;
  if (type == NODE_MERMAID_DIAGRAM)
  {
    node->mermaid_code = strdup(data);
    node->len = strlen(data);
  }
  else
  {
    node->data = data;
    node->len = len;
char *print_line_type(LineType type) {
  switch (type) {
  case LINE_EMPTY:
    return "LINE_EMPTY";
    break;
  case LINE_LISTITEM:
    return "LINE_LISTITEM";
    break;
  case LINE_PARAGRAPH:
    return "LINE_PARAGRAPH";
    break;
  case LINE_HEADER:
    return "LINE_HEADER";
    break;
  case LINE_IMAGE:
    return "LINE_IMAGE";
    break;
  case LINE_CODE_DELIM:
    return "LINE_CODE_DELIM";
    break;
  case LINE_MATH_DELIM:
    return "LINE_MATH_DELIM";
    break;
  case LINE_BLOCKTYPE:
    return "LINE_BLOCKTYPE";
    break;
  case LINE_TEXT:
    return "LINE_TEXT";
    break;
  default:
    return "LINE_UNKNOWN";
    break;
  }
}

char *print_node_type(NodeType type) {
  switch (type) {
  case NODE_BLOCK_QUOTE:
    return "NODE_BLOCK_QUOTE";
    break;
  case NODE_BREAK:
    return "NODE_BREAK";
    break;
  case NODE_CODE:
    return "NODE_CODE";
    break;
  case NODE_CODE_BLOCK:
    return "NODE_CODE_BLOCK";
    break;
  case NODE_MATH:
    return "NODE_MATH";
    break;
  case NODE_MATH_BLOCK:
    return "NODE_MATH_BLOCK";
    break;
  case NODE_DEFINITION:
    return "NODE_DEFINITION";
    break;
  case NODE_EMPH:
    return "NODE_EMPH";
    break;
  case NODE_HEADING:
    return "NODE_HEADING";
    break;
  case NODE_HTML:
    return "NODE_HTML";
    break;
  case NODE_IMAGE:
    return "NODE_IMAGE";
    break;
  case NODE_IMAGEREFERENCE:
    return "NODE_IMAGEREFERENCE";
    break;
  case NODE_INLINECODE:
    return "NODE_INLINECODE";
    break;
  case NODE_LINK:
    return "NODE_LINK";
    break;
  case NODE_LINKREFERENCE:
    return "NODE_LINKREFERENCE";
    break;
  case NODE_LIST:
    return "NODE_LIST";
    break;
  case NODE_ITEM:
    return "NODE_ITEM";
    break;
  case NODE_PARAGRAPH:
    return "NODE_PARAGRAPH";
    break;
  case NODE_ROOT:
    return "NODE_ROOT";
    break;
  case NODE_STRONG:
    return "NODE_STRONG";
    break;
  case NODE_TEXT:
    return "NODE_TEXT";
    break;
  case NODE_THEMATICBREAK:
    return "NODE_THEMATICBREAK";
    break;
  case NODE_SOFTBREAK:
    return "NODE_SOFTBREAK";
    break;
  case NODE_LINEBREAK:
    return "NODE_LINEBREAK";
    break;
  case NODE_MERMAID_DIAGRAM:
    return "NODE_MERMAID_DIAGRAM";
    break;
  default:
    return "unknown";
  }
}

md_node *create_md_node(NodeType type, int start_line, int end_line,
                        int start_column, int end_column) {
  md_node *node = (md_node *)malloc(sizeof(md_node));

  if (node == NULL) {
    printf("Failed to allocate memory for node %s", print_node_type(type));
    abort();
  }

  node->type = type;
  node->next = NULL;
  node->prev = NULL;
  node->parent = NULL;
  node->first_child = NULL;
  node->last_child = NULL;
  node->user_data = MODE_APPEND;
  node->data = NULL;
  node->len = 0;
  node->start_line = start_line;
  node->end_line = end_line;
  node->start_column = start_column;
  node->end_column = end_column;
  return node;
}

md_node *create_empty_md_node(NodeType type)
{
  return create_md_node(type, NULL, 0, NULL, NULL, NULL, NULL, NULL, MODE_NONE);
md_node *create_empty_md_node(NodeType type, int start_line, int end_line) {
  return create_md_node(type, start_line, end_line, -1, -1);
}

NodeType md_node_get_type(md_node *node) { return node->type; }
char *md_node_get_fence_info(md_node *node) { return node->code_language; }
char *md_node_get_literal(md_node *node) { return node->data; }
char *md_node_get_mermaid_code(md_node *node) { return node->data; }
int md_node_get_heading_level(md_node *node) { return node->heading_level; }

ListType md_node_get_list_type(md_node *node) { return node->list_type; }

char *md_node_get_url(md_node *node) { return node->url; }

char *md_node_get_title(md_node *node) { return node->title; }
