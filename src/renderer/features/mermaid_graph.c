#include "mermaid_graph.h"
#include <ctype.h>
#include <stdio.h>
#include <string.h>

/* 
 * Parses a single line of Mermaid graph code to extract the 'from' and 'to' nodes
 * of a directed edge. Updates the provided character pointers to point to the 'from'
 * and 'to' nodes if an edge is found.
 * Parameters:
 * - line: A string representing a line from a Mermaid graph code.
 * - fromNode: A pointer to a character where the 'from' node identifier will be stored.
 * - toNode: A pointer to a character where the 'to' node identifier will be stored.
 * Returns:
 * - 1 if an edge is successfully found and parsed, 0 otherwise.
 */
int parse_graph_line(char *line, char *fromNode, char *toNode) {
  char *arrowPtr = strstr(line, "-->");
  if (arrowPtr != NULL) {
    *arrowPtr = '\0';
    arrowPtr += 3;

    while (isspace((unsigned char)*line))
      line++;
    while (isspace((unsigned char)*(arrowPtr)))
      arrowPtr++; 

    char *end = arrowPtr;
    while (*end && *end != ';' && *end != '\n' && !isspace((unsigned char)*end))
      end++;
    *end = '\0';

    strcpy(fromNode, line);
    strcpy(toNode, arrowPtr);
    return 1;
  }
  return 0; 
}

/* 
 * Adds a node to the node list if it does not already exist in the list.
 * Parameters:
 * - node_list: A string representing the list of nodes found so far.
 * - node: The node character to add to the list.
 */
void addNode(char *node_list, char node) {
  int len = strlen(node_list);
  for (int i = 0; i < len; i++) {
    if (node_list[i] == node)
      return; 
  }
  node_list[len] = node; 
  node_list[len + 1] = '\0';
}

/* 
 * Parses Mermaid graph code to extract all nodes and edges, populating the provided
 * node list and edge list arrays. It updates the edge count to reflect the number of
 * edges found.
 * Parameters:
 * - mermaid_code: A string containing the Mermaid graph code.
 * - node_list: A character array to be filled with unique node identifiers.
 * - edge_list: An array of Edge structures to be filled with the found edges.
 * - edge_count: A pointer to an integer to be updated with the count of edges found.
 */
void parse_graph_mermaid_code(const char *mermaid_code, char *node_list,
                        Edge *edge_list, int *edge_count) {
  char line[MAX_LINE_LENGTH];
  char fromNode[2] = {0}, toNode[2] = {0};
  *edge_count = 0;
  const char *code_cursor = mermaid_code;
  char *line_cursor;

  while (*code_cursor != '\0') {
    line_cursor = line;

    while (isspace((unsigned char)*code_cursor))
      code_cursor++;
    while (*code_cursor != '\0' && *code_cursor != '\n') {
      *line_cursor++ = *code_cursor++;
    }
    *line_cursor = '\0';

    if (strncmp(line, "graph", 5) == 0) {
      code_cursor++;
      continue;
    }

    if (parse_graph_line(line, fromNode, toNode)) {
      addNode(node_list, fromNode[0]);
      addNode(node_list, toNode[0]);

      edge_list[*edge_count].fromNode = fromNode[0];
      edge_list[*edge_count].toNode = toNode[0];
      (*edge_count)++;
    }

    if (*code_cursor != '\0')
      code_cursor++;
  }
}

/* 
 * Calculates the layout positions for each node in the graph for visualization
 * purposes, considering the graph's structure defined by nodes and edges.
 * Parameters:
 * - node_list: A string of unique node identifiers.
 * - edge_list: An array of Edge structures representing the edges of the graph.
 * - edge_count: The number of edges in edge_list.
 * - positions: An array of Position structures to be filled with the calculated positions of each node.
 */
void calculate_layout(char *node_list, Edge *edge_list, int edge_count,
                      Position *positions) {
  LayoutNode layoutNodes[MAX_NODES] = {0};
  int levelSizes[MAX_LEVELS] = {0}; 

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
          layoutNodes[fromIndex].level + 1; 
      levelSizes[layoutNodes[toIndex].level]++;
    }
  }

  for (int i = 0; layoutNodes[i].id != '\0'; i++) {
    positions[i].x = layoutNodes[i].level * 2;             
    positions[i].y = levelSizes[layoutNodes[i].level] * 2; 
    levelSizes[layoutNodes[i].level]--;
  }
}

/* 
 * Generates a LaTeX representation of the graph using TikZ package, based on the
 * node positions and edges information provided. Outputs the LaTeX code to the given
 * file stream.
 * Parameters:
 * - node_list: A string of unique node identifiers.
 * - positions: An array of Position structures representing the positions of each node.
 * - edge_list: An array of Edge structures representing the edges of the graph.
 * - edge_count: The number of edges in edge_list.
 * - outputFile: A file stream where the generated LaTeX code is written.
 */
void generate_latex_graph(char *node_list, Position *positions, Edge *edge_list,
                    int edge_count, FILE *outputFile) {
  fprintf(outputFile, "\\begin{tikzpicture}[>=stealth]\n");

  for (int i = 0; node_list[i] != '\0'; i++) {
    fprintf(outputFile, "\\node (%c) at (%d, %d) {%c};\n", node_list[i],
            positions[i].x, positions[i].y, node_list[i]);
  }

  for (int i = 0; i < edge_count; i++) {
    fprintf(outputFile, "\\draw[->] (%c) -- (%c);\n", edge_list[i].fromNode,
            edge_list[i].toNode);
  }

  fprintf(outputFile, "\\end{tikzpicture}\n");
}
