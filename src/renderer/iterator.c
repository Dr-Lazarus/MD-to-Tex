#include "iterator.h"

int is_leaf(md_node *node) { return node->first_child == NULL; }

md_iter *md_iter_new(md_node *root) {
  if (root == NULL) {
    return NULL;
  }
  md_iter *result = (md_iter *)calloc(1, sizeof(md_iter));
  result->root = root;
  result->current.event_type = EVENT_NONE;
  result->current.node = NULL;
  result->next.event_type = EVENT_ENTER;
  result->next.node = root;

  return result;
}

iter_event_type md_iter_next(md_iter *iter) {
  iter_event_type event_type = iter->next.event_type;
  md_node *node = iter->next.node;

  iter->current.event_type = event_type;
  iter->current.node = node;

  if (event_type == EVENT_DONE) {
    return event_type;
  }

  /* roll forward to next item, setting both fields */
  if (event_type == EVENT_ENTER && !is_leaf(node)) {
    if (node->first_child == NULL) {
      /* stay on this node but exit */
      iter->next.event_type = EVENT_EXIT;
    } else {
      iter->next.event_type = EVENT_ENTER;
      iter->next.node = node->first_child;
    }
  } else if (node == iter->root) {
    /* don't move past root */
    iter->next.event_type = EVENT_DONE;
    iter->next.node = NULL;
  } else if (node->next) {
    iter->next.event_type = EVENT_ENTER;
    iter->next.node = node->next;
  } else if (node->parent) {
    iter->next.event_type = EVENT_EXIT;
    iter->next.node = node->parent;
  } else {
    // idk what this is for
    // assert(false);
    iter->next.event_type = EVENT_DONE;
    iter->next.node = NULL;
  }

  return event_type;
}

md_node *md_iter_get_node(md_iter *iter) { return iter->current.node; }
