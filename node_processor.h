#include <stdint.h>
#include <stdio.h>
#include <stddef.h>

typedef struct cmark_mem
{
    void *(*calloc)(size_t, size_t);
    void *(*realloc)(void *, size_t);
    void (*free)(void *);
} cmark_mem;

struct cmark_node
{
    cmark_mem *mem;

    struct cmark_node *next;
    struct cmark_node *prev;
    struct cmark_node *parent;
    struct cmark_node *first_child;
    struct cmark_node *last_child;

    void *user_data;

    unsigned char *data;
    size_t len; /* Should've been bufsize_t */

    int start_line;
    int start_column;
    int end_line;
    int end_column;
    uint16_t type;
    uint16_t flags;
};

char *process_node(struct cmark_node *root_node);