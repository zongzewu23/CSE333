// name: Zongze Wu
// UW email: zongzewu@uw.edu

#include <stdio.h> // for printf
#include <inttypes.h> // for PRId64
#include <stdint.h> //for int64_t, int32_t
#include <stdlib.h> // for EXIT_SUCCESS

// Print out the usage of the program and exit.
void Usage(void);

//Copy every element in src to dst one by one while sorting the dst using insertion sort
void CopyAndSort(int64_t src[], int64_t dst[], int32_t size);

//The implementation of insertion sort with non-decreasing order
void insertionSort(int64_t dst[], int64_t element, int32_t position);

//print the array one by one in a row, 
//however ignoring the detail about spaces between the elements
void printArray(int64_t array[], int32_t size);

int main(int argc, char**argv){
  //initialize the testing array and it's sorted copy
  int64_t original[] = {3, 2, -5, 7, 17, 42, 6, 333, 7, 8, -8, 6};
  int64_t copy[12];

  //pass two arrays and the size of the array as parameters
  CopyAndSort(original, copy, 12);
  printArray(copy, 12);
  
  //quit
  return EXIT_SUCCESS;
}

void CopyAndSort(int64_t src[], int64_t dst[], int32_t size){
  if(size <= 0){
    Usage();//handle edge case of empty array
  }
  //copy the first element in src no matter what, because it has no comparable element.
  //Notice that by this, We do not need to worry about the initialization about dst.
  dst[0] = src[0];

  //insert all other elements from src into dst in non_decreasing order.
  for(int32_t i = 1; i < size; i++){
  //As we passing in the last sorted position in dst, we won't worry about whether the dst is
  //initialized, because we will focus on the sorted portion and the uninitialized portion will be
  //overwritten one by one
  insertionSort(dst, src[i], i - 1);  
  }
}

void insertionSort(int64_t dst[], int64_t element, int32_t position){
  //Find the smallest position that is bigger than the element we are about to insert, and shift
  //those bigger element in dst to the right to achieve ascending effect. 
  while(dst[position] > element && position >= 0){
    dst[position + 1] = dst[position];
    position -= 1;
  }
  //Insert the element
  dst[position + 1] = element;
}

void printArray(int64_t array[], int32_t size){
   if(size <= 0){
    Usage();//handle edge case of empty array
  }
 //print the element in the array one by one
  for(int32_t i = 0; i < size; i++){
    //for Portability 
    printf("%" PRId64 " ", array[i]);
  }
  //Line Break  
  printf("\n");
}

void Usage(void) {
    fprintf(stderr,
            "usage: ./ex1\n"
            "This program sorts a fixed array of int64_t elements using insertion sort.\n"
            "The sorted result will be printed in non-decreasing order.\n");
    exit(EXIT_FAILURE);
}

