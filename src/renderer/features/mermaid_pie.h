#ifndef MERMAID_PIE_H
#define MERMAID_PIE_H

#include <stdio.h>

#define MAX_PIE_SECTIONS 10
#define MAX_LABEL_LENGTH 100
#define MAX_TITLE_LENGTH 100

typedef struct
{
    char label[MAX_LABEL_LENGTH];
    int value;
} PieSection;

void parse_pie_chart_mermaid_code(const char *mermaidCode, char *title, PieSection *sections, int *sectionCount);
void generate_latex_pie_chart(const char *title, const PieSection *sections, int sectionCount, FILE *output);

#endif
