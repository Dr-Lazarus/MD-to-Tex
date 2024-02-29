typedef struct
{
    void *data;
    int size;      /* Number of elements in the vector */
    int elem_size; /* Size of each vector element */
    int capacity;  /* Maximum size of the vector before data* needs realloc */
} Cvector;

/* Initialize a Cvector */
Cvector *cvec_init(int count, int elem_size);

/* Returns number of elements */
int cvec_size(Cvector *cvector);

/* Returns size (in bytes) of each element */
int cvec_elem_size(Cvector *cvector);

/* Copies elem_size bytes from value and puts it at end of data */
void cvec_push_back(Cvector *cvector, void *value);

/* Removes last element from Cvector. */
void cvec_pop_back(Cvector *cvector);

void cvec_clear(Cvector *cvector);

/*
Gets element at certain index.
Pointer to the element may be invalidated upon clear, push_back, pop_back
So copy the data if you want to store it
*/
void *cvec_at(Cvector *cvector, int index);

/* Frees Cvector */
void cvec_free(Cvector *cvector);
