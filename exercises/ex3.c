// Copyright (c) 2025 Z. Wu
// Name: Zongze Wu
// UW Email: zongzewu@uw.edu

#include <stdint.h>  // For unint16_t
#include <stdio.h>   // For printf
#include <stdlib.h>  // For EXIT_SUCCESS, EXIT_FAILURE malloc, and free

typedef struct point3d_st {
  uint16_t x, y, z;
} Point3d, *Point3dPtr;

// Use malloc to allocate memory for the point3d_st struct,
// also assign the passed-in parameters to each fields in the struct.
// Return a NULL pointer if not enough memory.
// Return a Point3dPtr pointer if everything goes smoothly.
Point3dPtr AllocatePoint3d(uint16_t param_x,
                            uint16_t param_y, uint16_t param_z);

int main(int argc, char** argv) {
  // Call AllocatePoint3d and pass in parameters
  Point3dPtr point = AllocatePoint3d(1, 23, 32);

  // AllocatedPoint3d returns NULL pointer
  if (point == NULL) {
    fprintf(stderr, "Failed on AllocatePoint3d, no pointer assigned\n");
    return EXIT_FAILURE;
  }

  // Function returns a pointer but the value assigned is wrong
  if (point->x != 1 || point->y != 23 || point->z != 32) {
    fprintf(stderr, "Pointer assigned, But struct fields have wrong value\n");
    return EXIT_FAILURE;
  }

  // Successfully malloced spaces for a point and assigned correct values
  // Therefore free the pointer and terminate silently.
  free(point);
  return EXIT_SUCCESS;
}

Point3dPtr AllocatePoint3d(uint16_t param_x,
                            uint16_t param_y, uint16_t param_z) {
  // Use malloc to allocate enough space for a 3dpoint struct
  Point3dPtr point_ptr = (Point3dPtr) malloc (3*sizeof(uint16_t));

  if (point_ptr == NULL) {
    return NULL;  // Failed on malloc
  }

  // Assign value to fields
  point_ptr->x = param_x;
  point_ptr->y = param_y;
  point_ptr->z = param_z;

  // Return a pointer to the allocated struct fields
  return point_ptr;
}
