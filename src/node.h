#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
/* 
 * Defined by the recommended line length for markdown.
 */
#define MAX_LINE_LENGTH 121
#define INDENTATION 2

typedef enum {
  NODE_BLOCK_QUOTE,
  NODE_CODE,
  NODE_CODE_BLOCK,
  NODE_MATH,
  NODE_MATH_BLOCK,
  NODE_EMPH,
  NODE_HEADING,  // currently ok
  NODE_HTML,     // we will not take this
  NODE_IMAGE,
  NODE_LINK,
  NODE_LIST,
  NODE_ITEM,
  NODE_PARAGRAPH,  // currently ok but not the details
  NODE_ROOT,       // currently in use for root
  NODE_STRONG,
  NODE_TEXT,  // currently the only type of text
  NODE_NONE
} NodeType;

typedef enum { LIST_BULLET, LIST_NUMBERED } ListType;

typedef enum {
  LINE_EMPTY,
  LINE_LISTITEM,
  LINE_HEADER,
  LINE_IMAGE,
  LINE_CODE_DELIM,
  LINE_MATH_DELIM,
  LINE_BLOCKQUOTE,
  LINE_TEXT
} LineType;

typedef enum {
  MODE_EMPTY,
  MODE_APPEND,
  MODE_STARTNEW,
  MODE_NONE,
  MODE_CODE,
  MODE_PROCESSED
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
  LineType prev_line_type;
  uint16_t flags;

  int heading_level;
  char *code_language;
  ListType list_type;
  char *url;
  int url_length;
  char *title;
  int title_length;
  char *mermaid_code;
  char delimiter;
  int list_number;
} md_node;

void print_node(md_node *node, int indent_level);

void print_type(md_node *node, int indent_level);

md_node *create_md_node(NodeType type, int start_line, int end_line,
                        int start_column, int end_column);

char *print_line_type(LineType type);
char *print_node_type(NodeType type);
md_node *create_empty_md_node(NodeType type, int start_line, int end_line);

NodeType md_node_get_type(md_node *node);
char *md_node_get_fence_info(md_node *node);
char *md_node_get_literal(md_node *node);
char *md_node_get_mermaid_code(md_node *node);
int md_node_get_heading_level(md_node *node);
ListType md_node_get_list_type(md_node *node);
char *md_node_get_url(md_node *node);
char *md_node_get_title(md_node *node);
