#include "mermaid_class_diagram.h"
#include <ctype.h>
#include <stdio.h>
#include <string.h>

void parse_class_diagram_mermaid_code(const char* mermaidCode, ClassNode* classNodes, int* classNodeCount, ClassRelationship* relationships, int* relationshipCount) {}

void generate_latex_class_diagram(const ClassNode* classNodes, int classNodeCount, const ClassRelationship* relationships, int relationshipCount, FILE* output) {
  fprintf(output, "\\begin{tikzpicture}[>=stealth]\n");

  for (int i = 0; i < classNodeCount; i++) {
      fprintf(output, "");
  }

  for (int i = 0; i < relationshipCount; i++) {
      fprintf(output, "\\draw (%s) -- (%s) node[midway, fill=white] {%s};\n",
              relationships[i].fromId, relationships[i].toId, relationships[i].label);
  }

  fprintf(output, "\\end{tikzpicture}\n");
}
