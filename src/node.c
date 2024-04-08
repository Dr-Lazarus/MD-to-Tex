#include "node.h"

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

NodeType md_node_get_type(md_node *node) { return node->type; }
char *md_node_get_fence_info(md_node *node) { return node->code_language; }
char *md_node_get_literal(md_node *node) { return node->data; }
int md_node_get_heading_level(md_node *node) { return node->heading_level; }

ListType md_node_get_list_type(md_node *node) { return node->list_type; }

char *md_node_get_url(md_node *node) { return node->url; }

char *md_node_get_title(md_node *node) { return node->title; }
