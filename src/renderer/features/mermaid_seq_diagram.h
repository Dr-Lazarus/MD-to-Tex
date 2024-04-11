#ifndef CVEC_H
#define CVEC_H
#include "../../util/cvector.h"
#endif
#include <stdio.h>

typedef struct {
 const char *name;
 int name_len;
 int id;
} SeqThread;

typedef struct {
 int from_id;
 int to_id;
 const char *message;
 int message_len;
} SeqMessage;

void parse_seq_diagram_mermaid_code(const char *mermaid_code, Cvector *threads,
                                    Cvector *messages);
void generate_seq_class_diagram(Cvector *threads, Cvector *messages,
                                FILE *output);
int seq_diagram_mermaid_code_is_diagram_type(const char *mermaid_code);
int seq_diagram_mermaid_code_is_participant(const char *mermaid_code);
int seq_diagram_mermaid_code_get_token_length(const char *start,
                                              const char *terminating_str);

// Returns the a SeqThread's id if a similar name exist.
// Else, create a new SeqThread and return that id.
int seq_diagram_mermaid_code_get_thread_id(Cvector *threads, const char *name,
                                           int name_len);
const char *seq_diagram_mermaid_code_move_cursor_to_eol(const char *cursor);