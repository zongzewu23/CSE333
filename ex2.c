// Copyright (c) 2025 Z. Wu
// name: Zongze Wu
// UW email: zongzewu@uw.edu

#include <stdio.h>  // for printf
#include <stdlib.h>  // for EXIT_SUCCESS
#include <stdint.h>  // for int32_t, uint8_t
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
  // print the num_bytes  and the pointer mem_addr by using
  // formatting %d and %p, notice that the format of %p include "0x"
  printf("The %d bytes starting at %p are: ", num_bytes, mem_addr);
  // cast the void* to a char* to access the memory byte by byte
  char *ptr = (char*) mem_addr;
  // use for loop to iterate for each byte in the variable
  for (int i = 0; i < num_bytes; i++) {
    // use marco PRIx8 to provide the correct format specifier for
    // printing the values of type
    // uint8_t. Also %02 means that if there is not enough two hex number,
    // then fill the left with0
    printf("%02" PRIx8 " ", (uint8_t)*(ptr + i));
  }
  // line breaks
  printf("\n");
}
