#include "mermaid_seq_diagram.h"

#include <ctype.h>
#include <stdlib.h>
#include <string.h>

void parse_seq_diagram_mermaid_code(const char *mermaid_code, Cvector *threads,
                                    Cvector *messages) {
  const char *code_cursor = mermaid_code;
  int is_first_line = 1;

  while (*code_cursor != '\0') {
    /* Get rid of whitespaces in the line. */
    while (isspace(*code_cursor)) {
      code_cursor++;
    }

    if (is_first_line &&
        seq_diagram_mermaid_code_is_diagram_type(code_cursor) == 0) {
      printf(
          "Error in parse_seq_diagram_mermaid_code: First line is not "
          "sequenceDiagram\n");
      exit(1);
    }

    /* Iterate until the end of line */
    if (is_first_line) {
      is_first_line = 0;
      code_cursor += 15;
      while (*code_cursor != '\n' && *code_cursor != '\0') {
        if (!isspace(*code_cursor)) {
          printf(
              "Error in parse_seq_diagram_mermaid_code: Unexpected text after "
              "sequenceDiagram");
          exit(1);
        }
        code_cursor++;
      }

      if (*code_cursor == '\0') {
        return;
      }

      /* Current cursor is at the \n. Increment one more time. */
      code_cursor++;
      continue;
    }

    if (seq_diagram_mermaid_code_is_participant(code_cursor)) {
      code_cursor += 11;
      while (isspace(*code_cursor)) {
        if (*code_cursor == '\n') {
          printf(
              "Error in parse_seq_diagram_mermaid_code: Unexpected line break "
              "after "
              "participant");
          exit(1);
        }
        code_cursor++;
      }

      if (*code_cursor == '\0') {
        printf(
            "Error in parse_seq_diagram_mermaid_code: Unexpected text end "
            "after "
            "participant");
        exit(1);
      }

      int name_len =
          seq_diagram_mermaid_code_get_token_length(code_cursor, "\n");
      /* Add the participant if it doesn't exist yet. */
      seq_diagram_mermaid_code_get_thread_id(threads, code_cursor, name_len);

      code_cursor = seq_diagram_mermaid_code_move_cursor_to_eol(code_cursor);
      continue;
    }

    int first_name_len =
        seq_diagram_mermaid_code_get_token_length(code_cursor, "->>");
    int first_thread_id = seq_diagram_mermaid_code_get_thread_id(
        threads, code_cursor, first_name_len);
    code_cursor = strstr(code_cursor, "->>") + 3;

    while (isspace(*code_cursor)) {
      code_cursor++;
    }

    int second_name_len =
        seq_diagram_mermaid_code_get_token_length(code_cursor, ":");
    int second_thread_id = seq_diagram_mermaid_code_get_thread_id(
        threads, code_cursor, second_name_len);
    code_cursor = strstr(code_cursor, ":") + 1;

    while (isspace(*code_cursor)) {
      code_cursor++;
    }

    int text_len = seq_diagram_mermaid_code_get_token_length(code_cursor, "\n");

    SeqMessage new_seq_msg = {first_thread_id, second_thread_id, code_cursor,
                              text_len};
    cvec_push_back(messages, &new_seq_msg);

    code_cursor = seq_diagram_mermaid_code_move_cursor_to_eol(code_cursor);
  }

  return;
}

void generate_seq_class_diagram(Cvector *threads, Cvector *messages,
                                FILE *output) {
  fprintf(output, "\\begin{sequencediagram}\n");

  for (int i = 0; i < threads->size; i++) {
    SeqThread *seq_thread = (SeqThread *)cvec_at(threads, i);
    if (i == 0) {
      fprintf(output, "  \\newthread{%d}{%.*s}\n", seq_thread->id,
              seq_thread->name_len, seq_thread->name);
      continue;
    }
    fprintf(output, "  \\newinst[2]{%d}{%.*s}\n", seq_thread->id,
            seq_thread->name_len, seq_thread->name);
  }

  for (int i = 0; i < messages->size; i++) {
    SeqMessage *seq_msg = (SeqMessage *)cvec_at(messages, i);

    if (seq_msg->from_id == seq_msg->to_id) {
      fprintf(output, "  \\begin{call}{%d}{%.*s}{%d}{}\n", seq_msg->from_id,
              seq_msg->message_len, seq_msg->message, seq_msg->to_id);
      fprintf(output, "  \\end{call}\n");
      continue;
    }

    fprintf(output, "  \\begin{messcall}{%d}{%.*s}{%d}\n", seq_msg->from_id,
            seq_msg->message_len, seq_msg->message, seq_msg->to_id);
    fprintf(output, "  \\end{messcall}\n");
  }

  fprintf(output, "\\end{sequencediagram}\n");
}

int seq_diagram_mermaid_code_is_diagram_type(const char *mermaid_code) {
  if (strncmp(mermaid_code, "sequenceDiagram", 15) == 0 &&
      (isspace(mermaid_code[15]) || mermaid_code[15] == '\0')) {
    return 1;
  }
  return 0;
}

int seq_diagram_mermaid_code_is_participant(const char *mermaid_code) {
  if (strncmp(mermaid_code, "participant", 11) == 0 &&
      isspace(mermaid_code[11])) {
    return 1;
  }
  return 0;
}

int seq_diagram_mermaid_code_get_token_length(const char *start,
                                              const char *terminating_str) {
  const char *end = strstr(start, terminating_str);
  const char *line_break = strstr(start, "\n");

  /* Weird case because the last line of mermaid is not ended by \n. */
  /* So we feed it the end of string instead. */
  if (line_break == NULL) {
    line_break = start + strlen(start);
  }
  if (end == NULL && strncmp(terminating_str, "\n", 1) == 0) {
    end = start + strlen(start);
  }

  if (end == NULL) {
    printf("Error in seq_diagram_mermaid_code_get_token_length: Can't find %s",
           terminating_str);
    exit(1);
  }
  if (line_break < end) {
    printf(
        "Error in seq_diagram_mermaid_code_get_token_length: Can't find "
        "%s before linebreak",
        terminating_str);
    exit(1);
  }

  while (isspace(*(end - 1))) {
    end--;
  }
  return end - start;
}

int seq_diagram_mermaid_code_get_thread_id(Cvector *threads, const char *name,
                                           int name_len) {
  for (int i = 0; i < threads->size; i++) {
    SeqThread *seq_thread = (SeqThread *)cvec_at(threads, i);
    if (strncmp(seq_thread->name, name, name_len) == 0) {
      return seq_thread->id;
    }
  }

  SeqThread new_seq_thread = {name, name_len, threads->size};
  cvec_push_back(threads, &new_seq_thread);
  return threads->size - 1;
}

const char *seq_diagram_mermaid_code_move_cursor_to_eol(const char *cursor) {
  while (*cursor != '\n' && *cursor != '\0') {
    cursor++;
  }
  if (*cursor == '\n') {
    return cursor + 1;
  }
  return cursor;
}
