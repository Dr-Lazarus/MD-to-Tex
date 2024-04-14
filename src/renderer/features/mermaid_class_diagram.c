#include "mermaid_class_diagram.h"
#include <ctype.h>
#include <stdio.h>
#include <string.h>

void parse_class_diagram_mermaid_code(const char *mermaidCode, FILE *output) {
  char *diagramStart = strstr(mermaidCode, "classDiagram");
  if (!diagramStart) {
      printf("No 'classDiagram' marker found.\n");
      return;
  }
  
  diagramStart += strlen("classDiagram");

  char *classStart = strstr(diagramStart, "class");
  if (!classStart) {
      printf("No class definitions found.\n");
      return;
  }

  size_t relationshipSectionLength = classStart - mermaidCode;
  char *relationshipSection = (char *)malloc(relationshipSectionLength + 1);
  if (!relationshipSection) {
      perror("Failed to allocate memory for relationship section");
      return;
  }
  strncpy(relationshipSection, mermaidCode, relationshipSectionLength);
  relationshipSection[relationshipSectionLength] = '\0';

  char *input = classStart;
  int classCount = 0;
  Class classes[10]; 

  while (input != NULL && classCount < 10) {
      initializeClass(&classes[classCount]);
      char *nextClassPosition = strstr(input + 5, "class"); 
      if (nextClassPosition != NULL) {
          *nextClassPosition = '\0';
      }
      extractClassData(input, &classes[classCount]);
      classCount++;
      if (nextClassPosition != NULL) {
          *nextClassPosition = 'c'; 
      }
      input = nextClassPosition;
  }

  parseRelationships(relationshipSection, classes, classCount);
  free(relationshipSection); 

  char node_list[MAX_CLASSES + 1] = {0}; 
  ClassEdge edge_list[MAX_CLASSES * MAX_CLASSES] = {0}; 
  ClassPosition positions[MAX_CLASSES] = {0};
  int edge_count = 0;

  for (int i = 0; i < classCount; i++) {
      node_list[i] = getClassId(i);
      for (int j = 0; j < classes[i].relationshipsCount; j++) {
          int targetIndex = -1;
          for (int k = 0; k < classCount; k++) {
              if (strcmp(classes[k].name, classes[i].relationships[j].relatedClassName) == 0) {
                  targetIndex = k;
                  break;
              }
          }
          if (targetIndex != -1) {
              edge_list[edge_count].fromNode = getClassId(i);
              edge_list[edge_count].toNode = getClassId(targetIndex);
              edge_count++;
          }
      }
  }

  calculate_class_layout(node_list, edge_list, edge_count, positions);

  generate_latex_class_diagram(classes, classCount, positions, output);

  for (int i = 0; i < classCount; i++) {
      freeClass(&classes[i]);
  } 
}

/* 
 * Calculates the layout positions for each node in the graph for visualization
 * purposes, considering the graph's structure defined by nodes and edges.
 * Parameters:
 * - node_list: A string of unique node identifiers.
 * - edge_list: An array of Edge structures representing the edges of the graph.
 * - edge_count: The number of edges in edge_list.
 * - positions: An array of Position structures to be filled with the calculated positions of each node.
 */
void calculate_class_layout(char *node_list, ClassEdge *edge_list, int edge_count, ClassPosition *positions) {
 ClassLayoutNode layoutNodes[MAX_NODES] = {0};
 int levelSizes[MAX_LEVELS] = {0};

 for (int i = 0; node_list[i] != '\0'; i++) {
  layoutNodes[i].id = node_list[i];
  layoutNodes[i].level = -1;
 }

 for (int i = 0; i < edge_count; i++) {
  char fromId = edge_list[i].fromNode;
  char toId = edge_list[i].toNode;

  int fromIndex = -1, toIndex = -1;
  for (int j = 0; layoutNodes[j].id != '\0'; j++) {
   if (layoutNodes[j].id == fromId)
    fromIndex = j;
   if (layoutNodes[j].id == toId)
    toIndex = j;
  }

  if (fromIndex != -1 && toIndex != -1) {
   if (layoutNodes[fromIndex].level == -1) {
    layoutNodes[fromIndex].level = 0;
    levelSizes[0]++;
   }
   layoutNodes[toIndex].level = layoutNodes[fromIndex].level + 1;
   levelSizes[layoutNodes[toIndex].level]++;
  }
 }

 for (int i = 0; layoutNodes[i].id != '\0'; i++) {
  positions[i].x = layoutNodes[i].level * 5;
  positions[i].y = levelSizes[layoutNodes[i].level] * 5;
  levelSizes[layoutNodes[i].level]--;
 }
}

char getClassId(int index) {
    return 'A' + index; 
}

void generate_latex_class_diagram(Class* classes, int classCount, ClassPosition* positions, FILE* outputFile) {
    fprintf(outputFile, "\\begin{tikzpicture}[node distance=1.5cm]\n");

    for (int i = 0; i < classCount; i++) {
        fprintf(outputFile, "\\umlclass[x=%d, y=%d]{%s}{\n", positions[i].x, -positions[i].y, classes[i].name);
        
        for (int j = 0; j < classes[i].attributesCount; j++) {
            fprintf(outputFile, "%s\\\\" , classes[i].attributes[j]);
        }
        fprintf(outputFile, "}{");
        for (int j = 0; j < classes[i].functionsCount; j++) {
            fprintf(outputFile, "%s\\\\" , classes[i].functions[j]);
        }
        fprintf(outputFile, "}\n");
    }

    for (int i = 0; i < classCount; i++) {
        for (int j = 0; j < classes[i].relationshipsCount; j++) {
            const char* relDescriptor = classes[i].relationships[j].relationshipName;
            const char* relatedClass = classes[i].relationships[j].relatedClassName;
            if (strstr(relDescriptor, "contains") != NULL) {
                fprintf(outputFile, "\\umlcompo{%s}{%s}\n", classes[i].name, relatedClass);
            } else if (strstr(relDescriptor, "has") != NULL) {
                fprintf(outputFile, "\\umlaggreg{%s}{%s}\n", classes[i].name, relatedClass);
            } else if (strstr(relDescriptor, "inherits") != NULL || strstr(relDescriptor, "extends") != NULL) {
                fprintf(outputFile, "\\umlinherit{%s}{%s}\n", classes[i].name, relatedClass);
            } else {
                fprintf(outputFile, "\\umlassoc{%s}{%s}\n", classes[i].name, relatedClass);
            }
        }
    }
    fprintf(outputFile, "\\end{tikzpicture}\n");
}

void initializeClass(Class *cls) {
    cls->name = NULL;
    cls->attributesCapacity = INITIAL_CAPACITY;
    cls->attributes = malloc(cls->attributesCapacity * sizeof(char*));
    cls->attributesCount = 0;
    cls->functionsCapacity = INITIAL_CAPACITY;
    cls->functions = malloc(cls->functionsCapacity * sizeof(char*));
    cls->functionsCount = 0;
    cls->relationshipsCapacity = INITIAL_CAPACITY;
    cls->relationships = malloc(cls->relationshipsCapacity * sizeof(Relationship));
    cls->relationshipsCount = 0;
}

void addAttribute(Class *cls, const char *attribute) {
  if (attribute[0] == '+') {
        attribute++; 
  }
  if (cls->attributesCount == cls->attributesCapacity) {
      cls->attributesCapacity *= 2;
      cls->attributes = realloc(cls->attributes, cls->attributesCapacity * sizeof(char*));
  }
  cls->attributes[cls->attributesCount++] = strdup(attribute);
}

void addFunction(Class *cls, const char *function) {
  if (function[0] == '+') {
        function++; 
  }
  if (cls->functionsCount == cls->functionsCapacity) {
      cls->functionsCapacity *= 2;
      cls->functions = realloc(cls->functions, cls->functionsCapacity * sizeof(char*));
  }
  cls->functions[cls->functionsCount++] = strdup(function);
}

char* trim(char* str) {
    char *end;

    while(isspace((unsigned char)*str)) str++;

    if(*str == 0) {
        return str;
    }

    end = str + strlen(str) - 1;
    while(end > str && isspace((unsigned char)*end)) end--;

    *(end+1) = 0;

    return str;
}

void extractClassData(char *input, Class *cls) {
    char *nameStart = strstr(input, "class ");
    if (nameStart != NULL) {
        nameStart += 6; 
    } else {
        return; 
    }

    char *bracePosition = strchr(nameStart, '{');
    if (bracePosition != NULL) {
        int nameLength = bracePosition - nameStart;
        cls->name = (char *)malloc(nameLength + 1);
        strncpy(cls->name, nameStart, nameLength);
        cls->name[nameLength] = '\0'; 
        cls->name = trim(cls->name);
    } else {
        return; 
    }

    char *contentStart = bracePosition + 1;
    char *line = strtok(contentStart, "\n");

    while (line) {
        line = trim(line);

        if (line[0] == '}' || line[0] == '\0') {
            line = strtok(NULL, "\n");
            continue;
        }

        if (strstr(line, "()") != NULL) {
            addFunction(cls, line);
        } else {
            if (line[strlen(line) - 1] != '}') {
                addAttribute(cls, line);
            }
        }
        line = strtok(NULL, "\n");
    }
}

Class* findClassByName(Class* classes, int classCount, const char* name) {
    for (int i = 0; i < classCount; i++) {
        if (strcmp(classes[i].name, name) == 0) {
            return &classes[i];
        }
    }
    return NULL;
}

void addRelationshipToClass(Class* cls, const char* relatedClassName, const char* relationshipType, const char* relationshipName) {
    if (cls->relationshipsCount == cls->relationshipsCapacity) {
        cls->relationshipsCapacity *= 2;
        cls->relationships = realloc(cls->relationships, cls->relationshipsCapacity * sizeof(Relationship));
    }
    Relationship* rel = &cls->relationships[cls->relationshipsCount++];
    rel->relatedClassName = strdup(relatedClassName);
    rel->relationshipType = strdup(relationshipType);
    rel->relationshipName = strdup(relationshipName);
}

void parseRelationships(const char* input, Class* classes, int classCount) {
    char* inputCopy = strdup(input);
    if (!inputCopy) {
        perror("Failed to duplicate input");
        return;
    }

    char* line = strtok(inputCopy, "\n");
    while (line) {
        char class1[50], class2[50], relationshipType1[50], relationshipType2[50], relationshipName[100];
        
        if (sscanf(line, " %49[^ ] \"%[^\"]\" -- \"%[^\"]\" %49[^ ] : %99[^\n]",
                   class1, relationshipType1, relationshipType2, class2, relationshipName) == 5) {            
            Class* cls1 = findClassByName(classes, classCount, class1);
            Class* cls2 = findClassByName(classes, classCount, class2);
            if (cls1 && cls2) {
                addRelationshipToClass(cls1, class2, relationshipType2, relationshipName);
                addRelationshipToClass(cls2, class1, relationshipType1, relationshipName);
            } else {
                printf("One or both classes not found for relationship: %s and/or %s\n", class1, class2);
            }
        }

        line = strtok(NULL, "\n");
    }

    free(inputCopy);
}


void freeClass(Class *cls) {
    free(cls->name);
    for (int i = 0; i < cls->attributesCount; i++) {
        free(cls->attributes[i]);
    }
    free(cls->attributes);
    for (int i = 0; i < cls->functionsCount; i++) {
        free(cls->functions[i]);
    }
    free(cls->functions);
    for (int i = 0; i < cls->relationshipsCount; i++) {
        free(cls->relationships[i].relatedClassName);
        free(cls->relationships[i].relationshipType);
        free(cls->relationships[i].relationshipName);
    }
    free(cls->relationships);
}
