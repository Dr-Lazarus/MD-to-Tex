#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef enum
{
    DIAGRAM_START,
    DIAGRAM_END,
    ACTOR,
    MESSAGE,
    CONDITION_START,
    CONDITION_END,
    CLASS,
    CLASS_RELATION,
    COMMENT,
    UNKNOWN
} TokenType;

// Structure to hold a token
typedef struct
{
    TokenType type;
    char *content;
} Token;

// Prototypes
void processLine(char *line, TokenType currentDiagramType, Token *tokens, int *tokenCount);
TokenType identifyDiagramType(char *line);
void addToken(TokenType type, const char *content, Token *tokens, int *tokenCount);

int main()
{
    const char *filename = "your_diagrams.md"; // Replace with the actual file name
    FILE *file = fopen(filename, "r");
    if (!file)
    {
        perror("File opening failed");
        return EXIT_FAILURE;
    }

    char line[1024];
    TokenType currentDiagramType = UNKNOWN;
    Token tokens[1000];
    int tokenCount = 0;

    while (fgets(line, sizeof(line), file))
    {
        if (strstr(line, "```mermaid"))
        {
            currentDiagramType = DIAGRAM_START;
            addToken(DIAGRAM_START, line, tokens, &tokenCount);
        }
        else if (currentDiagramType != UNKNOWN && strstr(line, "```"))
        {
            currentDiagramType = UNKNOWN;
            addToken(DIAGRAM_END, line, tokens, &tokenCount);
        }
        else if (currentDiagramType == DIAGRAM_START)
        {
            currentDiagramType = identifyDiagramType(line);
            addToken(currentDiagramType, line, tokens, &tokenCount);
        }
        else if (currentDiagramType != UNKNOWN)
        {
            processLine(line, currentDiagramType, tokens, &tokenCount);
        }
        else
        {
            // Lines outside of mermaid blocks can be handled here if needed
        }
    }

    fclose(file);

    for (int i = 0; i < tokenCount; ++i)
    {
        printf("Token %d: Type %d, Content: %s\n", i, tokens[i].type, tokens[i].content);
    }

    for (int i = 0; i < tokenCount; ++i)
    {
        free(tokens[i].content);
    }

    return 0;
}

void processLine(char *line, TokenType currentDiagramType, Token *tokens, int *tokenCount)
{
    // Add detailed parsing logic based on diagram type
    if (currentDiagramType == DIAGRAM_START)
    { // Placeholder for actual type checks
        // Check for specific elements based on diagram type
        if (strstr(line, "->>"))
        {
            addToken(MESSAGE, line, tokens, tokenCount);
        }
        else if (strstr(line, "<|--"))
        {
            addToken(CLASS_RELATION, line, tokens, tokenCount);
        }
        else
        {
            addToken(UNKNOWN, line, tokens, tokenCount);
        }
    }
}

TokenType identifyDiagramType(char *line)
{

    if (strstr(line, "sequenceDiagram"))
    {
        return DIAGRAM_START;
    }
    else if (strstr(line, "classDiagram"))
    {
        return DIAGRAM_START;
    }
    else if (strstr(line, "graph"))
    {
        return DIAGRAM_START;
    }
    return UNKNOWN;
}

void addToken(TokenType type, const char *content, Token *tokens, int *tokenCount)
{
    tokens[*tokenCount].type = type;
    tokens[*tokenCount].content = strdup(content);
    (*tokenCount)++;
}
