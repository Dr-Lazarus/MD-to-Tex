#ifndef TREE_H
#define TREE_H
#include "tree.h"
#endif
#include <regex.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "parser/parser.h"
#include "renderer/renderer.h"

void printHelp() {
 printf("Usage: md_2_tex -i <inputfile> [-o <outputfile.tex>]\n");
 printf(
     "Convert Markdown files to LaTeX format by parsing the Markdown into an "
     "AST (Abstract Syntax Tree) and then rendering the LaTeX output.\n\n");
 printf("Options:\n");
 printf("  -i  Specify the input Markdown file.\n");
 printf(
     "  -o  Specify the output LaTeX file (optional). If not provided, the "
     "output filename is derived from the input filename with a .tex "
     "extension.\n");
 printf("  --h Show this help message.\n");
}

int adjustOutputFilename(char **filename, const char *newExt) {
 char *dot = strrchr(*filename, '.');
 if (!dot) {
  size_t currentLen = strlen(*filename);
  size_t newFilenameLen = currentLen + strlen(newExt) + 1;
  char *newFilename = malloc(newFilenameLen);
  if (!newFilename)
   return 0;
  snprintf(newFilename, newFilenameLen, "%s%s", *filename, newExt);
  free(*filename);
  *filename = newFilename;
  return 1;
 }

 size_t baseLen = dot - *filename;
 size_t newFilenameLen = baseLen + strlen(newExt) + 1;
 char *newFilename = malloc(newFilenameLen);
 if (!newFilename)
  return 0;
 snprintf(newFilename, newFilenameLen, "%.*s%s", (int)baseLen, *filename,
          newExt);
 free(*filename);
 *filename = newFilename;
 return 1;
}

int main(int argc, char **argv) {
 if (argc < 2) {
  printHelp();
  return EXIT_FAILURE;
 }

 char *inputFileName = NULL;
 char *outputFileName = NULL;
 int i;

 for (i = 1; i < argc; i++) {
  if (strcmp(argv[i], "--h") == 0) {
   printHelp();
   return 0;
  } else if (strcmp(argv[i], "-i") == 0) {
   if (i + 1 < argc) {
    inputFileName = strdup(argv[++i]);
   } else {
    fprintf(stderr, "Error: Input file name not specified!\n");
    return EXIT_FAILURE;
   }
  } else if (strcmp(argv[i], "-o") == 0) {
   if (i + 1 < argc) {
    outputFileName = strdup(argv[++i]);
   } else {
    fprintf(stderr, "Error: Output file name not specified!\n");
    return EXIT_FAILURE;
   }
  }
 }

 if (!inputFileName) {
  fprintf(stderr, "Error: No input file specified. Use -i <filename>\n");
  return EXIT_FAILURE;
 }

 if (!outputFileName) {
  outputFileName = strdup(inputFileName);
  if (!adjustOutputFilename(&outputFileName, ".tex")) {
   fprintf(stderr, "Failed to set output file name\n");
   free(inputFileName);
   free(outputFileName);
   return EXIT_FAILURE;
  }
 }

 md_node *document = parse_source(inputFileName);
 if (!document) {
  fprintf(stderr, "Failed to parse the document\n");
  free(inputFileName);
  free(outputFileName);
  return 1;
 }

 FILE *output = fopen(outputFileName, "w");
 if (!output) {
  perror("Error opening output file");
  free_tree(document);
  free(inputFileName);
  free(outputFileName);
  return 1;
 }

 fprintf(output, "\\documentclass{article}\n");
 fprintf(output, "\\usepackage{tikz}\n");
 fprintf(output, "\\usepackage{pgf-pie}\n");
 fprintf(output, "\\usepackage{pgf-umlsd}\n");
 fprintf(output, "\\begin{document}\n");
 traverse_ast(document, output);
 fprintf(output, "\\end{document}\n");

 fclose(output);
 free_tree(document);
 free(inputFileName);
 free(outputFileName);

 printf("Conversion completed successfully!\n");
 return 0;
}
