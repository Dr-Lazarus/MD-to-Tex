#include "mathblock.h"
#include <string.h>

int is_mathblock_indicator(const char *line, int line_length) {
  return strncmp(line, "$$", 2) == 0 && line_length == 2;
}
