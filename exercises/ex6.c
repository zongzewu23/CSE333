// Copyright (c) 2025 Z. Wu
// Name: Zongze Wu
// UW Email: zongzewu@uw.edu

#include <stdio.h>    // For foppen, fclose, fseek. ftell, fread, printf
#include <stdlib.h>   // For EXIT_SUCCESS EXIT_FAILURE
#include <errno.h>    // For perror

int main(int argc, char** argv) {
  FILE *fin;

  if (argc != 2) {
    fprintf(stderr, "Can only read one file");
    return EXIT_FAILURE;
  }

  fin = fopen(argv[1], "rb");  // Open file with Read, binary mode
  if (fin == NULL) {
    // Return failure if failed on fopen
    fprintf(stderr, "%s --- ", argv[1]);
    perror("fopen for read failed");
    return EXIT_FAILURE;
  }

  if (fseek(fin, 0, SEEK_END) != 0) {
    // Return failure if fseek doesn't return 0 .i.e failed
    perror("fseek failed");
    return EXIT_FAILURE;
  }

  // The pointer who reading the file is now at the end of the file
  // so ftell can tell the total size of the file
  long size = ftell(fin);
  if (size == -1) {
    perror("ftell failed");
    return EXIT_FAILURE;
  }

  // For properly read the file from its end to its begin
  size--;

  while (size >= 0) {
    char ch;

    // Set the pointer to the position we want to read
    if (fseek(fin, size, SEEK_SET) != 0) {
      perror("fseek failed");
      return EXIT_FAILURE;
    }

    // Read the char to the ch
    if (fread((void*)&ch, sizeof(char), 1, fin) != sizeof(char)) {
      perror("fread failed");
      return EXIT_FAILURE;
    }

    // print ch
    printf("%c", ch);

    size--;
  }

  // Close file
  fclose(fin);

  return EXIT_SUCCESS;
}
