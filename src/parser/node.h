#include <stdint.h>

typedef enum {
  NODE_BLOCKQUOTE,
  NODE_BREAK,
  NODE_CODE,
  NODE_DEFINITION,
  NODE_EMPHASIS,
  NODE_HEADING,
  NODE_HTML,
  NODE_IMAGE,
  NODE_IMAGEREFERENCE,
  NODE_INLINECODE,
  NODE_LINK,
  NODE_LINKREFERENCE,
  NODE_LIST,
  NODE_LISTITEM,
  NODE_PARAGRAPH,
  NODE_ROOT,
  NODE_STRONG,
  NODE_TEXT,
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
  /* bufsize_t len; */

  int start_line;
  int start_column;
  int end_line;
  int end_column;
  NodeType type;
  uint16_t flags;

  int heading_level;
  /* union { */
  /*   cmark_list list; */
  /*   cmark_code code; */
  /*   cmark_heading heading; */
  /*   cmark_link linke; */
  /*   cmark_custom custom; */
  /*   int html_block_type; */
  /* } as; */

} md_node;
