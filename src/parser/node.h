#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// defined by the recommended line length for markdown
#define MAX_LINE_LENGTH 121
#define INDENTATION 2

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

void print_node(md_node *node, int indent_level);

void print_type(md_node *node, int indent_level);

md_node *create_md_node(NodeType type, char *data, int len, md_node *next,
                        md_node *prev, md_node *parent, md_node *first_child,
                        md_node *last_child, Mode user_data);

md_node *create_empty_md_node(NodeType type);
