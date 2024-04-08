#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// defined by the recommended line length for markdown
#define MAX_LINE_LENGTH 121
#define INDENTATION 2

typedef enum {
  NODE_BLOCK_QUOTE,
  NODE_BREAK,
  NODE_CODE,
  NODE_CODE_BLOCK,
  NODE_DEFINITION,
  NODE_EMPH,
  NODE_HEADING, // currently ok
  NODE_HTML,    // we will not take this
  NODE_IMAGE,
  NODE_IMAGEREFERENCE,
  NODE_INLINECODE,
  NODE_LINK,
  NODE_LINKREFERENCE,
  NODE_LIST,
  NODE_ITEM,
  NODE_PARAGRAPH, // currently ok but not the details
  NODE_ROOT,      // currently in use for root
  NODE_STRONG,
  NODE_TEXT, // currently the only type of text
  NODE_THEMATICBREAK,
  NODE_SOFTBREAK,
  NODE_LINEBREAK
} NodeType;

typedef enum { LIST_BULLET, LIST_NUMBERED } ListType;

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
  ListType list_type;
  char *url;
  char *title;
} md_node;

void print_node(md_node *node, int indent_level);

void print_type(md_node *node, int indent_level);

md_node *create_md_node(NodeType type, char *data, int len, md_node *next,
                        md_node *prev, md_node *parent, md_node *first_child,
                        md_node *last_child, Mode user_data);

md_node *create_empty_md_node(NodeType type);

NodeType md_node_get_type(md_node *node);
char *md_node_get_fence_info(md_node *node);
char *md_node_get_literal(md_node *node);
int md_node_get_heading_level(md_node *node);
ListType md_node_get_list_type(md_node *node);
char *md_node_get_url(md_node *node);
char *md_node_get_title(md_node *node);
