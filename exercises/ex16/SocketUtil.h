// Copyright (c) 2025 Z. Wu
// name: Zongze Wu
// UW email: zongzewu@uw.edu

#ifndef _SOCKETUTIL_H_
#define _SOCKETUTIL_H_

#include <sys/socket.h>    // for sockaddr (struct)
#include <netdb.h>         // for getaddrinfo() and related structs

// listen to the given port number
int  Listen(char *portnum, int *sock_family);

// write the client sent data to the stdout
void HandleClient(int c_fd, struct sockaddr *addr, size_t addrlen,
                  int sock_family);

#endif  // _SOCKETUTIL_H_
