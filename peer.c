#include "svec.h"
#include <fcntl.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#define PORT "6969"
#define ALIVE_MSG "I am alive!"
#define MSG_MAX 128 // max size for a message

#define PNB_SIZE 100
char hostname[PNB_SIZE];

// helper since all peers should be built the same way
struct addrinfo *get_peer_info(const char *pname) {
  struct addrinfo hints, *peer_res;
  memset(&hints, 0, sizeof hints);
  hints.ai_socktype = SOCK_DGRAM;
  hints.ai_family = AF_UNSPEC;
  hints.ai_flags = AI_PASSIVE;
  int status;
  if ((status = getaddrinfo(pname, PORT, &hints, &peer_res)) != 0) {
    fprintf(stderr, "Failed to get: %s.\n\t reason: %s\n", pname,
            gai_strerror(status));
    return NULL;
  }

  return peer_res;
}

// this func awaits the "I am alive" messages from this process' peers
// @param pname: peer name to retrieve the addrinfo of
// @param listen_fd: file descriptor returned after the accept call
// @returns 0 on success but:
// 1 -> if it couldn't recvfrom the peer's message
// 2 -> received a message but wasn't the alive message
int hear_peer(struct addrinfo *peer_res, int listen_fd) {
  char msg[MSG_MAX];
  socklen_t fromlen = sizeof(struct sockaddr_storage);
  int bytes_got;
  if ((bytes_got = recvfrom(listen_fd, msg, MSG_MAX, 0, peer_res->ai_addr,
                            &fromlen)) == -1) {
    perror("recvfrom");
    return 1;
  }
  msg[bytes_got] = '\0';
  if (strcmp(ALIVE_MSG, msg) == 0)
    return 0;

  return 2;
}

// Sends the "I am alive message" to the specified peer
// @param peer_addr: address info necessary for UDP sendto calls
int notify_peer(struct sockaddr *peer_addr, int sockfd) {
  socklen_t al = sizeof(struct sockaddr_storage);
  if (sendto(sockfd, ALIVE_MSG, MSG_MAX, 0, peer_addr, al) == -1) {
    perror("sendto");
    return 1;
  }
  return 0;
}

int main(int argc, const char **argv) {
  if (argc < 3) {
    fprintf(stderr, "Usage: ./peer -h <hostsfile>\n");
    return 1;
  }

  // cli flag processing
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
    return 3;
  }

  FILE *hosts_fd = fopen(argv[hf_arg_index], "r");
  if (hosts_fd == NULL) {
    perror("File error\n");
    return 3;
  }

  // collect names from file
  if (gethostname(hostname, PNB_SIZE) == -1) {
    perror("Couldn't retrieve hostname\n");
    return 4;
  }

  svec_t *peers = vec_from_list(hosts_fd, hostname);
  vec_printerr(peers); // for sureness sake

  struct addrinfo *self_res = get_peer_info(hostname);
  if (self_res == NULL)
    return 5;

  // setup our socket so that we may listen for connections
  int sockfd =
      socket(self_res->ai_family, self_res->ai_socktype, self_res->ai_protocol);
  if (bind(sockfd, self_res->ai_addr, self_res->ai_addrlen) == -1) {
    perror("bind");
    return 6;
  }
  sleep(5); // might be a lil hacky but eh

  // sending "I am alive"s
  struct {
    int sockfd;
    struct addrinfo *pai;
  } mailboxes[peers->len];
  for (int i = 0; i < peers->len; i++) {
    const char *pname = peers->names[i];
    struct addrinfo *pa = get_peer_info(pname);

    mailboxes[i].pai = pa;
    mailboxes[i].sockfd =
        socket(pa->ai_family, pa->ai_socktype, pa->ai_protocol);
    int status;
    if ((status = notify_peer(mailboxes[i].pai->ai_addr,
                              mailboxes[i].sockfd)) != 0) {
      return 7;
    }
  }

  // listening for "I am alive"s
  int msgc = 0;
  for (; msgc < peers->len; msgc++) {
    int pstatus;
    if ((pstatus = hear_peer(mailboxes[msgc].pai, sockfd)) != 0) {
      fprintf(stderr, "Error in hearing peer%d\n", pstatus);
      return 8;
    }
  }
  fprintf(stderr, "READY\n");

  // cleanup
  freeaddrinfo(self_res);
  for (int i = 0; i < peers->len; i++) {
    close(mailboxes[i].sockfd);
    freeaddrinfo(mailboxes[i].pai);
  }
  vec_destroy(peers);
  fclose(hosts_fd);

  return 0;
}
