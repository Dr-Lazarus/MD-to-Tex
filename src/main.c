#ifndef TREE_H
#define TREE_H
#include "tree.h"
#endif
#include "parser/parser.h"
#include "renderer/renderer.h"
#include <regex.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char **argv) {
  if (argc != 2) {
    printf("No file specified");
    return 1;
  }
  md_node *document = parse_source(argv[1]);
  print_tree_data(document, 0);
  printf("parsing done\n");

  // Prepare the LaTeX output file
  FILE *output = fopen("output.tex", "w");
  if (!output) {
    perror("Error opening output file");
    free_tree(document);
    return 1;
  }

  // Write the LaTeX document preamble
  fprintf(output, "\\documentclass{article}\n\\begin{document}\n");

  // Traverse the document tree and convert to LaTeX
  traverse_ast(document, output);

  // Write the LaTeX document end
  fprintf(output, "\\end{document}\n");

  // Clean up
  fclose(output);

  free_tree(document);

  free(document);
  return 0;
}
