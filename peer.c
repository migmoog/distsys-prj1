#include "svec.h"
#include <errno.h>
#include <fcntl.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#define PORT 6969
#define STR(i) #i

int main(int argc, const char **argv) {
  if (argc < 3) {
    fprintf(stderr, "Usage: ./peer -h <hostsfile>\n");
    return 1;
  }

  int hf_arg_index = -1;
  for (int i = 1; i < argc; i++) {
    if (strcmp("-h", argv[i]) == 0) {
      hf_arg_index = i + 1;
    }
  }

  if (hf_arg_index < 0) {
    fprintf(stderr, "No -h flag.\n");
    return 2;
  } else if (hf_arg_index > argc) {
    fprintf(stderr, "-h flag provided without filename\n");
    return 2;
  }

  FILE *hosts_fd = fopen(argv[hf_arg_index], "r");
  if (hosts_fd == NULL) {
    fprintf(stderr, "File Error: %s\n", strerror(errno));
    return 2;
  }

  const int PNB_SIZE = 100;
  char peername_buf[PNB_SIZE];
  char hostname[PNB_SIZE];
  if (gethostname(hostname, PNB_SIZE) == -1) {
    fprintf(stderr, "Couldn't retrieve hostname\n");
    return 3;
  }

  svec_t *peers = vec_new();
  int backlog = 0;
  while (fgets(peername_buf, PNB_SIZE, hosts_fd) != NULL) {
    backlog++;

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
    vec_add(peers, peername_buf);
    memset(peername_buf, 0, PNB_SIZE);
  }

  vec_printerr(peers);

  // iterate thru and connect to other networks
  for (int i = 0; i < peers->len; i++) {
    const char *pn = peers->names[i];
  }

  vec_destroy(peers);
  return 0;
}
