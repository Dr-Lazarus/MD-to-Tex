#ifndef TREE_H
#define TREE_H
#include "parser/tree.h"
#endif
#include "parser/parser.h"
#include <regex.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char **argv) {
  if (argc != 2) {
    printf("No file specified");
    return 1;
  }
  md_node *root = parse_source(argv[1]);
  print_tree_data(root, 0);
  free_tree(root);
  free(root);
  return 0;
}
