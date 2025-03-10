/*
 * Copyright ©2025 Hal Perkins.  All rights reserved.  Permission is
 * hereby granted to students registered for University of Washington
 * CSE 333 for use solely during Winter Quarter 2025 for purposes of
 * the course.  No other use, copying, distribution, or modification
 * is permitted without prior written consent. Copyrights for
 * third-party components of this work must be honored.  Instructors
 * interested in reusing these course materials should contact the
 * author.
 */

#include <stdio.h>       // for snprintf()
#include <unistd.h>      // for close(), fcntl()
#include <sys/types.h>   // for socket(), getaddrinfo(), etc.
#include <sys/socket.h>  // for socket(), getaddrinfo(), etc.
#include <arpa/inet.h>   // for inet_ntop()
#include <netdb.h>       // for getaddrinfo()
#include <errno.h>       // for errno, used by strerror()
#include <string.h>      // for memset, strerror()
#include <iostream>      // for std::cerr, etc.

#include "./ServerSocket.h"

extern "C" {
  #include "libhw1/CSE333.h"
}

#define MAX_DNS 1024

using std::string;

namespace hw4 {

ServerSocket::ServerSocket(uint16_t port) {
  port_ = port;
  listen_sock_fd_ = -1;
}

ServerSocket::~ServerSocket() {
  // Close the listening socket if it's not zero.  The rest of this
  // class will make sure to zero out the socket if it is closed
  // elsewhere.
  if (listen_sock_fd_ != -1)
    close(listen_sock_fd_);
  listen_sock_fd_ = -1;
}

bool ServerSocket::BindAndListen(int ai_family, int *const listen_fd) {
  // Use "getaddrinfo," "socket," "bind," and "listen" to
  // create a listening socket on port port_.  Return the
  // listening socket through the output parameter "listen_fd"
  // and set the ServerSocket data member "listen_sock_fd_"

  // STEP 1:
  // Populate the "hints" addrinfo structure for getaddrinfo().
  // ("man addrinfo")
  struct addrinfo hints;
  memset(&hints, 0, sizeof(struct addrinfo));
  hints.ai_family = AF_INET6;       // IPv6 (also handles IPv4 clients)
  hints.ai_socktype = SOCK_STREAM;  // stream
  hints.ai_flags = AI_PASSIVE;      // use wildcard "in6addr_any" address
  hints.ai_flags |= AI_V4MAPPED;    // use v4-mapped v6 if no v6 found
  hints.ai_protocol = IPPROTO_TCP;  // tcp protocol
  hints.ai_canonname = nullptr;
  hints.ai_addr = nullptr;
  hints.ai_next = nullptr;

  // Use argv[1] as the string representation of our portnumber to
  // pass in to getaddrinfo().  getaddrinfo() returns a list of
  // address structures via the output parameter "result".
  struct addrinfo *result;
  char port_str[6];
  snprintf(port_str, sizeof(port_str), "%u", port_);
  int res = getaddrinfo(nullptr, port_str, &hints, &result);

  // Did addrinfo() fail?
  if (res != 0) {
    std::cerr << "getaddrinfo() failed: ";
    std::cerr << gai_strerror(res) << std::endl;
    return -1;
  }

  // Loop through the returned address structures until we are able
  // to create a socket and bind to one.  The address structures are
  // linked in a list through the "ai_next" field of result.
  *listen_fd = -1;
  for (struct addrinfo *rp = result; rp != nullptr; rp = rp->ai_next) {
    *listen_fd = socket(rp->ai_family,
                       rp->ai_socktype,
                       rp->ai_protocol);
    if (*listen_fd == -1) {
      // Creating this socket failed.  So, loop to the next returned
      // result and try again.
      std::cerr << "socket() failed: " << strerror(errno) << std::endl;
      *listen_fd = -1;
      continue;
    }

  // Configure the socket; we're setting a socket "option."  In
  // particular, we set "SO_REUSEADDR", which tells the TCP stack
  // so make the port we bind to available again as soon as we
  // exit, rather than waiting for a few tens of seconds to recycle it.
  int optval = 1;
  setsockopt(*listen_fd, SOL_SOCKET, SO_REUSEADDR,
              &optval, sizeof(optval));

  // Try binding the socket to the address and port number returned
  // by getaddrinfo().
  if (bind(*listen_fd, rp->ai_addr, rp->ai_addrlen) == 0) {
    // Bind worked! Return to the caller the address family.
    sock_family_ = rp->ai_family;
     break;
  }

  // The bind failed.  Close the socket, then loop back around and
  // try the next address/port returned by getaddrinfo().
  close(*listen_fd);
  *listen_fd = -1;
}

  // Free the structure returned by getaddrinfo().
  freeaddrinfo(result);

  // If we failed to bind, return failure.
  if (*listen_fd <= 0) {
    return *listen_fd;
  }

  // Success. Tell the OS that we want this to be a listening socket.
  if (listen(*listen_fd, SOMAXCONN) != 0) {
    std::cerr << "Failed to mark socket as listening: ";
    std::cerr << strerror(errno) << std::endl;
    close(*listen_fd);
    return -1;
  }

  // Set the listen_sock_fd private field, note that the return param is set
  listen_sock_fd_ = *listen_fd;
  return true;
}

bool ServerSocket::Accept(int *const accepted_fd,
                          string *const client_addr,
                          uint16_t *const client_port,
                          string *const client_dns_name,
                          string *const server_addr,
                          string *const server_dns_name) const {
  // STEP 2:

  // Wait for a client to arrive.
  int client_fd;
  // have to put it outside of the while loop so we can access it
  // when looking up for the client IP
  struct sockaddr_storage caddr;
  socklen_t caddr_len = sizeof(caddr);
  while (1) {
    client_fd = accept(listen_sock_fd_,
                reinterpret_cast<struct sockaddr *>(&caddr), &caddr_len);
    if (client_fd < 0) {
      if ((errno == EAGAIN)||(errno == EINTR)) continue;
      std::cerr << "Failure on accept: " << strerror(errno) << std::endl;
      return false;
    }
    break;
  }

  // set the return parameter, this is the fd for the accepted client
  *accepted_fd = client_fd;

  // get the client IP and port, notice that
  // the client's ai_family has to be the same as server's socket_family_
  // to make a connection. Therefore we know the client's IPv? by checking
  // sock_family_
  if (sock_family_ == AF_INET) {
    // caddr is assigned some value once the client is connect and while stop
    // it contains the clients info, extract it to get client's addr and port
    struct sockaddr_in *v4addr = (struct sockaddr_in*) &caddr;
    char ip_str[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &(v4addr->sin_addr), ip_str, INET_ADDRSTRLEN);
    *client_addr = ip_str;
    *client_port = v4addr->sin_port;
  } else if (sock_family_ == AF_INET6) {
    // IPv6 has special postfix
    struct sockaddr_in6 *v6_addr = (struct sockaddr_in6*) &caddr;
    // client's IP address is transferred in bytes
    char ip_str[INET6_ADDRSTRLEN];
    // therefore use inet_ntop to convert from network to presentation
    // (convert binary IP address to a readable string)s
    inet_ntop(AF_INET6, &(v6_addr->sin6_addr), ip_str, INET6_ADDRSTRLEN);
    *client_addr = ip_str;
    // extract client's info
    *client_port = v6_addr->sin6_port;
  } else {
    std::cerr << "Not a valid IPv:" << sock_family_ << std::endl;
    return false;
  }

  // find the client's dns name
  char client_dns[MAX_DNS];  // ought to be big enough.
  if (getnameinfo(reinterpret_cast<struct sockaddr *>(&caddr),
                caddr_len, client_dns, MAX_DNS, nullptr, 0, 0) != 0) {
    snprintf(client_dns, sizeof(client_dns), "[reverse DNS failed]");
    return false;
  }
  *client_dns_name = client_dns;

  char server_dns[MAX_DNS];
  server_dns[0] = '\0';

  if (sock_family_ == AF_INET) {
    struct sockaddr_in srvr;
    socklen_t srvrlen = sizeof(srvr);
    char addrbuf[INET_ADDRSTRLEN];
    getsockname(client_fd, (struct sockaddr*) &srvr, &srvrlen);
    inet_ntop(AF_INET, &srvr.sin_addr, addrbuf, INET_ADDRSTRLEN);
    getnameinfo((const struct sockaddr *) &srvr, srvrlen,
                              server_dns, MAX_DNS, nullptr,0 ,0);
    *server_dns_name = server_dns;
    *server_addr = addrbuf;
  } else {
    struct sockaddr_in6 srvr;
    socklen_t srvrlen = sizeof(srvr);
    char addrbuf[INET6_ADDRSTRLEN];
    getsockname(client_fd, (struct sockaddr *) &srvr, &srvrlen);
    inet_ntop(AF_INET6, &srvr.sin6_addr, addrbuf, INET6_ADDRSTRLEN);
    getnameinfo((const struct sockaddr *) &srvr, srvrlen,
                              server_dns, MAX_DNS, nullptr,0 ,0);
    *server_dns_name = server_dns;
    *server_addr = addrbuf;
  }

  return true;
}

}  // namespace hw4
