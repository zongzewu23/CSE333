// Copyright (c) 2025 Z. Wu
// name: Zongze Wu
// UW email: zongzewu@uw.edu

#include <cstdlib>      // For EXIT_success
#include <iostream>     // for cout, cerr
#include <unistd.h>     // for close()
#include <string.h>     // for strerror()

#include "SocketUtil.h"

void Usage(char *progname);

int main(int argc, char **argv) {
  // Expect the port number as a command line argument.
  if (argc != 2) {
    Usage(argv[0]);
  }

  int sock_family;
  // set up the listening socket and its fd, waiting for incoming client
  // notice that listen_fd is not for data transferring, but only for
  // making connections
  int listen_fd = Listen(argv[1], &sock_family);
  if (listen_fd <= 0) {
    // We failed to bind/listen to a socket.  Quit with failure.
    std::cerr << "Couldn't bind to any addresses." << std::endl;
    return EXIT_FAILURE;
  }

  // Loop until get one client successfully connected to the listening port,
  // accepting a connection from a client and writing the data that the client
  // sent to the stdout
  int client_fd = -1;  // set it to -1 to indicate no connection is made
  // only connect to one client, read write and close
  while (client_fd == -1) {
    struct sockaddr_storage caddr;
    socklen_t caddr_len = sizeof(caddr);
    // make connection, this client_fd is for transferring data from client
    client_fd = accept(listen_fd,
                           reinterpret_cast<struct sockaddr *>(&caddr),
                           &caddr_len);
    // keep waiting until one connection is made
    if (client_fd < 0) {
      if ((errno == EINTR) || (errno == EAGAIN) || (errno == EWOULDBLOCK))
        continue;
      std::cerr << "Failure on accept: " << strerror(errno) << std::endl;
      break;
    }

    // handle client transferred data
    HandleClient(client_fd,
                 reinterpret_cast<struct sockaddr *>(&caddr),
                 caddr_len,
                 sock_family);
  }

  // close listening socket before exit
  close(listen_fd);
  return EXIT_SUCCESS;
}

void Usage(char *progname) {
  std::cerr << "usage: " << progname << " port" << std::endl;
  exit(EXIT_FAILURE);
}
