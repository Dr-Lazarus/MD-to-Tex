#ifndef NODE_H
#define NODE_H
#include "../node.h"
#endif

void traverse_ast(md_node *root, FILE *output);
void convert_paragraph(md_node *node, FILE *output, int entering);
void convert_text(md_node *node, FILE *output, int entering);
void convert_emph(md_node *node, FILE *output, int entering);
void convert_heading(md_node *node, FILE *output, int entering);
void convert_list(md_node *node, FILE *output, int entering);
void convert_item(md_node *node, FILE *output, int entering);
void convert_blockquote(md_node *node, FILE *output, int entering);
void convert_code_block(md_node *node, FILE *output, int entering);
void convert_code(md_node *node, FILE *output, int entering);
void convert_strong(md_node *node, FILE *output, int entering);
void convert_softbreak(md_node *node, FILE *output, int entering);
void convert_linebreak(md_node *node, FILE *output, int entering);
void convert_link(md_node *node, FILE *output, int entering);
void convert_image(md_node *node, FILE *output, int entering);
void convert_mermaid_diagram(md_node *node, FILE *output, int entering);
void convert_class_diagram(const char* mermaid_code, FILE* output);
void convert_sequence_diagram(const char* mermaid_code, FILE* output);
void convert_graph_diagram(const char* mermaid_code, FILE* output);
