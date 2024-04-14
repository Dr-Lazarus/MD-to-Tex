#include <regex.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int is_blockquote(const char *line, int line_length) {
  regex_t regex;
  int value;
  /* Creation of regEx */
  value = regcomp(&regex, "^>+.*", REG_EXTENDED);
  if (value != 0) {
    printf("regex didn't compile\n");
  }
  value = regexec(&regex, line, 0, NULL, 0);

  return (value == 0);
}

char *strip_blockquote(const char *text, int text_length) {
  char *result;
  int i, length;
  if (text[0] != '>') {
    result = (char *)calloc(text_length + 1, sizeof(char));
    strncpy(result, text, text_length);
    result[text_length] = '\0';
    return result;
  }

  for (i = 1; i < text_length; i++) {
    if (text[i] != ' ') {
      break;
    }
  }

  length = text_length - i + 1;
  result = (char *)calloc(length + 1, sizeof(char));
  strncpy(result, &(text[i]), length);
  result[length] = '\0';
  return result;
}
