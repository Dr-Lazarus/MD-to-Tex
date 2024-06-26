#include "cvector.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

Cvector *cvec_init(int count, int elem_size) {
  Cvector *cvector = (Cvector *)malloc(sizeof(Cvector));
  int capacity = 4;

  if (elem_size == 0) {
    printf("cvec_init: Can't initialize with elem_size = 0");
    exit(EXIT_FAILURE);
  }

  cvector->size = count;
  cvector->elem_size = elem_size;

  /* Get nearest power of two */
  while (capacity < count) {
    capacity *= 2;
  }
  cvector->capacity = capacity;
  cvector->data = (char *)calloc(capacity, elem_size);

  return cvector;
}

void cvec_push_back(Cvector *cvector, void *value) {
  cvector->size++;
  // Increase capacity if doesn't fit
  if (cvector->size > cvector->capacity) {
    cvector->capacity *= 2;
    cvector->data =
        (char *)realloc(cvector->data, cvector->elem_size * cvector->capacity);
  }

  cvec_put(cvector, cvector->size - 1, value);
}

void cvec_pop_back(Cvector *cvector) {
  cvector->size--;

  // Decrease capacity if cvector->size < capacity/4
  // Capacity can't go below 4 after decrease
  if (cvector->size <= cvector->capacity / 4 && cvector->capacity / 2 >= 4) {
    cvector->capacity /= 2;
    cvector->data =
        (char *)realloc(cvector->data, cvector->elem_size * cvector->capacity);
  }
}

void cvec_clear(Cvector *cvector) {
  free(cvector->data);
  cvector->data = (char *)calloc(4, cvector->elem_size);
  cvector->size = 0;
  cvector->capacity = 4;
}

void *cvec_at(Cvector *cvector, int index) {
  if (index >= cvector->size) {
    printf("cvec_put: Index requested out of range");
    exit(EXIT_FAILURE);
  }

  return &cvector->data[index * cvector->elem_size];
}

void cvec_put(Cvector *cvector, int index, void *value) {
  if (index >= cvector->size) {
    printf("cvec_put: Index requested out of range");
    exit(EXIT_FAILURE);
  }

  memcpy(&cvector->data[index * cvector->elem_size], value, cvector->elem_size);
}

void cvec_free(Cvector *cvector) {
  free(cvector->data);
  free(cvector);
}
