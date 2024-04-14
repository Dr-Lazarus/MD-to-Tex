#include "mermaid_class_diagram.h"
#include <ctype.h>
#include <stdio.h>
#include <string.h>
#define MAX_ATTR_METHOD_LEN 256
int is_class_definition(const char *line) {
  // This function will return 1 if the line starts with "class".
  // Trim whitespace before checking to handle cases where the line might start with spaces.
  while (isspace((unsigned char)*line))
    line++;
  return strncmp(line, "class", 5) == 0 && isspace((unsigned char)line[5]);
}

int is_relationship_definition(const char *line) {
  // This function will return 1 if the line contains relationship indicators like "<|--".
  // It checks for various UML relationship symbols.
  const char *relationshipIndicators[] = {
      "<|--",  // Inheritance
      "*--",   // Composition
      "o--",   // Aggregation
      "-->",   // Directed association
      "--",    // Association or link
      "..>",   // Dependency
      "..|>",  // Realization or interface implementation
      NULL};

  for (const char **indicator = relationshipIndicators; *indicator != NULL;
       ++indicator) {
    if (strstr(line, *indicator)) {
      return 1;
    }
  }
  return 0;
}

void trim_space(char *str) {
  char *end, *start = str + strspn(str, " \t\n\r");  // Trim leading whitespace
  if (*start == 0) {                                 // All spaces?
    *str = 0;
    return;
  }
  end = start + strlen(start) - 1;
  while (end > start && isspace((unsigned char)*end))
    end--;  // Trim trailing whitespace
  *(end + 1) = 0;
  memmove(str, start, end + 2 - start);  // Shift back to start
}

void parse_class_line(const char **lines, int numLines, int *currentLine,
                      ClassNode *classNode) {
  char buffer[1024] = {0};
  const char *ptr = strstr(lines[*currentLine], "class");
  if (!ptr)
    return;

  // Skip past 'class' and whitespace to extract class name
  ptr += 5;  // Skip past 'class'
  while (isspace(*ptr))
    ptr++;  // Skip spaces

  // Extract class name
  const char *nameEnd = strchr(ptr, '{');
  if (!nameEnd)
    return;

  int nameLength = nameEnd - ptr;
  strncpy(classNode->id, ptr,
          nameLength < sizeof(classNode->id) ? nameLength
                                             : sizeof(classNode->id) - 1);
  classNode
      ->id[nameLength < sizeof(classNode->id) ? nameLength
                                              : sizeof(classNode->id) - 1] =
      '\0';
  trim_space(classNode->id);  // Trim spaces from the class name

  // Initialize counts
  classNode->attribute_count = 0;
  classNode->method_count = 0;
  classNode->annotation_count = 0;

  // Move to the line containing the opening '{'
  ptr = strchr(lines[*currentLine], '{');
  if (!ptr)
    return;
  ptr++;  // Move past '{'

  // Handle the internal structure potentially across multiple lines
  while (*currentLine < numLines) {
    if (strstr(lines[*currentLine], "}"))
      break;  // End of class definition
    strcpy(buffer, ptr);
    ptr = buffer;

    while (*ptr && *ptr != '\n' && *ptr != '}') {
      if (*ptr == '\n' || *ptr == '\r') {
        ptr++;
        continue;  // Skip new lines within class definition
      }
      char *line_end = strchr(ptr, '\n');
      if (!line_end)
        line_end = strchr(ptr, '}');

      if (!line_end)
        break;

      int length = (int)(line_end - ptr);
      strncpy(buffer, ptr, length);
      buffer[length] = '\0';

      // Process the content
      if (strstr(buffer, "<<")) {  // Annotation
        sscanf(buffer, " << %[^>>] >>", buffer);
        trim_space(buffer);
        strncpy(classNode->annotations[classNode->annotation_count++], buffer,
                MAX_ATTR_METHOD_LEN - 1);
      } else if (strchr(buffer, '(')) {  // Method
        trim_space(buffer);
        strncpy(classNode->methods[classNode->method_count++], buffer,
                MAX_ATTR_METHOD_LEN - 1);
      } else if (strchr(buffer, ':')) {  // Attribute
        trim_space(buffer);
        strncpy(classNode->attributes[classNode->attribute_count++], buffer,
                MAX_ATTR_METHOD_LEN - 1);
      }

      ptr = line_end + 1;
    }

    (*currentLine)++;
    if (*currentLine < numLines) {
      ptr = lines[*currentLine];  // Move to the next line
    }
  }
}

void parse_relationship_line(const char *line,
                             ClassRelationship *relationship) {
  char buffer[1024];
  // Assuming the format: Source "cardinality" --|> "cardinality" Destination : Label
  char *ptr = strstr(line, "--");
  if (!ptr)
    return;

  // Extract source class and cardinality
  sscanf(line, "%s \"%[^\"]\"", relationship->fromId,
         relationship->cardinality_from);
  line = ptr;

  // Extract relationship type and target class with cardinality
  sscanf(line, "%s \"%[^\"]\" %s : %[^\n]", buffer,
         relationship->cardinality_to, relationship->toId, relationship->label);

  // Determine relationship type from buffer
  if (strstr(buffer, "<|--"))
    strcpy(relationship->type, "inheritance");
  else if (strstr(buffer, "*--"))
    strcpy(relationship->type, "composition");
  else if (strstr(buffer, "o--"))
    strcpy(relationship->type, "aggregation");
  else if (strstr(buffer, "-->"))
    strcpy(relationship->type, "association");
  else if (strstr(buffer, "--"))
    strcpy(relationship->type, "association");
}

void calculate_layout(ClassNode *classNodes, int classNodeCount,
                      ClassRelationship *relationships, int relationshipCount) {
  int *levels = malloc(classNodeCount * sizeof(int));
  int *xPos = malloc(classNodeCount * sizeof(int));

  // Initialize node levels and positions
  for (int i = 0; i < classNodeCount; i++) {
    levels[i] = -1;  // Unassigned level
    xPos[i] = 0;     // Initial horizontal position
  }

  // Assign levels using a simplistic topological sorting approach
  int level = 0;
  for (int i = 0; i < classNodeCount; i++) {
    if (levels[i] == -1) {  // If level is not assigned
      assign_levels(classNodes, classNodeCount, relationships,
                    relationshipCount, levels, i, 0);
    }
  }

  // Position nodes horizontally to minimize crossings
  int currentX = 0;
  for (int l = 0; levels[l] >= 0 && l < classNodeCount; l++) {
    int countAtLevel = 0;
    for (int i = 0; i < classNodeCount; i++) {
      if (levels[i] == l) {
        xPos[i] = currentX++;
        countAtLevel++;
      }
    }
    currentX +=
        countAtLevel /
        2;  // Increase currentX for a new block of nodes at the same level
  }

  // Assign positions to nodes based on their level and horizontal position
  for (int i = 0; i < classNodeCount; i++) {
    classNodes[i].position.x = xPos[i] * 100;   // scale x position
    classNodes[i].position.y = levels[i] * 75;  // scale y position
  }

  free(levels);
  free(xPos);
}

// Helper function for level assignment
void assign_levels(ClassNode *classNodes, int classNodeCount,
                   ClassRelationship *relationships, int relationshipCount,
                   int *levels, int currentNode, int currentLevel) {
  levels[currentNode] = currentLevel;
  for (int i = 0; i < relationshipCount; i++) {
    if (strcmp(classNodes[currentNode].id, relationships[i].fromId) == 0) {
      int toIndex = -1;
      for (int j = 0; j < classNodeCount; j++) {
        if (strcmp(classNodes[j].id, relationships[i].toId) == 0) {
          toIndex = j;
          break;
        }
      }
      if (toIndex != -1 && levels[toIndex] < currentLevel + 1) {
        assign_levels(classNodes, classNodeCount, relationships,
                      relationshipCount, levels, toIndex, currentLevel + 1);
      }
    }
  }
}

int is_relevant_line(const char *line) {
  // Function to check if the line is relevant to Mermaid syntax
  const char *relevant_keywords[] = {
      "classDiagram",
      "class",
      "<<",
      "+",
      "-",
      "%%",
      "<|--",
      "*--",
      "o--",
      "-->",
      NULL  // Sentinel to mark end of array
  };

  for (int i = 0; relevant_keywords[i] != NULL; i++) {
    if (strstr(line, relevant_keywords[i]) != NULL) {
      return 1;
    }
  }
  return 0;
}

int get_next_line(const char **mermaidCode, char *line, int *line_number) {
  if (mermaidCode == NULL || *mermaidCode == NULL || **mermaidCode == '\0') {
    return 0;  // No more lines to read
  }

  const char *start = *mermaidCode;
  const char *end;
  int line_length;

  while (*start != '\0') {
    // Find the end of the current line or the end of the string
    for (end = start; *end != '\n' && *end != '\0'; end++)
      ;

    line_length = end - start;
    if (line_length > MAX_LINE_LENGTH - 1) {
      printf("Error on line %d: Line length exceeds MAX_LINE_LENGTH.\n",
             *line_number);
      exit(EXIT_FAILURE);
    }

    // Copy the line to the buffer
    strncpy(line, start, line_length);
    line[line_length] = '\0';  // Null-terminate the line

    // Increment line counter after copying line
    (*line_number)++;

    // Check for and skip blank lines and comments
    if (line[0] != '\0' && line[0] != '%' && is_relevant_line(line)) {
      *mermaidCode = (*end != '\0')
                         ? end + 1
                         : end;  // Move pointer to the start of the next line
      return 1;
    }

    // Error if line is not relevant
    if (line[0] != '\0' && line[0] != '%' && !is_relevant_line(line)) {
      printf(
          "Error on line %d: Line is not relevant to Mermaid class diagram "
          "syntax.\n",
          *line_number);
      exit(EXIT_FAILURE);
    }

    if (*end == '\0') {
      break;  // Reached the end of the string
    }
    start = end + 1;  // Move to the start of the next line
  }

  // If we reach the end of the string without finding relevant lines, throw an error.
  printf("Error: No relevant Mermaid syntax found in the input.\n");
  exit(EXIT_FAILURE);
}
/*
 * Parses the provided Mermaid class diagram code into a structured format
 * that includes class nodes and their relationships. This structured format
 * is then used to generate a corresponding LaTeX representation.
 * Parameters:
 * - mermaidCode: A string containing the Mermaid code for a class diagram.
 * - classNodes: An array to be populated with parsed class nodes.
 * - classNodeCount: A pointer to an integer to hold the count of class nodes parsed.
 * - relationships: An array to be populated with the relationships between class nodes.
 * - relationshipCount: A pointer to an integer to hold the count of relationships parsed.
 */
void parse_class_diagram_mermaid_code(const char *mermaidCode,
                                      ClassNode *classNodes,
                                      int *classNodeCount,
                                      ClassRelationship *relationships,
                                      int *relationshipCount) {
  const char *currentPtr = mermaidCode;
  char line[MAX_LINE_LENGTH];
  int currentLine = 0;

  while (get_next_line(&currentPtr, line, &currentLine)) {
    if (is_class_definition(line)) {
      parse_class_line(&currentPtr, &currentLine,
                       classNodes + (*classNodeCount));
      (*classNodeCount)++;
    } else if (is_relationship_definition(line)) {
      parse_relationship_line(line, relationships + (*relationshipCount));
      (*relationshipCount)++;
    }
  }

  calculate_layout(classNodes, *classNodeCount, relationships,
                   *relationshipCount);
}

/*
 * Generates a LaTeX representation of a class diagram, using TikZ for drawing.
 * The diagram is composed of class nodes and their relationships, defined by
 * the structured data produced by parsing Mermaid code.
 * Parameters:
 * - classNodes: An array of class nodes that make up the class diagram.
 * - classNodeCount: The number of class nodes in the classNodes array.
 * - relationships: An array of relationships between the class nodes.
 * - relationshipCount: The number of relationships in the relationships array.
 * - output: A file stream to write the generated LaTeX code to.
 */
void generate_latex_class_diagram(const ClassNode *classNodes,
                                  int classNodeCount,
                                  const ClassRelationship *relationships,
                                  int relationshipCount, FILE *output) {
  int i;

  fprintf(output, "\\begin{tikzpicture}[>=stealth]\n");

  for (i = 0; i < relationshipCount; i++) {
    fprintf(output, "\\draw (%s) -- (%s) node[midway, fill=white] {%s};\n",
            relationships[i].fromId, relationships[i].toId,
            relationships[i].label);
  }

  fprintf(output, "\\end{tikzpicture}\n");
}
