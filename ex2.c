// Copyright (c) 2025 Z. Wu
// name: Zongze Wu
// UW email: zongzewu@uw.edu

#include <stdio.h>  // for printf
#include <stdlib.h>  // for EXIT_SUCCESS
#include <stdint.h>  // for int32_t, uint8_t， uintptr_t
#include <inttypes.h>  // for PRIx8

// print the values of the bytes allocated to some variable in hex.
void PrintBytes(void* mem_addr, int num_bytes);

int main(int argc, char **argv) {
  char     char_val = '0';
  int32_t  int_val = 1;
  float    float_val = 1.0;
  double   double_val  = 1.0;

  typedef struct {
    char     char_val;
    int32_t  int_val;
    float    float_val;
    double   double_val;
  } Ex2Struct;

  Ex2Struct struct_val = { '0', 1, 1.0, 1.0 };

  PrintBytes(&char_val, sizeof(char));
  PrintBytes(&int_val, sizeof(int32_t));
  PrintBytes(&float_val, sizeof(float));
  PrintBytes(&double_val, sizeof(double));
  PrintBytes(&struct_val, sizeof(struct_val));

  return EXIT_SUCCESS;
}

void PrintBytes(void* mem_addr, int num_bytes) {
  // Cast mem_addr to a pointer to uint8_t to access the memory byte by byte
  uint8_t* ptr = (uint8_t*) mem_addr;
  // Print the starting memory address in hexadecimal and the number of bytes
  printf("The %d bytes starting at 0x%" PRIxPTR " are: ",
      num_bytes, (uintptr_t)mem_addr);

  // Use for loop to iterate through the bytes
  for (int i = 0; i < num_bytes; i++) {
    // Use the macro PRIx8 to format the output for uint8_t type in hexadecimal
    // %02 ensures two hexadecimal digits are printed.
    // The (i > 0) ? " " : "" ensures a space is printed between bytes
    // but not before the first byte
    printf("%s%02" PRIx8, (i > 0) ? " " : "", *(ptr + i));
    // (i > 0)?" ":"" from ex1 solution
  }
  // Print a newline
  printf("\n");
}

