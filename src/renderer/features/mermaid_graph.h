#ifndef NODE_H
#define NODE_H
#include "../../node.h"
#endif

#define MAX_NODES 256
#define MAX_EDGES 256
#define MAX_LEVELS 10

typedef struct {
    char fromNode;
    char toNode;
} Edge;

typedef struct {
    int x, y;
} Position;

typedef struct {
    char id; 
    int level;
} LayoutNode;

int parse_graph_line(char *line, char *fromNode, char *toNode);
void addNode(char *node_list, char node);
void parse_graph_mermaid_code(const char *mermaid_code, char *node_list, Edge *edge_list, int *edge_count);
void calculate_layout(char *node_list, Edge *edge_list, int edge_count, Position *positions);
void generate_latex_graph(char *node_list, Position *positions, Edge *edge_list, int edge_count, FILE *outputFile);
