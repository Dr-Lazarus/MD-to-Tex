#include "mermaid_pie.h"
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

// Helper function to trim leading and trailing whitespace
char *trim_whitespace(char *str)
{
    char *end;

    // Trim leading space
    while (isspace((unsigned char)*str))
        str++;

    if (*str == 0) // All spaces?
        return str;

    // Trim trailing space
    end = str + strlen(str) - 1;
    while (end > str && isspace((unsigned char)*end))
        end--;

    // Write new null terminator character
    end[1] = '\0';

    return str;
}

void parse_pie_chart_mermaid_code(const char *mermaidCode, char *title, PieSection *sections, int *sectionCount)
{
    const char *cursor = strstr(mermaidCode, "title") + 5;
    // Assuming the title ends with a newline
    sscanf(cursor, "%[^\n]\n", title);
    cursor = strchr(cursor, '\n') + 1; // Move to the first section

    while (*cursor && *sectionCount < MAX_PIE_SECTIONS)
    {
        cursor = strchr(cursor, '"') + 1; // Move to the start of the label
        char *endQuote = strchr(cursor, '"');
        if (!endQuote)
            break; // Exit if no closing quote

        size_t labelLen = endQuote - cursor;
        if (labelLen >= MAX_LABEL_LENGTH)
            labelLen = MAX_LABEL_LENGTH - 1;
        strncpy(sections[*sectionCount].label, cursor, labelLen);
        sections[*sectionCount].label[labelLen] = '\0';

        sscanf(endQuote + 3, "%d", &sections[*sectionCount].value); // Skip " :
        (*sectionCount)++;

        cursor = strchr(endQuote, '\n'); // Move to the next line
        if (cursor)
            cursor++; // Skip the newline character
        else
            break;
    }
}

void generate_latex_pie_chart(const char *title, const PieSection *sections, int sectionCount, FILE *output)
{
    // Assuming the document preamble and necessary packages are included elsewhere
    // as the function is designed to generate only the pie chart part.

    // Output the title in bold, centered
    fprintf(output, "\\begin{center}\n");
    fprintf(output, "\\textbf{%s}\n", title);
    fprintf(output, "\\end{center}\n\n");

    // Drawing the pie chart
    fprintf(output, "\\begin{tikzpicture}\n");
    fprintf(output, "    \\pie[text=legend, radius=3]{\n");

    for (int i = 0; i < sectionCount; i++)
    {
        fprintf(output, "        %d/%s", sections[i].value, sections[i].label);
        if (i < sectionCount - 1)
        {
            fprintf(output, ",\n");
        }
    }

    fprintf(output, "}\n");
    fprintf(output, "\\end{tikzpicture}\n");
}
