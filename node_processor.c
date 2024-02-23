#include "node_processor.h"

/* Add other enums as necessary */
enum
{
    NODETYPE_LITERAL,
    NODETYPE_BLOCKQUOTE,
    NODETYPE_CODE
} NodeType;

char *process_node(struct cmark_node *root_node)
{
    struct cmark_node *curr_child;
    char **some_container = (char **)malloc(10 * sizeof(char *));
    int index;
    char *output;

    /* If it's a literal, no processing is required.*/
    if (root_node->type == NODETYPE_LITERAL)
    {
        return root_node->data;
    }

    /* Then, it's a parent. Process all its children */
    index = 0;
    for (curr_child = root_node->first_child; curr_child != NULL; curr_child = curr_child->next)
    {
        /* Maybe implement a better container class like std::vector */
        some_container[index] = process_node(curr_child);
        index++;
        if (curr_child == root_node->last_child)
        {
            break;
        }
    }

    switch (root_node->type)
    {
    case NODETYPE_BLOCKQUOTE:
        /* Some processing code here. Use data from some_container. */
        output = "This node is a block quote";
        break;
    case NODETYPE_CODE:
        /* Some processing code here. Use data from some_container. */
        output = "This node is code";
        break;
    default:
        break;
    }

    return output;
}