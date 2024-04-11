#include "list.h"
#include <regex.h>
#include <stdio.h>

int is_list_item(const char *line, int line_length) {
  regex_t list_regex;
  int value;
  // Creation of regEx
  // we are opinionated and need the space
  value = regcomp(&list_regex, "^(\t| )*(\\*|-|\\+)(\t| )(.*)$", REG_EXTENDED);
  if (value != 0) {
    printf("regex didn't compile\n");
  }
  value = regexec(&list_regex, line, 0, NULL, 0);
  if (value == 0) {
    return 1;
  }

  // check numbered list
  value = regcomp(&list_regex, "^(\t| )*([[:digit:]]+\\.)(\t| )(.*)$",
                  REG_EXTENDED);
  if (value != 0) {
    printf("regex didn't compile\n");
  }
  value = regexec(&list_regex, line, 0, NULL, 0);

  return (value == 0);
}
