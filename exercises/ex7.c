// Copyright (c) 2025 Z. Wu
// name: Zongze Wu
// UW email: zongzewu@uw.edu

#include <stdlib.h>   // for EXIT_SUCCESS
#include <stdio.h>    // for fwrite
#include <fcntl.h>    // for open()
#include <unistd.h>   // for close() write() read()
#include <string.h>   // for strlen strcmp
#include <stdbool.h>  // for true false
#include <errno.h>    // for EINTR EAGAIN errno
#include <dirent.h>   // for readdir closedir

#define MIN_LENGTH 4
#define SIZE 1024
#define PATH_MAX 2048

// return true if this fileName end with ".txt"
bool IsTxt(const char* fileName);
// Output the entire contents of fileName to stdout
void StdOut(const char* fileName);

int main(int argc, char** argv) {
  // check to make sure we have valid command line arguments
  if (argc != 2) {
    fprintf(stderr, "Usage: ./dirdump <dirname or path to the dir>\n");
    return EXIT_FAILURE;
  }

  // open the directory, look at opendir()
  DIR *dirp = opendir(argv[1]);
  if (dirp == NULL) {
    fprintf(stderr, "Could not open directory\n");
    return EXIT_FAILURE;
  }

  // read through/parse the directory
  struct dirent *entry;

  // read current directory
  entry = readdir(dirp);
  while (entry != NULL) {
    // if this is a ".txt" file, then concatenate its valid path and pass into
    // StdOut()
    if (IsTxt(entry->d_name)) {
      // store the txt file's full path
      char fullPath[PATH_MAX];
      int length = strlen(argv[1]);
      // determine whether the last string of the second argument is "/"
      if (argv[1][length-1] == '/') {
        snprintf(fullPath, sizeof(fullPath), "%s%s", argv[1], entry->d_name);
      } else {
        snprintf(fullPath, sizeof(fullPath), "%s/%s", argv[1], entry->d_name);
      }
      // copy the contents to stdout
      StdOut(fullPath);
  }
    // go to the next dir or file
    entry = readdir(dirp);
  }

  // clean up
  closedir(dirp);
  return EXIT_SUCCESS;
}

bool IsTxt(const char* fileName) {
  int length = strlen(fileName);
  // fileName less than 4 characters, no way it's a ".txt" file
  if (length <= MIN_LENGTH) {
    return false;
  }
  // determine if the last 4 characters are ".txt"
  return strcmp(fileName + length - MIN_LENGTH, ".txt") == 0;
}

void StdOut(const char *fileName) {
  // open this file by fileNmae(path), read only, and receiving returned fd
  int fd = open(fileName , O_RDONLY);
  // make sure the file is readable
  if (fd == -1) {
    fprintf(stderr, "Could not open file for reading\n");
    exit(EXIT_FAILURE);
  }
  // read the txt file into the buffer until EOF or an error occurs
  char buf[SIZE];
  ssize_t len;
  do {
    len = read(fd, buf, SIZE);
    // failed to read
    if (len == -1) {
      // Not interupt nor do it again, can only exit at this time
      if (errno != EINTR && errno != EAGAIN) {
        close(fd);
        perror(NULL);
        exit(EXIT_FAILURE);
      }
      // read again
      continue;
    }
    size_t total = 0;  // write how much bytes to the stdout in total
    ssize_t wlen;  // wrote how much bytes to the stdout this loop
    while (total < len) {
      wlen = write(1, buf + total, len - total);
      // failed to write
      if (wlen == -1) {
        // can not resume write, close file and exit
        if (errno != EINTR && errno != EAGAIN) {
          close(fd);
          perror(NULL);
          exit(EXIT_FAILURE);
        }
        // write again
        continue;
      }
      // update where the last write was made
      total += wlen;
    }
  } while (len != 0);
}
