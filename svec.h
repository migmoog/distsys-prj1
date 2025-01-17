#ifndef SVEC_H
#define SVEC_H

#include <stdio.h>

typedef struct {
  char **names;
  int capacity;
  int len;
} svec_t;

// constructs a new string vector
svec_t *vec_new();

// creates a string vec from a list of newlines
// NOTE: you still have to close the file
svec_t *vec_from_list(FILE *flist, const char *hostname);

// adds a copy of s to the vector
void vec_add(svec_t *v, const char *s);

// frees the vector from memory
void vec_destroy(svec_t *v);

// prints vec to stderr
void vec_printerr(svec_t *v);

#endif
