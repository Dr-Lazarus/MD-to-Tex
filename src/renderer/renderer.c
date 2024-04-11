#include "renderer.h"
#include "iterator.h"
#include "../renderer/features/mermaid_graph.h"
#include "../renderer/features/mermaid_class_diagram.h"
#include "../renderer/features/mermaid_seq_diagram.h"
#include "../renderer/features/mermaid_pie.h"
#ifndef CVEC_H
#define CVEC_H
#include "../util/cvector.h"
#endif
#include <ctype.h>
#include <stdio.h>
#include <string.h>

/*
 * Traverses the abstract syntax tree (AST) starting from the root node
 * and converts it into LaTeX format by calling specific conversion functions
 * based on the node type. The traversal is done using an iterator.
 */
void traverse_ast(md_node *root, FILE *output)
{
  iter_event_type ev_type;
  md_iter *iter = md_iter_new(root);

  while ((ev_type = md_iter_next(iter)) != EVENT_DONE)
  {
    md_node *node = md_iter_get_node(iter);
    int entering = ev_type == EVENT_ENTER;

    switch (md_node_get_type(node))
    {
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
          !strcmp(node->code_language, "mermaid"))
      {
        ev_type = md_iter_next(iter);
        node = md_iter_get_node(iter);
        entering = ev_type == EVENT_ENTER;
        convert_mermaid_diagram(node, output, entering);
        ev_type = md_iter_next(iter);
      }
      else
      {
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

/*
 * Converts a paragraph node into LaTeX. Ensures a blank line is inserted
 * at the end of a paragraph for proper formatting.
 */
void convert_paragraph(md_node *node, FILE *output, int entering)
{
  if (!entering)
  {
    fprintf(output, "\n\n");
  }
}

/*
 * Converts a text node into LaTeX, performing necessary escaping
 * for special LaTeX characters within the text.
 */
void convert_text(md_node *node, FILE *output, int entering)
{
  if (entering)
  {
    const char *text = md_node_get_literal(node);
    while (*text)
    {
      switch (*text)
      {
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
      default:
        fputc(*text, output);
        break;
      }
      text++;
    }
  }
}

/*
 * Wraps the content of an emphasis node with LaTeX emphasis formatting.
 */
void convert_emph(md_node *node, FILE *output, int entering)
{
  fprintf(output, entering ? "\\emph{" : "}");
}

/*
 * Converts a heading node into the appropriate LaTeX sectioning command
 * based on the heading level.
 */
void convert_heading(md_node *node, FILE *output, int entering)
{
  int level;

  fprintf(output, entering ? "\\emph{" : "}");
  level = md_node_get_heading_level(node);
  switch (level)
  {
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
    fprintf(output, entering ? "\\paragraph{" : "}\n");
    break;
  }
}

/*
 * Starts a LaTeX list environment (itemize or enumerate) when entering
 * a list node and ends it when exiting.
 */
void convert_list(md_node *node, FILE *output, int entering)
{
  ListType list_type = md_node_get_list_type(node);
  if (entering)
  {
    fprintf(output, list_type == LIST_BULLET ? "\\begin{itemize}\n"
                                             : "\\begin{enumerate}\n");
  }
  else
  {
    fprintf(output, list_type == LIST_BULLET ? "\\end{itemize}\n"
                                             : "\\end{enumerate}\n");
  }
}

/*
 * Converts an item node into a LaTeX item. It is used within list environments.
 */
void convert_item(md_node *node, FILE *output, int entering)
{
  if (entering)
  {
    fprintf(output, "\\item ");
  }
  else
  {
    fprintf(output, "\n");
  }
}

/*
 * Wraps the content of a blockquote node with LaTeX quote environment.
 */
void convert_blockquote(md_node *node, FILE *output, int entering)
{
  if (entering)
  {
    fprintf(output, "\\begin{quote}");
  }
  else
  {
    fprintf(output, "\\end{quote}\n");
  }
}

/*
 * Encloses a code block node content within a LaTeX verbatim environment
 * to preserve formatting and special characters.
 */
void convert_code_block(md_node *node, FILE *output, int entering)
{
  if (entering)
  {
    fprintf(output, "\n\\begin{verbatim}\n");
  }
  else
  {
    fprintf(output, "\n\\end{verbatim}\n");
  }
}

/*
 * Converts a code node into LaTeX using the texttt command to preserve
 * the typewriter font style.
 */
void convert_code(md_node *node, FILE *output, int entering)
{
  if (entering)
  {
    const char *code_text = md_node_get_literal(node);
    fprintf(output, "\\texttt{%s}", code_text ? code_text : "");
  }
}

/*
 * Wraps the content of a strong emphasis node with LaTeX bold formatting.
 */
void convert_strong(md_node *node, FILE *output, int entering)
{
  fprintf(output, entering ? "\\textbf{" : "}");
}

/*
 * Converts a softbreak node into a space in LaTeX. Softbreaks represent
 * spaces or soft line breaks in the source markdown.
 */
void convert_softbreak(md_node *node, FILE *output, int entering)
{
  fprintf(output, " ");
}

/*
 * Converts a linebreak node into a forced newline in LaTeX using the newline command.
 */
void convert_linebreak(md_node *node, FILE *output, int entering)
{
  fprintf(output, "\\newline\n");
}

/*
 * Converts a link node into LaTeX hyperref format, making the link clickable.
 */
void convert_link(md_node *node, FILE *output, int entering)
{
  if (entering)
  {
    const char *url = md_node_get_url(node);
    fprintf(output, "\\href{%s}{", url);
  }
  else
  {
    fprintf(output, "}");
  }
}

/*
 * Converts an image node into LaTeX figure environment, using includegraphics
 * for the image and caption for the alternative text.
 */
void convert_image(md_node *node, FILE *output, int entering)
{
  if (entering)
  {
    const char *url = md_node_get_url(node);
    const char *alt_text = md_node_get_title(node);
    fprintf(output,
            "\\begin{figure}[h]\\centering\\includegraphics{%s}\\caption{%s}"
            "\\end{figure}\n",
            url, alt_text ? alt_text : "");
  }
}

/*
 * Identifies the type of Mermaid diagram in a node and calls the appropriate
 * conversion function for Mermaid diagrams to LaTeX.
 */
void convert_mermaid_diagram(md_node *node, FILE *output, int entering)
{
  const char *mermaid_code;

  if (!entering)
    return;

  mermaid_code = md_node_get_mermaid_code(node);
  if (!mermaid_code)
    return;

  if (strstr(mermaid_code, "classDiagram") != NULL)
  {
    convert_class_diagram(mermaid_code, output);
  }
  else if (strstr(mermaid_code, "sequenceDiagram") != NULL)
  {
    convert_sequence_diagram(mermaid_code, output);
  }
  else if (strstr(mermaid_code, "pie title") != NULL)
  {
    convert_pie_chart(mermaid_code, output);
  }
  else
  {
    convert_graph_diagram(mermaid_code, output);
  }
}

/*
 * Converts a Mermaid sequence diagram into LaTeX.
 */
void convert_sequence_diagram(const char *mermaid_code, FILE *output) {
 Cvector *threads = cvec_init(0, sizeof(SeqThread));
 Cvector *messages = cvec_init(0, sizeof(SeqMessage));

 parse_seq_diagram_mermaid_code(mermaid_code, threads, messages);
 generate_seq_class_diagram(threads, messages, output);

 cvec_free(threads);
 cvec_free(messages);
}

/*
 * Parses and converts a Mermaid class diagram into LaTeX, generating a
 * class diagram using LaTeX commands based on the parsed structure.
 */
void convert_class_diagram(const char *mermaid_code, FILE *output)
{
  ClassNode classNodes[MAX_CLASSES];
  ClassRelationship relationships[MAX_RELATIONSHIPS];
  int classCount = 0, relCount = 0;

  parse_class_diagram_mermaid_code(mermaid_code, classNodes, &classCount, relationships, &relCount);
  generate_latex_class_diagram(classNodes, classCount, relationships, relCount, output);
}

void convert_pie_chart(const char *mermaid_code, FILE *output)
{
  char title[MAX_TITLE_LENGTH];
  PieSection sections[MAX_PIE_SECTIONS];
  int sectionCount = 0;

  parse_pie_chart_mermaid_code(mermaid_code, title, sections, &sectionCount);
  generate_latex_pie_chart(title, sections, sectionCount, output);
}

/*
 * Parses and converts a Mermaid graph diagram into LaTeX, generating a
 * graph layout using LaTeX commands based on the parsed structure.
 */
void convert_graph_diagram(const char *mermaid_code, FILE *output)
{
  char node_list[MAX_NODES] = {0};
  Edge edge_list[MAX_EDGES];
  int edge_count = 0;
  int i;
  Position positions[MAX_NODES];

  parse_graph_mermaid_code(mermaid_code, node_list, edge_list, &edge_count);

  printf("Node List: %s\n", node_list);
  for (i = 0; i < edge_count; i++)
  {
    printf("Edge: %c -> %c\n", edge_list[i].fromNode, edge_list[i].toNode);
  }

  calculate_layout(node_list, edge_list, edge_count, positions);
  generate_latex_graph(node_list, positions, edge_list, edge_count, output);
}
