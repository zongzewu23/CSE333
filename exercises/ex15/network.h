// Copyright (c) 2025 Z. Wu
// name: Zongze Wu
// UW email: zongzewu@uw.edu
#ifndef _NETWORK_H_
#define _NETWORK_H_

#include <sys/types.h>     // for size_t
#include <sys/socket.h>    // for sockaddr_storage (struct)
#include <netdb.h>         // for getaddrinfo() and related structs


// find out IP address through domain name, still works fine if pass in IP
// return true if successfully returned a sockaddr_storage and ret_addrlen
// false otherwise
bool LookUpName(char *name,
  unsigned short port,
  struct sockaddr_storage *ret_addr,
  size_t *ret_addrlen);

// connect the host and server by using sockaddr_storage and addrlen, return
// true if successfully connected, and a File describution will be returned,
// false otherwise
bool Connect(const struct sockaddr_storage &addr,
  const size_t &addrlen,
  int *ret_fd);

#endif  // _NETWORK_H_
