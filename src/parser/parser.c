#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#define MAX_LINE_LENGTH 1024;
FILE *file = fopen("textfile.txt", "r");
char line[MAX_LINE_LENGTH];
while (fgets(line, MAX_LINE_LENGTH, file) != NULL)
{
printf("Line: %s\n", line);
}
fclose(file);
return 0;

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

int main(int argc, char ** argv) {
  if (argc != 2) {
    printf("No file specified");
    return 1;
  }
  
  char *source_code = read_source_code(argv[1]);

  if (source_code == NULL) {
      fprintf(stderr, "Error reading source code from '%s'\n", argv[1]);
      return 1;
  }
  char *lines = source_code;
  char delim = '\n';
  int line_number = 0;

  do {
    printf("line: %d\n", line_number);
    printf("%s\n", lines);
    lines++;
    lines = strstr()
    line_number++;
  }
  while ((lines =)) {

  }

  return 0;
}
