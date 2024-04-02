#include "cmark.h"
#include "node.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void convert_paragraph(cmark_node *node, FILE *output, bool entering);
void convert_text(cmark_node *node, FILE *output, bool entering);
void convert_emph(cmark_node *node, FILE *output, bool entering);
void convert_heading(cmark_node *node, FILE *output, bool entering);
void convert_list(cmark_node *node, FILE *output, bool entering);
void convert_item(cmark_node *node, FILE *output, bool entering);
void convert_blockquote(cmark_node *node, FILE *output, bool entering);
void convert_code_block(cmark_node *node, FILE *output, bool entering);
void convert_code(cmark_node *node, FILE *output, bool entering);
void convert_strong(cmark_node *node, FILE *output, bool entering);
void convert_softbreak(cmark_node *node, FILE *output, bool entering);
void convert_linebreak(cmark_node *node, FILE *output, bool entering);
void convert_link(cmark_node *node, FILE *output, bool entering);
void convert_image(cmark_node *node, FILE *output, bool entering);

void traverse_ast(cmark_node *root, FILE *output) {
  cmark_event_type ev_type;
  cmark_iter *iter = cmark_iter_new(root);

  while ((ev_type = cmark_iter_next(iter)) != CMARK_EVENT_DONE) {
    cmark_node *node = cmark_iter_get_node(iter);
    bool entering = ev_type == CMARK_EVENT_ENTER;

    if (cmark_node_get_type(node) == CMARK_NODE_CODE_BLOCK &&
        strcmp(cmark_node_get_fence_info(node), "diagram") == 0) {
      convert_diagram(node, output, entering);
    } else {
      switch (cmark_node_get_type(node)) {
      case CMARK_NODE_PARAGRAPH:
        convert_paragraph(node, output, entering);
        break;
      case CMARK_NODE_TEXT:
        convert_text(node, output, entering);
        break;
      case CMARK_NODE_EMPH:
        convert_emph(node, output, entering);
        break;
      case CMARK_NODE_HEADING:
        convert_heading(node, output, entering);
        break;
      case CMARK_NODE_LIST:
        convert_list(node, output, entering);
        break;
      case CMARK_NODE_ITEM:
        convert_item(node, output, entering);
        break;
      case CMARK_NODE_BLOCK_QUOTE:
        convert_blockquote(node, output, entering);
        break;
      case CMARK_NODE_CODE_BLOCK:
        convert_code_block(node, output, entering);
        break;
      case CMARK_NODE_CODE:
        convert_code(node, output, entering);
        break;
      case CMARK_NODE_STRONG:
        convert_strong(node, output, entering);
        break;
      case CMARK_NODE_SOFTBREAK:
        convert_softbreak(node, output, entering);
        break;
      case CMARK_NODE_LINEBREAK:
        convert_linebreak(node, output, entering);
        break;
      case CMARK_NODE_LINK:
        convert_link(node, output, entering);
        break;
      case CMARK_NODE_IMAGE:
        convert_image(node, output, entering);
        break;
      default:
        break;
      }
    }
  }
  cmark_iter_free(iter);
}

void convert_paragraph(cmark_node *node, FILE *output, bool entering) {
  if (!entering) {
    fprintf(output, "\n\n"); // Ensure a blank line at the end of a paragraph
  }
}

void convert_text(cmark_node *node, FILE *output, bool entering) {
  if (entering) {
    const char *text = cmark_node_get_literal(node);
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

void convert_emph(cmark_node *node, FILE *output, bool entering) {
  fprintf(output, entering ? "\\emph{" : "}");
}

void convert_heading(cmark_node *node, FILE *output, bool entering) {
  if (!entering) {
    return; // Only process the heading when entering.
  }

  // Prepare a buffer for the heading text. Adjust size as needed.
  char buffer[1024] = {0};

  // Fetch the heading text by concatenating the text of child nodes.
  cmark_iter *iter = cmark_iter_new(node);
  cmark_event_type ev_type;
  cmark_node *cur_node;
  while ((ev_type = cmark_iter_next(iter)) != CMARK_EVENT_DONE) {
    cur_node = cmark_iter_get_node(iter);
    if (cmark_node_get_type(cur_node) == CMARK_NODE_TEXT &&
        ev_type == CMARK_EVENT_ENTER) {
      // Ensure not to overflow the buffer.
      if (strlen(buffer) + strlen(cmark_node_get_literal(cur_node)) <
          sizeof(buffer) - 1) {
        strcat(buffer, cmark_node_get_literal(cur_node));
      }
    }
  }
  cmark_iter_free(iter);

  int level = cmark_node_get_heading_level(node);
  // Format the heading based on its level into LaTeX.
  switch (level) {
  case 1:
    fprintf(output, "\\section{%s}\n", buffer);
    break;
  case 2:
    fprintf(output, "\\subsection{%s}\n", buffer);
    break;
  case 3:
    fprintf(output, "\\subsubsection{%s}\n", buffer);
    break;
  case 4:
    fprintf(output, "\\paragraph{%s}\n", buffer);
    break;
  case 5:
    fprintf(output, "\\subparagraph{%s}\n", buffer);
    break;
  default:
    // Handle unexpected heading levels, if any.
    fprintf(output, "\\paragraph{%s}\n", buffer);
    break;
  }
}

void convert_list(cmark_node *node, FILE *output, bool entering) {
  cmark_list_type list_type = cmark_node_get_list_type(node);
  if (entering) {
    fprintf(output, list_type == CMARK_BULLET_LIST ? "\\begin{itemize}\n"
                                                   : "\\begin{enumerate}\n");
  } else {
    fprintf(output, list_type == CMARK_BULLET_LIST ? "\\end{itemize}\n"
                                                   : "\\end{enumerate}\n");
  }
}

void convert_item(cmark_node *node, FILE *output, bool entering) {
  if (entering) {
    // Directly start items without a leading paragraph command
    fprintf(output, "\\item ");
  }
  // No else case needed as items don't need explicit closing in LaTeX
}

void convert_blockquote(cmark_node *node, FILE *output, bool entering) {
  if (entering) {
    fprintf(output, "\\begin{quote}");
  } else {
    fprintf(output, "\\end{quote}\n");
  }
}

// void convert_code_block(cmark_node *node, FILE *output, bool entering) {
//     if (entering) {
//         fprintf(output, "\n\\begin{verbatim}\n");
//     } else {
//         fprintf(output, "\\end{verbatim}\n");
//     }
// }

void convert_code(cmark_node *node, FILE *output, bool entering) {
  if (entering) {
    // Fetch the code text and wrap it in \texttt
    const char *code_text = cmark_node_get_literal(node);
    fprintf(output, "\\texttt{%s}", code_text ? code_text : "");
  }
}

void convert_strong(cmark_node *node, FILE *output, bool entering) {
  fprintf(output, entering ? "\\textbf{" : "}");
}

void convert_softbreak(cmark_node *node, FILE *output, bool entering) {
  fprintf(output, " "); // Softbreaks can be converted to spaces
}

void convert_linebreak(cmark_node *node, FILE *output, bool entering) {
  fprintf(output, "\\newline\n"); // Force a new line in LaTeX
}

void convert_link(cmark_node *node, FILE *output, bool entering) {
  if (entering) {
    const char *url = cmark_node_get_url(node);
    fprintf(output, "\\href{%s}{", url);
  } else {
    fprintf(output, "}");
  }
}

void convert_image(cmark_node *node, FILE *output, bool entering) {
  if (entering) {
    const char *url = cmark_node_get_url(node);
    const char *alt_text =
        cmark_node_get_title(node); // Use title as alt text if available
    fprintf(output,
            "\\begin{figure}[h]\\centering\\includegraphics{%s}\\caption{%s}"
            "\\end{figure}\n",
            url, alt_text ? alt_text : "");
  }
  // No need to handle exiting since images are self-contained
}

int main(int argc, char **argv) {
  if (argc < 2) {
    fprintf(stderr, "Usage: %s <markdown_file>\n", argv[0]);
    return 1;
  }

  // Open the Markdown file
  FILE *file = fopen(argv[1], "r");
  if (!file) {
    perror("Error opening file");
    return 1;
  }

  // Read the content of the file into a string
  fseek(file, 0, SEEK_END);
  long length = ftell(file);
  fseek(file, 0, SEEK_SET);
  char *input_markdown = malloc(length + 1);
  if (!input_markdown) {
    perror("Memory allocation failed");
    fclose(file);
    return 1;
  }
  fread(input_markdown, 1, length, file);
  fclose(file);
  input_markdown[length] = '\0';

  // Convert the Markdown to a document tree
  cmark_node *document =
      cmark_parse_document(input_markdown, length, CMARK_OPT_DEFAULT);
  free(input_markdown);

  // Prepare the LaTeX output file
  FILE *output = fopen("output.tex", "w");
  if (!output) {
    perror("Error opening output file");
    cmark_node_free(document);
    return 1;
  }

  // Write the LaTeX document preamble
  fprintf(output, "\\documentclass{article}\n\\begin{document}\n");

  // Traverse the document tree and convert to LaTeX
  traverse_ast(document, output);

  // Write the LaTeX document end
  fprintf(output, "\\end{document}\n");

  // Clean up
  cmark_node_free(document);
  fclose(output);

  return 0;
}
