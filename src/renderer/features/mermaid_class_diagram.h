#ifndef NODE_H
#define NODE_H
#include "../../node.h"
#endif

#define MAX_CLASSES 50
#define MAX_RELATIONSHIPS 50
#define MAX_ATTRIBUTES 50
#define MAX_METHODS 50
#define MAX_ANNOTATIONS 5
#define MAX_LINE_LENGTH 256
#define ID_SIZE 50

typedef struct {
  int x, y;  // Position of the class in the layout
} Position;

typedef struct {
  char id[ID_SIZE];
  char title[ID_SIZE];
  char attributes[MAX_ATTRIBUTES][ID_SIZE];
  int attribute_count;
  char methods[MAX_METHODS][ID_SIZE];
  int method_count;
  char annotations[MAX_ANNOTATIONS][ID_SIZE];
  int annotation_count;
  Position position;
} ClassNode;

typedef struct {
  char fromId[ID_SIZE];
  char toId[ID_SIZE];
  char label[ID_SIZE];
  char type[ID_SIZE];  // Type of relationship e.g., inheritance, association
  char cardinality_from[ID_SIZE];  // Cardinality on the 'from' side
  char cardinality_to[ID_SIZE];    // Cardinality on the 'to' side
} ClassRelationship;

void parse_class_diagram_mermaid_code(const char *mermaidCode,
                                      ClassNode *classNodes,
                                      int *classNodeCount,
                                      ClassRelationship *relationships,
                                      int *relationshipCount);
void generate_latex_class_diagram(const ClassNode *classNodes,
                                  int classNodeCount,
                                  const ClassRelationship *relationships,
                                  int relationshipCount, FILE *output);
