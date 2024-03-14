typedef struct
{
    char *data;
    /* Number of elements in the vector */
    int size;
    /* Size of each vector element */
    int elem_size;
    /* Maximum number of elements in the vector before data needs realloc
    Will be increased and decreased exponentially.
    For example, if a c_vector with 5 is initialized, it's capacity will be 8.
    When it eventually grows to 8 elements and push_back is called, it will increase its capacity to 16.
    The capacity will only be realloc'd back to 8 when there is (capacity/4) elements left in the vector (So in this case, 4 elements)*/
    int capacity;
} Cvector;

/* Initialize a Cvector.
If count more than 0 is specified, data will be null-initialized. */
Cvector *cvec_init(int count, int elem_size);

/* Copies elem_size bytes from value and puts it at end of data */
void cvec_push_back(Cvector *cvector, void *value);

/* Removes last element from Cvector.
Intentionally doesn't return anything because if it returns something, the return value will have to be freed by the caller.
It would be inconvenient.*/
void cvec_pop_back(Cvector *cvector);

void cvec_clear(Cvector *cvector);

/*
Gets pointer to the element at certain index.
Pointer to the element may be invalidated upon clear, push_back, pop_back
So copy the data if you want to store it for a long time
*/
void *cvec_at(Cvector *cvector, int index);

/*
Writes to a specific index in the cvector.
More specifically, copies cvector->elem_size bytes from *value to the vector's index.
Technically, you can also write to the pointer returned by cvec_at. This is just a convenience function
*/
void cvec_put(Cvector *cvector, int index, void *value);

/* Frees Cvector */
void cvec_free(Cvector *cvector);