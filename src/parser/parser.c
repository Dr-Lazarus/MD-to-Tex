#include <string.h>
#include <stdio.h>
#include <stdlib.h>

// defined by the recommended line length for markdown
#define MAX_LINE_LENGTH 121

// first we need some code to read the file
char* read_source_code(const char *filename) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        fprintf(stderr, "Error opening file: %s\n", filename);
        exit(EXIT_FAILURE);
    }
    fseek(file, 0, SEEK_END);  
    long file_size = ftell(file);
    rewind(file);
    char *source_code = (char *)malloc((file_size + 1) * sizeof(char));
    if (source_code == NULL) {
        fprintf(stderr, "Error allocating memory for source code\n");
        exit(EXIT_FAILURE);
    }
    size_t read_size = fread(source_code, sizeof(char), file_size, file);
    source_code[read_size] = '\0';
    fclose(file);
    return source_code;
}

// possible reference for line reading
// https://stackoverflow.com/questions/29576799/reading-an-unknown-length-line-from-stdin-in-c-with-fgets

int main(int argc, char ** argv) {
  if (argc != 2) {
    printf("No file specified");
    return 1;
  }
  
  // first we read the file
  FILE *file = fopen(argv[1], "r");
  if (file == NULL) {
    printf("Error reading file %s", argv[1]);
    return 1;
  }

  // we make an easy assumption that the MAX_LINE_LENGTH is 1024
  char buffer[MAX_LINE_LENGTH];
  int buffer_length;

  char *new_line_char;
  char *line = (char *) calloc(MAX_LINE_LENGTH, sizeof(char));
  char *ptr = line;
  int line_size = MAX_LINE_LENGTH;
  int line_length = 0;
  int line_number = 0;
  int is_empty_line = 0;

  while (fgets(buffer, MAX_LINE_LENGTH, file) != NULL) {
    // the idea is we have a buffer
    printf("Buffer: %s\n", buffer);

    // we want to copy whatever in the buffer over to the line
    buffer_length = strlen(buffer);

    line_length += buffer_length;
    if (line_length > line_size) {

      // we need to double the size;
      line_size *= 2;
      line = (char *) realloc(line, line_size * sizeof(char));
    }
    
    // then we copy safely
    strncpy(ptr, buffer, buffer_length);

    // we move the pointer along so we can copy correctly next
    ptr += buffer_length;
    // we want to check if there is a new line in the 
    new_line_char = strstr(buffer, "\n");

    // if there is no new line, we keep reading
    if (new_line_char == NULL) {
      // we continue reading
      continue;
    } 
    
    // shift the pointer back slightly for the '\n'
    ptr--;
    // replace the pointer with '\0' so we don't deal with '\n'
    *ptr = '\0';
    is_empty_line = (buffer[0] == '\n') ? 1 : 0;
    printf("Line number: %d\n", line_number);
    printf("Copied Line: %s\n", line);
    printf("is_empty_line %d\n", is_empty_line);
    // Now we need to parse and figure out what the node is

    // now we want to empty line
    memset(line, '\0', line_size);
    // reset the pointer to the start
    ptr = line;
    // and reset the line_length
    line_length = 0;
    // we increment line number as well
    line_number++;
  }

  free(line);
  fclose(file);
  return 0;
}
