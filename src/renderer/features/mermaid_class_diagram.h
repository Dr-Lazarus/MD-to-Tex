#ifndef NODE_H
#define NODE_H
#include "../../node.h"
#include "mermaid_graph.h"
#endif

#define INITIAL_CAPACITY 5

typedef struct {
    char *relatedClassName;
    char *relationshipType;
    char *relationshipName;
} Relationship;

typedef struct {
    char *name;
    char **attributes;
    int attributesCount;
    int attributesCapacity;
    char **functions;
    int functionsCount;
    int functionsCapacity;
    Relationship *relationships;
    int relationshipsCount;
    int relationshipsCapacity;
} Class;

typedef struct {
    char fromNode;
    char toNode;
} ClassEdge;

typedef struct {
    int x, y;
} ClassPosition;

typedef struct {
    char id; 
    int level;
} ClassLayoutNode;

#define MAX_CLASSES 26

void calculate_class_layout(char *node_list, ClassEdge *edge_list, int edge_count, ClassPosition *positions);
char getClassId(int index);
void generate_latex_class_diagram(Class* classes, int classCount, ClassPosition* positions, FILE* outputFile);
void initializeClass(Class *cls);
void addAttribute(Class *cls, const char *attribute);
void addFunction(Class *cls, const char *function);
char* trim(char* str);
void extractClassData(char *input, Class *cls);
Class* findClassByName(Class* classes, int classCount, const char* name);
void addRelationshipToClass(Class* cls, const char* relatedClassName, const char* relationshipType, const char* relationshipName);
void parseRelationships(const char* input, Class* classes, int classCount);
void freeClass(Class *cls);

typedef struct {
    char fromId[50]; 
    char toId[50]; 
    char label[50];
} ClassRelationship;

void parse_class_diagram_mermaid_code(const char* mermaidCode, FILE *output);
// void generate_latex_class_diagram(const ClassNode* classNodes, int classNodeCount, const ClassRelationship* relationships, int relationshipCount, FILE* output);
