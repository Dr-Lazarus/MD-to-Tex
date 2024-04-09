#ifndef NODE_H
#define NODE_H
#include "../node.h"
#endif

#define MAX_NODES 256
#define MAX_EDGES 256
#define MAX_LEVELS 10

typedef struct {
    char fromNode;
    char toNode;
} Edge;

typedef struct {
    int x, y; // Coordinates for the node
} Position;

typedef struct {
    char id; 
    int level;
} LayoutNode;

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
