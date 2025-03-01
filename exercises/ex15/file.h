// Copyright (c) 2025 Z. Wu
// name: Zongze Wu
// UW email: zongzewu@uw.edu
#ifndef _FILE_H_
#define _FILE_H_

#include <sys/types.h>    // for ssize_t
#include <string>         // for string

// continuously read bytes from fd to buffer until there is nothing to read
// return the number of bytes read or -1 if function failed
ssize_t ReadFromFD(int fd, char *buf, size_t count);

// continuously write bytes from buffer to fd until it write count # of bytes
// return number of bytes write if succeed, -1 if function failed
ssize_t WriteToFD(int fd, const char *buf, size_t count);

// copy the content from filename to fd
// return true if succeed, false otherwise
bool CopyFileToFD(const std::string &filename, int dest_fd);

#endif  // _FILE_H_
