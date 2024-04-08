#include "renderer.h"
#include "iterator.h"
#include <stdio.h>
#include <string.h>

void traverse_ast(md_node *root, FILE *output) {
  iter_event_type ev_type;
  md_iter *iter = md_iter_new(root);

  while ((ev_type = md_iter_next(iter)) != EVENT_DONE) {
    md_node *node = md_iter_get_node(iter);
    int entering = ev_type == EVENT_ENTER;

    if (md_node_get_type(node) == NODE_CODE_BLOCK &&
        strcmp(md_node_get_fence_info(node), "diagram") == 0) {
      // convert_diagram(node, output, entering);
    } else {
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
        md_iter_next(iter);
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
        convert_code_block(node, output, entering);
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
      default:
        break;
      }
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
  if (!entering) {
    return; // Only process the heading when entering.
  }

  // Prepare a buffer for the heading text. Adjust size as needed.
  char buffer[1024] = {0};

  // Fetch the heading text by concatenating the text of child nodes.
  md_iter *iter = md_iter_new(node);
  iter_event_type ev_type;
  md_node *cur_node;
  while ((ev_type = md_iter_next(iter)) != EVENT_DONE) {
    cur_node = md_iter_get_node(iter);
    if (md_node_get_type(cur_node) == NODE_TEXT && ev_type == EVENT_ENTER) {
      // Ensure not to overflow the buffer.
      if (strlen(buffer) + strlen(md_node_get_literal(cur_node)) <
          sizeof(buffer) - 1) {
        strcat(buffer, md_node_get_literal(cur_node));
      }
    }
  }
  int level = md_node_get_heading_level(node);
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
    const char *alt_text =
        md_node_get_title(node); // Use title as alt text if available
    fprintf(output,
            "\\begin{figure}[h]\\centering\\includegraphics{%s}\\caption{%s}"
            "\\end{figure}\n",
            url, alt_text ? alt_text : "");
  }
  // No need to handle exiting since images are self-contained
}
