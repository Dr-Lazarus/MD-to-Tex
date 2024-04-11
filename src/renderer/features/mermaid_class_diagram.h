#ifndef NODE_H
#define NODE_H
#include "../../node.h"
#endif

#define MAX_CLASSES 50
#define MAX_RELATIONSHIPS 50

typedef struct {
    char id[50]; 
    char attributes[1024];
} ClassNode;

typedef struct {
    char fromId[50]; 
    char toId[50]; 
    char label[50];
} ClassRelationship;

void parse_class_diagram_mermaid_code(const char* mermaidCode, ClassNode* classNodes, int* classNodeCount, ClassRelationship* relationships, int* relationshipCount);
void generate_latex_class_diagram(const ClassNode* classNodes, int classNodeCount, const ClassRelationship* relationships, int relationshipCount, FILE* output);
