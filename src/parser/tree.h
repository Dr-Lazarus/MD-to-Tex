md_node *init_tree() {
  return create_md_node(NODE_ROOT, "root", 4, NULL, NULL, NULL, NULL, NULL,
                        MODE_EMPTY);
}

void print_tree_data(md_node *node, int indent_level) {
  if (node == NULL) {
    return;
  }
  print_type(node, indent_level);
  print_node(node, indent_level);
  for (int i = 0; i < indent_level * INDENTATION; i++) {
    printf(" ");
  }
  printf("--------\n");
  md_node *child;
  for (child = node->first_child; child != NULL; child = child->next) {
    print_tree_data(child, indent_level + 1);
  }
}

void append_to_root(md_node *root, md_node *new_child) {
  new_child->parent = root;
  if (root->last_child == NULL) {
    root->last_child = new_child;
    root->first_child = new_child;
  } else {
    root->last_child->next = new_child;
    new_child->prev = root->last_child;
    root->last_child = new_child;
  }
}

void link_children(md_node *dest, md_node *source) {
  dest->last_child->next = source->first_child;
  source->first_child->prev = dest->last_child;
  dest->last_child = source->last_child;

  for (md_node *start = source->first_child; start != NULL;
       start = start->next) {
    start->parent = dest;
  }
}

void free_tree(md_node *root) {
  if (root == NULL) {
    return;
  }

  // if there are children
  if (root->first_child != NULL) {
    for (md_node *child = root->first_child; child != NULL;
         child = child->next) {
      free_tree(child);
    }

    md_node *tmp;
    md_node *head = root->first_child;
    while (head != NULL) {
      tmp = head;
      head = head->next;

      if (tmp->data != NULL) {
        free(tmp->data);
      }
      free(tmp);
    }
  }
}
