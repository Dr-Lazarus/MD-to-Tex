#ifndef TREE_H
#define TREE_H
#include "tree.h"
#endif
#include "parser/parser.h"
#include "renderer/renderer.h"
#include <regex.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

void printWelcomeMessage()
{
  printf("\n");
  printf("  ======================================\n");
  printf("     Welcome to the MD-to-LaTeX CLI\n");
  printf("  ======================================\n");
  printf("\n");
}

void printMenu()
{
  printf("Please select an option:\n");
  printf("  1. Convert MD to LaTeX\n");
  printf("  2. Exit\n");
  printf("\nEnter your choice: ");
}
int main(int argc, char **argv)
{
  FILE *logFile = fopen("program.log", "a");
  if (!logFile)
  {
    perror("Failed to open log file");
    return EXIT_FAILURE;
  }

  char *inputFileName = NULL;
  char *outputFileName = NULL;
  int menuChoice = 0;

  printWelcomeMessage();

  while (1)
  {
    printMenu();
    scanf("%d", &menuChoice);

    switch (menuChoice)
    {
    case 1:
      printf("Enter input MD file name: ");
      inputFileName = malloc(256); // Allocate memory for file name
      scanf("%s", inputFileName);

      // Generate the output file name based on the input file name
      char *dot = strrchr(inputFileName, '.');
      size_t outputFileNameLen = (dot ? (size_t)(dot - inputFileName) : strlen(inputFileName)) + 5; // +4 for ".tex" and +1 for null terminator
      outputFileName = malloc(outputFileNameLen);                                                   // Allocate memory for the output file name
      if (dot)
      {
        snprintf(outputFileName, outputFileNameLen, "%.*s.tex", (int)(dot - inputFileName), inputFileName);
      }
      else
      {
        snprintf(outputFileName, outputFileNameLen, "%s.tex", inputFileName);
      }

      if (strlen(outputFileName) == 0)
      {
        free(outputFileName);                  // Free if no input was provided
        outputFileName = strdup("output.tex"); // Use default name
      }

      md_node *document = parse_source(inputFileName);
      if (!document)
      {
        fprintf(stderr, "Failed to parse the document\n");
        free(inputFileName);
        free(outputFileName);
        return 1;
      }
      print_tree_data(document, 0);
      printf("Parsing done\n");

      FILE *output = fopen(outputFileName, "w");
      if (!output)
      {
        perror("Error opening output file");
        free_tree(document);
        free(inputFileName);
        free(outputFileName);
        return 1;
      }

      fprintf(output, "\\documentclass{article}\n\\begin{document}\n");
      traverse_ast(document, output);
      fprintf(output, "\\end{document}\n");

      fclose(output);
      free_tree(document);
      free(inputFileName);
      free(outputFileName);

      printf("Conversion completed successfully!\n");
      break;
    case 2:
      printf("Exiting...\n");
      return 0;
    default:
      printf("Invalid choice. Please enter 1 or 2.\n");
    }
  }

  return 0;
}