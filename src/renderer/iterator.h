#ifndef TREE_H
#define TREE_H
#include "../tree.h"
#endif
#ifndef NODE_H
#define NODE_H
#include "../node.h"
#endif

typedef enum {
  EVENT_ENTER,
  EVENT_DONE,
  EVENT_EXIT,
  EVENT_NONE
} iter_event_type;

typedef struct {
  iter_event_type event_type;
  md_node *node;
} md_iter_state;

typedef struct md_iter {
  md_node *root;
  md_iter_state current;
  md_iter_state next;
} md_iter;

md_iter *md_iter_new(md_node *root);
iter_event_type md_iter_next(md_iter *iter);
md_node *md_iter_get_node(md_iter *iter);
int is_leaf(md_node *node);
