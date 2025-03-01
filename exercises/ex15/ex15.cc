// Copyright (c) 2025 Z. Wu
// name: Zongze Wu
// UW email: zongzewu@uw.edu

#include <cstdlib>      // For EXIT_success
#include <iostream>     // for cout, cerr
#include <unistd.h>     // for close()

#include "file.h"
#include "network.h"


void Usage(char *progname);

int main(int argc, char **argv) {
  // check input parameters contains hostname, port and filename
  if (argc != 4) {
    Usage(argv[0]);
  }

  // extract hostname, port, filename, from user input
  char* hostname = argv[1];  // hostname is char*
  // cast second parameter to unsigned short
  unsigned short port = static_cast<unsigned short>(atoi(argv[2]));
  std::string filename = argv[3];  // filename is string type

  // Set receiving parameters
  struct sockaddr_storage ret_addr;
  size_t ret_addrlen;

  // analize domain name
  if (!LookUpName(hostname, port, &ret_addr, &ret_addrlen)) {
    std::cerr << "LookupName Failed on hostname: " << hostname << std::endl;
    return EXIT_FAILURE;
  }

  int dest_fd;

  // connect to the server
  if (!Connect(ret_addr, ret_addrlen, &dest_fd)) {
    std::cerr << "Connect failed" << std::endl;
    return EXIT_FAILURE;
  }

  // copy local file to the server
  // note that dest_fd is actually a local socket_fd that represent the server
  // because it's connected to the server.
  if (!CopyFileToFD(filename, dest_fd)) {
    std::cerr << "CopyFileToFD failed: " << filename << "->";
    std::cerr << hostname << std::endl;
    close(dest_fd);
    return EXIT_FAILURE;
  }

  // free the dest_fd
  close(dest_fd);

  return EXIT_SUCCESS;
}


void Usage(char *progname) {
  std::cerr << "usage: " << progname << " hostname port filename" << std::endl;
  exit(EXIT_FAILURE);
}
