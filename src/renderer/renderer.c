#include "renderer.h"
#include "iterator.h"
#include <ctype.h>
#include <stdio.h>
#include <string.h>

void traverse_ast(md_node *root, FILE *output) {
  iter_event_type ev_type;
  md_iter *iter = md_iter_new(root);

  while ((ev_type = md_iter_next(iter)) != EVENT_DONE) {
    md_node *node = md_iter_get_node(iter);
    int entering = ev_type == EVENT_ENTER;
    printf("node type: %d\n", node->type);
    printf("node data: %s\n", node->data);
    printf("entering: %d\n", entering);

    ;
    switch (md_node_get_type(node)) {
    case NODE_PARAGRAPH:
      convert_paragraph(node, output, entering);
      break;
    case NODE_TEXT:
      convert_text(node, output, entering);
      break;
    case NODE_EMPH:
      convert_emph(node, output, entering);
      break;
    case NODE_HEADING:
      convert_heading(node, output, entering);
      break;
    case NODE_LIST:
      convert_list(node, output, entering);
      break;
    case NODE_ITEM:
      convert_item(node, output, entering);
      break;
    case NODE_BLOCK_QUOTE:
      convert_blockquote(node, output, entering);
      break;
    case NODE_CODE_BLOCK:
      if (node->code_language != NULL &&
          !strcmp(node->code_language, "mermaid")) {
        ev_type = md_iter_next(iter);
        node = md_iter_get_node(iter);
        entering = ev_type == EVENT_ENTER;
        printf("node type: %d\n", node->type);
        printf("node data: %s\n", node->data);
        printf("entering: %d\n", entering);
        convert_mermaid_diagram(node, output, entering);
        ev_type = md_iter_next(iter);
        ev_type = md_iter_next(iter);
      } else {

        convert_code_block(node, output, entering);
      }
      break;
    case NODE_CODE:
      convert_code(node, output, entering);
      break;
    case NODE_STRONG:
      convert_strong(node, output, entering);
      break;
    case NODE_SOFTBREAK:
      convert_softbreak(node, output, entering);
      break;
    case NODE_LINEBREAK:
      convert_linebreak(node, output, entering);
      break;
    case NODE_LINK:
      convert_link(node, output, entering);
      break;
    case NODE_IMAGE:
      convert_image(node, output, entering);
      break;
    case NODE_MERMAID_DIAGRAM:
      convert_mermaid_diagram(node, output, entering);
      break;
    default:
      break;
    }
  }
}

void convert_paragraph(md_node *node, FILE *output, int entering) {
  if (!entering) {
    fprintf(output, "\n\n"); // Ensure a blank line at the end of a paragraph
  }
}

void convert_text(md_node *node, FILE *output, int entering) {
  if (entering) {
    const char *text = md_node_get_literal(node);
    // Assuming a simplified version of latex escaping for demonstration
    while (*text) {
      switch (*text) {
      case '\\':
        fprintf(output, "\\textbackslash{}");
        break;
      case '{':
        fprintf(output, "\\{");
        break;
      case '}':
        fprintf(output, "\\}");
        break;
      case '$':
        fprintf(output, "\\$");
        break;
      // Add cases for other special characters here...
      default:
        fputc(*text, output);
        break;
      }
      text++;
    }
  }
}

void convert_emph(md_node *node, FILE *output, int entering) {
  fprintf(output, entering ? "\\emph{" : "}");
}

void convert_heading(md_node *node, FILE *output, int entering) {
  fprintf(output, entering ? "\\emph{" : "}");
  int level = md_node_get_heading_level(node);
  // Format the heading based on its level into LaTeX.
  switch (level) {
  case 1:
    fprintf(output, entering ? "\\section{" : "}\n");
    break;
  case 2:
    fprintf(output, entering ? "\\subsection{" : "}\n");
    break;
  case 3:
    fprintf(output, entering ? "\\subsubsection{" : "}\n");
    break;
  case 4:
    fprintf(output, entering ? "\\paragraph{" : "}\n");
    break;
  case 5:
    fprintf(output, entering ? "\\subparagraph{" : "}\n");
    break;
  default:
    // Handle unexpected heading levels, if any.
    fprintf(output, entering ? "\\paragraph{" : "}\n");
    break;
  }
}

void convert_list(md_node *node, FILE *output, int entering) {
  ListType list_type = md_node_get_list_type(node);
  if (entering) {
    fprintf(output, list_type == LIST_BULLET ? "\\begin{itemize}\n"
                                             : "\\begin{enumerate}\n");
  } else {
    fprintf(output, list_type == LIST_BULLET ? "\\end{itemize}\n"
                                             : "\\end{enumerate}\n");
  }
}

void convert_item(md_node *node, FILE *output, int entering) {
  if (entering) {
    // Directly start items without a leading paragraph command
    fprintf(output, "\\item ");
  } else {
    fprintf(output, "\n");
  }
  // No else case needed as items don't need explicit closing in LaTeX
}

void convert_blockquote(md_node *node, FILE *output, int entering) {
  if (entering) {
    fprintf(output, "\\begin{quote}");
  } else {
    fprintf(output, "\\end{quote}\n");
  }
}

void convert_code_block(md_node *node, FILE *output, int entering) {
  if (entering) {
    fprintf(output, "\n\\begin{verbatim}\n");
  } else {
    fprintf(output, "\n\\end{verbatim}\n");
  }
}

void convert_code(md_node *node, FILE *output, int entering) {
  if (entering) {
    // Fetch the code text and wrap it in \texttt
    const char *code_text = md_node_get_literal(node);
    fprintf(output, "\\texttt{%s}", code_text ? code_text : "");
  }
}

void convert_strong(md_node *node, FILE *output, int entering) {
  fprintf(output, entering ? "\\textbf{" : "}");
}

void convert_softbreak(md_node *node, FILE *output, int entering) {
  fprintf(output, " "); // Softbreaks can be converted to spaces
}

void convert_linebreak(md_node *node, FILE *output, int entering) {
  fprintf(output, "\\newline\n"); // Force a new line in LaTeX
}

void convert_link(md_node *node, FILE *output, int entering) {
  if (entering) {
    const char *url = md_node_get_url(node);
    fprintf(output, "\\href{%s}{", url);
  } else {
    fprintf(output, "}");
  }
}

void convert_image(md_node *node, FILE *output, int entering) {
  if (entering) {
    const char *url = md_node_get_url(node);
    const char *alt_text = md_node_get_title(node);
    fprintf(output,
            "\\begin{figure}[h]\\centering\\includegraphics{%s}\\caption{%s}"
            "\\end{figure}\n",
            url, alt_text ? alt_text : "");
  }
}

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

void parse_mermaid_code(const char *mermaid_code, char *node_list,
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

void generate_latex(char *node_list, Position *positions, Edge *edge_list,
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

void convert_mermaid_diagram(md_node *node, FILE *output, int entering) {
  if (!entering)
    return;

  const char *mermaid_code = md_node_get_mermaid_code(node);
  if (!mermaid_code)
    return;

  char node_list[MAX_NODES] = {0};
  Edge edge_list[MAX_EDGES];
  int edge_count = 0;
  Position positions[MAX_NODES]; // For each node

  // This updates node_list and edge_list
  parse_mermaid_code(mermaid_code, node_list, edge_list, &edge_count);

  printf("Node List: %s\n", node_list);
  for (int i = 0; i < edge_count; i++) {
    printf("Edge: %c -> %c\n", edge_list[i].fromNode, edge_list[i].toNode);
  }

  // This calculates the layout of the nodes
  calculate_layout(node_list, edge_list, edge_count, positions);

  generate_latex(node_list, positions, edge_list, edge_count, output);
}
