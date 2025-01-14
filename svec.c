#include "svec.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

svec_t *vec_new() {
  svec_t *out = (svec_t *)malloc(sizeof(svec_t));
  out->capacity = 2;
  out->len = 0;
  out->names = (char **)malloc(sizeof(char *) * out->capacity);
  return out;
}

void vec_add(svec_t *v, const char *s) {
  v->len++;
  if (v->len == v->capacity) {
    v->capacity *= 2;
    v->names = (char **)realloc(v->names, sizeof(char *) * v->capacity);
  }

  v->names[v->len - 1] = strdup(s);
}

void vec_destroy(svec_t *v) {
  for (int i = 0; i < v->len; i++) {
    free(v->names[i]);
  }
  free(v->names);
  free(v);
}

void vec_printerr(svec_t *v) {
  for (int i = 0; i < v->len; i++) {
    fprintf(stderr, "%s\n", v->names[i]);
  }
}
