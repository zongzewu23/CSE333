// Copyright (c) 2025 Z. Wu
// name: Zongze Wu
// UW email: zongzewu@uw.edu

#include "file.h"

#include <fcntl.h>    // for open()
#include <unistd.h>   // for close() write() read()
#include <stdbool.h>  // for true false
#include <errno.h>    // for EINTR EAGAIN errno
#include <string.h>   // For strerror
#include <iostream>   // for cout

#define BUF_SIZE 4096


ssize_t ReadFromFD(int fd, char *buf, size_t count) {
  ssize_t total_len = 0;  // record bytes being read
  ssize_t len;
  while (1) {
    // read from the last time position, and read count - total_len bytes
    len = read(fd, buf + total_len, count - total_len);
    if (len == 0) {  // EOF
      break;
    } else if (len == -1) {  // interrupted
      // interrupted by some non-recoverable causes
      if (errno != EINTR && errno != EAGAIN) {
        std::cerr << "read() failed: " << strerror(errno) << std::endl;
        return -1;
      }
      // interrupted by some recoverable causes, so try again
      continue;
    }
    // update total_len to keep track on who much bytes being read
    total_len += len;
  }

  return total_len;
}

ssize_t WriteToFD(int fd, const char *buf, size_t count) {
  ssize_t total_len = 0;
  ssize_t len;
  while (total_len < static_cast<ssize_t>(count)) {
    // Continue writing from the buffer where the last write operation
    // was performed, percisely indicating the number of bytes to be written
    len = write(fd, buf + total_len, count - total_len);
    if (len == -1) {
      if (errno != EINTR && errno != EAGAIN) {
        // interrupted by some non-recoverable causes
        std::cerr << "Write() failed: " << strerror(errno) << std::endl;
        return -1;
      }
      // interrupted by some recoverable causes, so try again
      continue;
    }
    // keep track of how much bytes have been written
    total_len += len;
  }

  return total_len;
}

bool CopyFileToFD(const std::string &filename, int dest_fd) {
  // open this file by fileNmae(path), read only, and receiving returned fd
  int source_fd = open(filename.c_str(), O_RDONLY);
  // make sure the file is opened
  if (source_fd == -1) {
    std::cerr << "Open() failed: " << strerror(errno) << std::endl;
    return false;
  }

  char buf[BUF_SIZE];
  ssize_t bytes_read;

  // keep read bytes from source_fd to buffer until nothing to read
  while ((bytes_read = ReadFromFD(source_fd, buf, sizeof(buf))) > 0) {
    // write bytes from buffer to dest_fd
    if (WriteToFD(dest_fd, buf, bytes_read) == -1) {
      close(source_fd);
      return false;
    }
  }

  // make sure at least successfully read some bytes
  if (bytes_read == -1) {
    close(source_fd);
    return false;
  }

  // clean up
  close(source_fd);
  return true;
}
