#ifndef SVEC_H
#define SVEC_H

typedef struct {
  char **names;
  int capacity;
  int len;
} svec_t;

// constructs a new string vector
svec_t *vec_new();

// adds a copy of s to the vector
void vec_add(svec_t *v, const char *s);

// frees the vector from memory
void vec_destroy(svec_t *v);

// prints vec to stderr
void vec_printerr(svec_t *v);
#endif
