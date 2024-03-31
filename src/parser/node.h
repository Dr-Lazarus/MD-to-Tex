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

typedef struct cmark_node {
  struct cmark_node *next;
  struct cmark_node *prev;
  struct cmark_node *parent;
  struct cmark_node *first_child;
  struct cmark_node *last_child;

  void *user_data;
  unsigned char *data;
  /* bufsize_t len; */

  int start_line;
  int start_column;
  int end_line;
  int end_column;
  uint16_t type;
  uint16_t flags;

  /* union { */
  /*   cmark_list list; */
  /*   cmark_code code; */
  /*   cmark_heading heading; */
  /*   cmark_link linke; */
  /*   cmark_custom custom; */
  /*   int html_block_type; */
  /* } as; */

} cmark_node;
