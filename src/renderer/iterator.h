#ifndef NODE_H
#define NODE_H
#include "../node.h"
#endif

typedef struct md_iter {
  md_node current_node;
} md_iter;

typedef enum { EVENT_ENTER, EVENT_DONE } iter_event_type;

md_iter *md_iter_new(md_node *root);
iter_event_type md_iter_next(md_iter *iter);
md_node *md_iter_get_node(md_iter *iter);
