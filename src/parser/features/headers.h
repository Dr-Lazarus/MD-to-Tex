int is_header(const char *line, int line_length) {
  // // check if its a header,
  // regex_t header_regex;
  // int value;
  // // Creation of regEx
  // // we are opinionated and need the space
  // value = regcomp( &header_regex, "^(#{1,6} .+)", 0);
  // if (value != 0) {
  //   printf("regex didn't compile");
  // }
  // value = regexec( &header_regex, line,
  //                   0, NULL, 0);
  // return value == 0;

  if (line == NULL) {
    return 0;
  }
  if (line[0] != '#') {
    return 0;
  }
  int have_hex = 1;
  int counter = 0;
  for (int i = 0; i < line_length; i++) {
    if (have_hex && line[i] == '#') {
      counter++;
      continue;
    } else if (have_hex && line[i] == ' ') {
      break;
    } else {
      have_hex = 0;
    }
  }
  return counter <= 6 ? have_hex : 0;
}

int get_header_level(const char *line, int line_length) {
  int c = 0;
  if (line[0] != '#') {
    return -1;
  }
  for (int i = 0; i < line_length; i++) {
    if (line[i] == '#') {
      c++;
    } else {
      return c;
    }
  }
  return -1;
}

void set_header_data(md_node *node, const char *line, int line_length) {

  node->heading_level = get_header_level(line, line_length);

  int heading_data_length = line_length - node->heading_level;
  node->data = (char *)calloc(heading_data_length, sizeof(char));
  node->len = heading_data_length;
  line += (node->heading_level + 1);
  strncpy(node->data, line, heading_data_length);
  printf("check null: %d\n", node->data[node->len]);
}
