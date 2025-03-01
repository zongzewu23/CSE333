// Copyright (c) 2025 Z. Wu
// name: Zongze Wu
// UW email: zongzewu@uw.edu

#include "network.h"

#include <arpa/inet.h>  // for AF_INET, AF_INET6
#include <assert.h>     // for assert()
#include <errno.h>      // for errno
#include <unistd.h>     // for close, read, write
#include <iostream>     // for std:cerr and std::endl
#include <string.h>     // for memset()


bool LookUpName(char *name,
                unsigned short port,
                struct sockaddr_storage *ret_addr,
                size_t *ret_addrlen) {
  // create hints and return results struct
  struct addrinfo hints, *results;
  // get error number returned from getaddrinfo()
  int retval;

  // initialize hints struct so that we can use "getaddrinfo()"
  memset(&hints, 0, sizeof(hints));
  // allow both IPV4 and IPv6
  hints.ai_family = AF_UNSPEC;
  // choose stream socket, suitable for TCP connection
  hints.ai_socktype = SOCK_STREAM;

  // Do the lookup by invoking getaddrinfo().
  if ((retval = getaddrinfo(name, nullptr, &hints, &results)) != 0) {
    std::cerr << "getaddrinfo failed: ";
    std::cerr << gai_strerror(retval) << std::endl;
    return false;
  }

  // Set the port in the first result.
  if (results->ai_family == AF_INET) {  // for IPv4 address
    struct sockaddr_in *v4addr = (struct sockaddr_in *) results-> ai_addr;
    v4addr->sin_port = htons(port);  // set port number using network order
  } else if (results->ai_family == AF_INET6) {  // for IPv6 address
    struct sockaddr_in6 *v6addr = (struct sockaddr_in6 *) results->ai_addr;
    v6addr->sin6_port = htons(port);
  } else {  // neither IPv4 nor Ipv6
    std::cerr << "getaddrinfo failed to provide an IPv4 or IPv6 address";
    std::cerr << std::endl;
    freeaddrinfo(results);  // free the linked list that getaddrinfo generated
    return false;
  }

  // Return the first result.
  assert(results != nullptr);  // make sure we got results
  // copy the queried results to ret_addr
  memcpy(ret_addr, results->ai_addr, results->ai_addrlen);
  // set up return parameter's addrlen
  *ret_addrlen = results->ai_addrlen;

  // clean up
  freeaddrinfo(results);
  return true;
}


bool Connect(const struct sockaddr_storage &addr,
  const size_t &addrlen,
  int *ret_fd) {
  // Generate a file descriptor for the local computer network buffer area
  int socket_fd = socket(addr.ss_family, SOCK_STREAM, 0);
  if (socket_fd == -1) {
    std::cerr << "socket() failed: " << strerror(errno) << std::endl;
    return false;
  }

  // connect the socket to the remote host
  // needs to cast the sockaddr_storage to sockaddr to match the type
  int res = connect(socket_fd,
    reinterpret_cast<const struct sockaddr *>(&addr), addrlen);
  if (res == -1) {
    std::cerr << "connect() failed: " << strerror(errno) << std::endl;
    return false;
  }

  // retunr the socket_fd though the return parameter
  *ret_fd = socket_fd;
  return true;
}

