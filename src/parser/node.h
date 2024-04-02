#include <stdint.h>
#include <stdio.h>
#include <string.h>

typedef enum {
  NODE_BLOCKQUOTE,
  NODE_BREAK,
  NODE_CODE,
  NODE_DEFINITION,
  NODE_EMPHASIS,
  NODE_HEADING, // currently ok
  NODE_HTML,    // we will not take this
  NODE_IMAGE,
  NODE_IMAGEREFERENCE,
  NODE_INLINECODE,
  NODE_LINK,
  NODE_LINKREFERENCE,
  NODE_LIST,
  NODE_LISTITEM,
  NODE_PARAGRAPH, // currently ok but not the details
  NODE_ROOT,      // currently in use for root
  NODE_STRONG,
  NODE_TEXT, // currently the only type of text
  NODE_THEMATICBREAK
} NodeType;

typedef enum {
  MODE_EMPTY,
  MODE_APPENDPARA,
  MODE_STARTNEW,
  MODE_NONE,
  MODE_CODE
} Mode;

typedef struct md_node {
  struct md_node *next;
  struct md_node *prev;
  struct md_node *parent;
  struct md_node *first_child;
  struct md_node *last_child;

  Mode user_data;
  char *data;
  int len;
  /* bufsize_t len; */

  int start_line;
  int start_column;
  int end_line;
  int end_column;
  NodeType type;
  uint16_t flags;

  int heading_level;
  char *code_language;
} md_node;

void print_node(md_node *node, int indent_level) {
  for (int i = 0; i < indent_level * 2; i++) {
    printf(" ");
  }
  if (node->data != NULL) {
    printf("data: %s\n", node->data);
  } else {
    printf("data: None\n");
  }

  switch (node->type) {
  case NODE_HEADING:
    for (int i = 0; i < indent_level * INDENTATION; i++) {
      printf(" ");
    }
    printf("level: %d\n", node->heading_level);
    break;
  case NODE_CODE:
    for (int i = 0; i < indent_level * 2; i++) {
      printf(" ");
    }
    printf("lang: %s\n", node->code_language);
    break;
  default:
    break;
  }
}

void print_type(md_node *node, int indent_level) {
  for (int i = 0; i < indent_level * INDENTATION; i++) {
    printf(" ");
  }
  switch (node->type) {
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
  case NODE_CODE:
    printf("code node\n");
    break;
  default:
    printf("unknown node\n");
    break;
  }
}

md_node *create_md_node(NodeType type, char *data, int len, md_node *next,
                        md_node *prev, md_node *parent, md_node *first_child,
                        md_node *last_child, Mode user_data) {
  md_node *node = (md_node *)malloc(sizeof(md_node));
  node->type = type;
  node->next = next;
  node->prev = prev;
  node->parent = parent;
  node->first_child = first_child;
  node->last_child = last_child;
  node->data = data;
  node->len = len;
  node->user_data = user_data;
  return node;
}

md_node *create_empty_md_node(NodeType type) {
  return create_md_node(type, NULL, 0, NULL, NULL, NULL, NULL, NULL, MODE_NONE);
}
