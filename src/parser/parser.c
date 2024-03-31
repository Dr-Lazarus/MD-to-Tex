#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <regex.h>
#include "node.h"

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



md_node* create_md_node(NodeType type, char * data, int len, md_node *next, md_node *prev, md_node *parent, md_node *first_child, md_node *last_child, Mode user_data) {
  md_node *node = (md_node *) malloc(sizeof(md_node));
  node->type = type;
  node->next = next;
  node->prev = prev;
  node->parent = parent;
  node->first_child = first_child;
  node->last_child = last_child;
  node->data = data;
  node->len = len;
  node->user_data = user_data;
  return node;
}

md_node* create_empty_md_node(NodeType type) {
  return create_md_node(type, NULL, 0, NULL, NULL, NULL, NULL, NULL, MODE_NONE);
}

md_node* init_tree() {
  return create_md_node(NODE_ROOT, "root", 4, NULL, NULL, NULL, NULL, NULL, MODE_EMPTY);
}


void print_node(md_node* node, int indent_level) {
  for (int i = 0; i < indent_level * 2; i++) {
    printf(" ");
  }
  if (node->data != NULL) {
    printf("data: %s\n", node->data);
  } else {
    printf("data: None\n");
  }

  switch (node->type) {
    case NODE_HEADING:
      for (int i = 0; i < indent_level * 2; i++) {
        printf(" ");
      }
      printf("level: %d\n", node->heading_level);
      break;
    case NODE_CODE:
      for (int i = 0; i < indent_level * 2; i++) {
        printf(" ");
      }
      printf("lang: %s\n", node->code_language);
      break;
    default:
      break;
  }
}

void print_type(md_node* node, int indent_level) {
  for (int i = 0; i < indent_level * 2; i++) {
    printf(" ");
  }
  switch (node->type) {
    case NODE_ROOT:
      printf("root node\n");
      break;
    case NODE_PARAGRAPH:
      printf("paragraph node\n");
      break;
    case NODE_TEXT:
      printf("text node\n");
      break;
    case NODE_HEADING:
      printf("header node\n");
      break;
    case NODE_CODE:
      printf("code node\n");
      break;
    default:
      printf("unknown node\n");
      break;
  }
}

void print_tree_data(md_node* node, int indent_level) {
  if (node == NULL) {
    return;
  }
  print_type(node, indent_level);
  print_node(node, indent_level);
  for(int i = 0; i < indent_level * 2; i++) {
    printf(" ");
  }
  printf("--------\n");
  md_node *child;
  for (child = node->first_child; child != NULL; child = child->next) {
    print_tree_data(child, indent_level + 1);
  }
}

void append_to_root(md_node* root, md_node* new_child) {
  new_child->parent = root;
  if (root->last_child == NULL) {
    root->last_child = new_child;
    root->first_child = new_child;
  } else {
    root->last_child->next = new_child;
    new_child->prev = root->last_child;
    root->last_child = new_child;
  }
}

void link_children(md_node *dest, md_node *source) {
  dest->last_child->next = source->first_child;
  source->first_child->prev = dest->last_child;
  dest->last_child = source->last_child;

  for (md_node *start = source->first_child; start != NULL; start = start->next) {
    start->parent = dest;
  }
}

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

  if (line == NULL) {return 0;}
  if (line[0] != '#') {return 0;}
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
  if (line[0] != '#') {return -1;}
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
  node->data = (char *) calloc(heading_data_length, sizeof(char));
  node->len = heading_data_length;
  line += (node->heading_level + 1);
  strncpy(node->data, line, heading_data_length);
  printf("check null: %d\n", node->data[node->len]);
}

int is_codeblock_indicator(const char* line, int line_length) {
  if (line_length < 3) {return 0;}
  return strstr(line, "```") != NULL && line[0] == '`';
}

void set_code_language(md_node *current_node, const char *line, int line_length) {
  if (line_length <= 3) {
    current_node->code_language = NULL;
    return;
  }

  current_node->code_language = (char *) calloc(line_length - 2, sizeof(char));
  line += 3;
  printf("%s\n", line);
  strncpy(current_node->code_language, line, line_length - 2);
}

void set_code_data(md_node *node, const char *line, int line_length) {
  if (node->data == NULL) {
    node->data = (char *) calloc(line_length + 1, sizeof(char));
    node->len = line_length;
    strncpy(node->data, line, line_length + 1);
  } else {
    node->data = (char *) realloc(node->data, line_length + node->len + 2 * sizeof(char));

    node->data[node->len] = '\n';
    node->data[node->len + 1] = '\0';
    strncat(node->data, line, line_length + 1);
    // 1 more character than usual since the '\n'
    node->len += (line_length + 1);
  }
  printf("code issue:\n");
  printf("check null: %d\n", node->data[node->len]);
}

void parse_paragraph_line(md_node *current_node, const char *line, int line_length) {
  md_node *text_node = create_empty_md_node(NODE_TEXT); 
  text_node->data = (char *) calloc(line_length + 1, sizeof(char));
  text_node->len = line_length;
  strncpy(text_node->data, line, line_length + 1);
  append_to_root(current_node, text_node);

  printf("check null: %d\n", text_node->data[text_node->len]);
}
// the idea of this function is based on the current node, we will figure out where to put the line with respect to the rest of the tree
void parse_line(md_node* root, const char *line, int line_length) {
  // first we check what content we have.
  if (line == NULL) {
    printf("Line should have something\n");
    abort();
  }

  md_node *new_child_node;
  // if its an empty line
  if (line_length == 0) {
    // we stop appending to the previous paragraph
    root->user_data = MODE_STARTNEW;
  } else if (is_header(line, line_length)) {
    // we are dealing with a header
    // we go back to root
    // we reset the mode to startnew
    root->user_data = MODE_STARTNEW;
    new_child_node = create_empty_md_node(NODE_HEADING);

    set_header_data(new_child_node, line, line_length);
    append_to_root(root, new_child_node);

  } else if (is_codeblock_indicator(line, line_length)) {

    switch (root->user_data) {
      case MODE_CODE:
        root->user_data = MODE_STARTNEW;
        break;
      case MODE_EMPTY:
      case MODE_STARTNEW:
        root->user_data = MODE_CODE;
        new_child_node = create_empty_md_node(NODE_CODE);
        set_code_language(new_child_node, line, line_length);
        append_to_root(root, new_child_node);
        break;
      case MODE_APPENDPARA:
        printf("why here\n");
        abort();
      default:
        printf("Unidentified mode to start code block %d\n", root->user_data);
        abort();
    }

  } else {
    if (root->user_data == MODE_CODE) {
      // means we want to add to the previous one
      set_code_data(root->last_child, line, line_length);
    } else {
  
      // currently assume its just text
      new_child_node = create_empty_md_node(NODE_PARAGRAPH);

      parse_paragraph_line(new_child_node, line, line_length);

      switch (root->user_data) {
        case MODE_STARTNEW:
        case MODE_EMPTY:
          append_to_root(root, new_child_node);
          break;
        case MODE_APPENDPARA:
          // we take the last_child of the last_child
          if (root->last_child->type != NODE_PARAGRAPH) {
            printf("last child is not paragraph, cannot append");
            abort();
          }
          link_children(root->last_child, new_child_node);
          break;
        default:
          break;
      }
      root->user_data = MODE_APPENDPARA;
    }
  }
}

void free_tree(md_node *root) {
  if (root == NULL) { return; }

  // if there are children
  if (root->first_child != NULL) {
    for (md_node *child = root->first_child; child != NULL; child = child->next) {
      free_tree(child);
    }

    md_node *tmp;
    md_node *head = root->first_child;
    while (head != NULL) {
      tmp = head;
      head = head->next;

      if (tmp->data != NULL) {
        free(tmp->data);
      }
      free(tmp);
    }
  }

}
// possible reference for line reading
// https://stackoverflow.com/questions/29576799/reading-an-unknown-length-line-from-stdin-in-c-with-fgets

int main(int argc, char ** argv) {
  if (argc != 2) {
    printf("No file specified");
    return 1;
  }
  
  char *file_contents = read_source_code(argv[1]);


  md_node *root = init_tree();

  int line_number = 0;
  int char_so_far = 0;
  int line_length = 0;
  int line_size = MAX_LINE_LENGTH;
  char *line = (char *) calloc(line_size, sizeof(char));
  char *ptr = file_contents;
  char *start = file_contents;

  while ((ptr = strstr(ptr, "\n")) != NULL) {
    line_length = ptr - start;
    if (line_size < line_length) {
      while (line_size < line_length) {
        line_size *= 2;
      }
      line = (char *) realloc(line, line_size * sizeof(char));
    }

    strncpy(line, start, line_length);
    line[line_length] = '\0';
    

    // Now we want to figure out how does this line fit in the Tree
    printf("parsing line: %d\n", line_number);
    printf("line: %s\n", line);
    parse_line(root, line, line_length);

    // terminating stuff
    line_number++;
    char_so_far += (line_length + 1);
    line_length = 0;
    ptr++;
    start = ptr;
  }
  // last line needs to be considered also
  line_length = strlen(file_contents) - char_so_far;
  if (line_size < line_length) {
    while (line_size < line_length) {
      line_size *= 2;
    }
    line = (char *) realloc(line, line_size * sizeof(char));
  }

  strncpy(line, start, line_length);
  line[line_length] = '\0';
  line_number++;
  line_length = 0;
  start = ptr;
  ptr++;

  print_tree_data(root, 0);
  free_tree(root);
  free(root);
  free(line);
  return 0;
}
