#include "renderer.h"
#include <ctype.h>
#include <stdio.h>
#include <string.h>

#include "../renderer/features/mermaid_class_diagram.h"
#include "../renderer/features/mermaid_graph.h"
#include "../renderer/features/mermaid_seq_diagram.h"
#ifndef CVEC_H
#define CVEC_H
#include "../util/cvector.h"
#endif
#include "iterator.h"

void traverse_ast(md_node *root, FILE *output) {
 iter_event_type ev_type;
 md_iter *iter = md_iter_new(root);

 while ((ev_type = md_iter_next(iter)) != EVENT_DONE) {
  md_node *node = md_iter_get_node(iter);
  int entering = ev_type == EVENT_ENTER;

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
  fprintf(output, "\n\n");  // Ensure a blank line at the end of a paragraph
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
  fprintf(output,
          list_type == LIST_BULLET ? "\\end{itemize}\n" : "\\end{enumerate}\n");
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
 fprintf(output, " ");  // Softbreaks can be converted to spaces
}

void convert_linebreak(md_node *node, FILE *output, int entering) {
 fprintf(output, "\\newline\n");  // Force a new line in LaTeX
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

void convert_mermaid_diagram(md_node *node, FILE *output, int entering) {
 if (!entering)
  return;

 const char *mermaid_code = md_node_get_mermaid_code(node);
 if (!mermaid_code)
  return;

 if (strstr(mermaid_code, "classDiagram") != NULL) {
  convert_class_diagram(mermaid_code, output);
 } else if (strstr(mermaid_code, "sequenceDiagram") != NULL) {
  convert_sequence_diagram(mermaid_code, output);
 } else {
  convert_graph_diagram(mermaid_code, output);
 }
}

void convert_sequence_diagram(const char *mermaid_code, FILE *output) {
 Cvector *threads = cvec_init(0, sizeof(SeqThread));
 Cvector *messages = cvec_init(0, sizeof(SeqMessage));

 parse_seq_diagram_mermaid_code(mermaid_code, threads, messages);
 generate_seq_class_diagram(threads, messages, output);

 cvec_free(threads);
 cvec_free(messages);
}

void convert_class_diagram(const char *mermaid_code, FILE *output) {
 ClassNode classNodes[MAX_CLASSES];
 ClassRelationship relationships[MAX_RELATIONSHIPS];
 int classCount = 0, relCount = 0;

 parse_class_diagram_mermaid_code(mermaid_code, classNodes, &classCount,
                                  relationships, &relCount);
 generate_latex_class_diagram(classNodes, classCount, relationships, relCount,
                              output);
}

void convert_graph_diagram(const char *mermaid_code, FILE *output) {
 char node_list[MAX_NODES] = {0};
 Edge edge_list[MAX_EDGES];
 int edge_count = 0;
 Position positions[MAX_NODES];  // For each node

 // This updates node_list and edge_list
 parse_graph_mermaid_code(mermaid_code, node_list, edge_list, &edge_count);

 printf("Node List: %s\n", node_list);
 for (int i = 0; i < edge_count; i++) {
  printf("Edge: %c -> %c\n", edge_list[i].fromNode, edge_list[i].toNode);
 }

 // This calculates the layout of the nodes
 calculate_layout(node_list, edge_list, edge_count, positions);
 generate_latex_graph(node_list, positions, edge_list, edge_count, output);
}