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

#define PNB_SIZE 100
svec_t *vec_from_list(FILE *flist, const char *hostname) {
  svec_t *out = vec_new();
  char peername_buf[PNB_SIZE];
  while (fgets(peername_buf, PNB_SIZE, flist) != NULL) {

    // trim whitespace because c smells like moldy gym socks
    const int l = strlen(peername_buf);
    if (peername_buf[l - 1] == '\n') {
      peername_buf[l - 1] = '\0';
    }

    if (strcmp(hostname, peername_buf) == 0) {
      // found our own name in the file, don't need it
      fprintf(stderr, "My name is %s\n", hostname);
      continue;
    }
    vec_add(out, peername_buf);
    memset(peername_buf, 0, PNB_SIZE);
  }

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
