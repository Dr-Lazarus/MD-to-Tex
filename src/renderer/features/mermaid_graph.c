#include "mermaid_graph.h"
#include <ctype.h>
#include <stdio.h>
#include <string.h>

int parse_graph_line(char *line, char *fromNode, char *toNode) {
  char *arrowPtr = strstr(line, "-->");
  if (arrowPtr != NULL) {
    *arrowPtr = '\0';
    arrowPtr += 3; // Skips the arrow

    while (isspace((unsigned char)*line))
      line++; // Skips leading spaces in fromNode
    while (isspace((unsigned char)*(arrowPtr)))
      arrowPtr++; // Skips leading spaces in toNode

    char *end = arrowPtr;
    while (*end && *end != ';' && *end != '\n' && !isspace((unsigned char)*end))
      end++;
    *end = '\0';

    strcpy(fromNode, line);
    strcpy(toNode, arrowPtr);
    return 1; // Found edge weee
  }
  return 0; // Didn't find edge sad
}

void addNode(char *node_list, char node) {
  int len = strlen(node_list);
  for (int i = 0; i < len; i++) {
    if (node_list[i] == node)
      return; // Node already exists
  }
  node_list[len] = node; // Add new node
  node_list[len + 1] = '\0';
}

void parse_graph_mermaid_code(const char *mermaid_code, char *node_list,
                        Edge *edge_list, int *edge_count) {
  char line[MAX_LINE_LENGTH];
  char fromNode[2] = {0}, toNode[2] = {0};
  *edge_count = 0;
  const char *code_cursor = mermaid_code;
  char *line_cursor;

  while (*code_cursor != '\0') {
    line_cursor = line;

    // Reads the diagram line (processes beginning and ending of line)
    while (isspace((unsigned char)*code_cursor))
      code_cursor++;
    while (*code_cursor != '\0' && *code_cursor != '\n') {
      *line_cursor++ = *code_cursor++;
    }
    *line_cursor = '\0';

    // Skips first line
    if (strncmp(line, "graph", 5) == 0) {
      code_cursor++;
      continue;
    }

    // Parses the diagram to get fromNode and toNode
    if (parse_graph_line(line, fromNode, toNode)) {
      addNode(node_list, fromNode[0]);
      addNode(node_list, toNode[0]);

      edge_list[*edge_count].fromNode = fromNode[0];
      edge_list[*edge_count].toNode = toNode[0];
      (*edge_count)++;
    }

    // Skips to the next line
    if (*code_cursor != '\0')
      code_cursor++;
  }
}

void calculate_layout(char *node_list, Edge *edge_list, int edge_count,
                      Position *positions) {
  LayoutNode layoutNodes[MAX_NODES] = {0};
  int levelSizes[MAX_LEVELS] = {0}; // How many nodes on each level?

  // Initialize layoutNodes
  for (int i = 0; node_list[i] != '\0'; i++) {
    layoutNodes[i].id = node_list[i];
    layoutNodes[i].level = -1;
  }

  for (int i = 0; i < edge_count; i++) {
    char fromId = edge_list[i].fromNode;
    char toId = edge_list[i].toNode;

    int fromIndex = -1, toIndex = -1;
    for (int j = 0; layoutNodes[j].id != '\0'; j++) {
      if (layoutNodes[j].id == fromId)
        fromIndex = j;
      if (layoutNodes[j].id == toId)
        toIndex = j;
    }

    if (fromIndex != -1 && toIndex != -1) {
      if (layoutNodes[fromIndex].level == -1) {
        layoutNodes[fromIndex].level = 0;
        levelSizes[0]++;
      }
      layoutNodes[toIndex].level =
          layoutNodes[fromIndex].level + 1; // Next level
      levelSizes[layoutNodes[toIndex].level]++;
    }
  }

  for (int i = 0; layoutNodes[i].id != '\0'; i++) {
    positions[i].x = layoutNodes[i].level * 2;             // Horizontal spacing
    positions[i].y = levelSizes[layoutNodes[i].level] * 2; // Vertical spacing
    levelSizes[layoutNodes[i].level]--;
  }
}

void generate_latex_graph(char *node_list, Position *positions, Edge *edge_list,
                    int edge_count, FILE *outputFile) {
  fprintf(outputFile, "\\begin{tikzpicture}[>=stealth]\n");

  // Node
  for (int i = 0; node_list[i] != '\0'; i++) {
    fprintf(outputFile, "\\node (%c) at (%d, %d) {%c};\n", node_list[i],
            positions[i].x, positions[i].y, node_list[i]);
  }

  // Edge
  for (int i = 0; i < edge_count; i++) {
    fprintf(outputFile, "\\draw[->] (%c) -- (%c);\n", edge_list[i].fromNode,
            edge_list[i].toNode);
  }

  fprintf(outputFile, "\\end{tikzpicture}\n");
}