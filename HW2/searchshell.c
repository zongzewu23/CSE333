/*
 * Copyright ©2025 Hal Perkins.  All rights reserved.  Permission is
 * hereby granted to students registered for University of Washington
 * CSE 333 for use solely during Winter Quarter 2025 for purposes of
 * the course.  No other use, copying, distribution, or modification
 * is permitted without prior written consent. Copyrights for
 * third-party components of this work must be honored.  Instructors
 * interested in reusing these course materials should contact the
 * author.
 */

// Feature test macro for strtok_r (c.f., Linux Programming Interface p. 63)
#define _XOPEN_SOURCE 600

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>

#include <errno.h>

#include "libhw1/CSE333.h"
#include "./CrawlFileTree.h"
#include "./DocTable.h"
#include "./MemIndex.h"

//////////////////////////////////////////////////////////////////////////////
// Helper function declarations, constants, etc

#define INITIAL_BUF_SIZE 128
#define INITIAL_QUERY_SIZE 4

static void Usage(void);
// static void ProcessQueries(DocTable *dt, MemIndex *mi);
// Better function than above, I think. This function will take the stdin and
// the docTable and memIndex, and the query list, and the query list size.
// then print out the result list with proper ranking, in a proper format.
static void ProcessQueries(FILE *f, DocTable *dt, MemIndex *idx,
                         char **query, int query_size);
// Read the user input into the return parameter (ret_str)
static int GetNextLine(FILE *f, char **ret_str);
// parser the user input str to a query list
static int GetQueries(char ***querys, char *words);

//////////////////////////////////////////////////////////////////////////////
// Main
int main(int argc, char **argv) {
  if (argc != 2) {
    Usage();
  }

  DocTable *dt;
  MemIndex *idx;

  // Crawl the directory that the user provides
  printf("Some happy elves are crawling the '%s' for you...\n", argv[1]);
  if (!CrawlFileTree(argv[1], &dt, &idx)) {
    fprintf(stderr, "Failed to crawl directory at %s\n", argv[1]);
    return EXIT_FAILURE;
  }


  // query loop
  while (true) {
    char *words = NULL;

    // prompt the user to query stuff
    printf("What word(s) are you looking for:\n");
    // Get the user's input
    if (!GetNextLine(stdin, &words)) {
      if (words != NULL) {
        // if failed to get the user's input , gotta free the word
        // cause in GetNextLine, it's on the heap
        free(words);
      }
      // this basically means the user hit the ctrl D
      if (errno == 0) {
        break;
      }
      perror("Failed read query from stdin\n");
      printf("These elves only speal English or Elvish...\n");
      DocTable_Free(dt);  // must free
      MemIndex_Free(idx);  // must free
      return EXIT_FAILURE;
    }

    // parese the query into a query[]
    char **query = NULL;
    int query_size = GetQueries(&query, words);

    if (query_size == -1) {
      free(words);  // don't forget this one
      printf("Failed to get queries.\n");
      return EXIT_FAILURE;
    }

    // finally, call the function to query the memIndex and print out
    // filepath and rank
    ProcessQueries(stdout, dt, idx, query, query_size);


    // free query list and the words inside of it
    for (int i = 0; i < query_size; i++) {
      free(query[i]);
    }
    free(query);
    free(words);  // oops, there is another words to free
  }

  printf("Some oppressed elves are cleaning up your memory byte by byte...\n");
  printf("---\n");
  DocTable_Free(dt);  // must free
  printf("--\n");
  MemIndex_Free(idx);  // must free
  printf("-\n");
  printf("Even though you have a lot of memory...\n");
  // Implement searchshell!  We're giving you very few hints
  // on how to do it, so you'll need to figure out an appropriate
  // decomposition into functions as well as implementing the
  // functions.  There are several major tasks you need to build:
  //
  //  - Crawl from a directory provided by argv[1] to produce and index
  //  - Prompt the user for a query and read the query from stdin, in a loop
  //  - Split a query into words (check out strtok_r)
  //  - Process a query against the index and print out the results
  //
  // When searchshell detects end-of-file on stdin (cntrl-D from the
  // keyboard), searchshell should free all dynamically allocated
  // memory and any other allocated resources and then exit.
  //
  // Note that you should make sure the fomatting of your
  // searchshell output exactly matches our solution binaries
  // to get full points on this part.
  return EXIT_SUCCESS;
}


//////////////////////////////////////////////////////////////////////////////
// Helper function definitions

static void Usage(void) {
  fprintf(stderr, "Usage: ./searchshell <docroot>\n");
  fprintf(stderr,
          "where <docroot> is an absolute or relative " \
          "path to a directory to build an index under.\n");
  exit(EXIT_FAILURE);
}

static void ProcessQueries(FILE *f, DocTable *dt, MemIndex *idx,
                         char **query, int query_size) {
  // query the memindex, assign it to the LinkedList
  LinkedList *ret_list = MemIndex_Search(idx, query, query_size);

  if (ret_list != NULL) {
    // iterate the LinkedList returned by the MI_Search and print the
    // file path and rank out
    LLIterator *it = LLIterator_Allocate(ret_list);
    while (LLIterator_IsValid(it)) {
      SearchResult *sr;
      LLIterator_Get(it, (LLPayload_t *) &sr);
      // get the file path to that doc_id using id_to_name HT
      char *file_path = DocTable_GetDocName(dt, sr->doc_id);
      // print things out in a correct format
      fprintf(f, "  %s (%d)\n", file_path, sr->rank);
      LLIterator_Next(it);
    }
    // huu, freed everything, what a luck
    LLIterator_Free(it);
    LinkedList_Free(ret_list, free);
  }
}

static int GetNextLine(FILE *f, char **ret_str) {
  size_t buf_size = INITIAL_BUF_SIZE;
  size_t len = 0;
  char *buffer = (char*) malloc(buf_size * sizeof(char));

  if (buffer == NULL) {
      perror("Memory allocation failed");
      return 0;
  }

    int c;
  while ((c = fgetc(f)) != EOF) {
    // If the buffer is full (excluding space for null terminator), expand it
    if (len >= buf_size - 1) {
      buf_size *= 2;  // Double the buffer size
      char *temp = realloc(buffer, buf_size);
      if (temp == NULL) {  // Check for memory allocation failure
        free(buffer);  // Free previously allocated buffer
        perror("Memory allocation failed");
        return 0;
      }
      buffer = temp;  // Update buffer pointer to newly allocated memory
    }

    // Convert character to lowercase and store in buffer
    buffer[len++] = (char) tolower(c);

    // Stop reading if a newline character is encountered
    if (c == '\n') {
      break;
    }
  }


  // Handle empty file case
  if (len == 0 && c == EOF) {
    free(buffer);
    return 0;
  }

  buffer[len] = '\0';

  // Remove trailing newline if present
  if (len > 0 && buffer[len - 1] == '\n') {
    buffer[len - 1] = '\0';
    len--;
  }

  *ret_str = buffer;
  return 1;
}


static int GetQueries(char ***queries, char *words) {
  int query_capacity = INITIAL_QUERY_SIZE;

  // Allocate memory for storing query words
  *queries = (char**) malloc(query_capacity * sizeof(char*));
  if (*queries == NULL) {
      perror("Memory allocation failed");
      return -1;
  }

  char *save_ptr;
  int count = 0;
  // Tokenize input string by spaces
  char *token = strtok_r(words, " ", &save_ptr);

  while (token != NULL) {
    // Expand queries array if it reaches capacity
    if (count >= query_capacity) {
      query_capacity *= 2;
      char **temp = (char**) realloc(*queries, query_capacity * sizeof(char*));
      if (temp == NULL) {  // Handle memory reallocation failure
        perror("Memory reallocation failed");

        // Free previously allocated strings before returning
        for (int i = 0; i < count; i++) {
          free((*queries)[i]);
        }
        free(*queries);
        return -1;
      }
      *queries = temp;
    }

    // Store the duplicated token in the queries array
    (*queries)[count++] = strdup(token);

    // Get the next token
    token = strtok_r(NULL, " ", &save_ptr);
  }

  return count;  // Return the number of queries extracted
}

