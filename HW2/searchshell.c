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
static void ProcessQueries(FILE *f, DocTable *dt, MemIndex *idx,
                         char **query, int query_size);
static int GetNextLine(FILE *f, char **ret_str);
static int GetQueries(char ***querys, char *words);

//////////////////////////////////////////////////////////////////////////////
// Main
int main(int argc, char **argv) {
  if (argc != 2) {
    Usage();
  }

  DocTable *dt;
  MemIndex *idx;

  printf("Some happy elves are crawling the files for you...\n");
  if (!CrawlFileTree(argv[1], &dt, &idx)) {
    fprintf(stderr, "Failed to crawl directory at %s\n", argv[1]);
    return EXIT_FAILURE;
  }


  // query loop
  while (true) {

    char *words = NULL;

    printf("What word(s) are you looking for:\n");
    if (!GetNextLine(stdin, &words)) {
      if (words != NULL) {
        free(words);
      }
      if (errno == 0) {
        break;
      }
      perror("Failed read query from stdin\n");
      printf("These elves only speal English or Elvish...\n");
      DocTable_Free(dt);
      MemIndex_Free(idx);
      return EXIT_FAILURE;
    }

    char **query = NULL;
    int query_size = GetQueries(&query, words);

    if (query_size == -1) {
      free(words);
      printf("Failed to get queries.\n");
      return EXIT_FAILURE;
    }

    ProcessQueries(stdout, dt, idx, query, query_size);


    for (int i = 0; i < query_size; i++) {
      free(query[i]);
    }
    free(query);
    free(words);
  }

  printf("Some oppressed elves are cleaning up memory...\n");
  DocTable_Free(dt);
  MemIndex_Free(idx);
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
  LinkedList *ret_list = MemIndex_Search(idx, query, query_size);
  


  if (ret_list != NULL) {
    LLIterator *it = LLIterator_Allocate(ret_list);
    while (LLIterator_IsValid(it)) {
      SearchResult *sr;
      LLIterator_Get(it, (LLPayload_t *) &sr);

      char *file_path = DocTable_GetDocName(dt, sr->doc_id);
      fprintf(f, "  %s (%d)\n", file_path, sr->rank);
      LLIterator_Next(it);
    }
    LLIterator_Free(it);
    LinkedList_Free(ret_list, free);
  }
}

static int GetNextLine(FILE *f, char **ret_str) {
  size_t buf_size = INITIAL_BUF_SIZE;
  size_t len = 0;
  char *buffer = (char*) malloc(buf_size*sizeof(char));

  if (buffer == NULL) {
      perror("Memory allocation failed");
      return 0;
  }
  int c;
  while ((c = fgetc(f)) != EOF) {
    if (len >= buf_size - 1) {
      buf_size *= 2;
      char *temp = realloc(buffer, buf_size);
      if (temp == NULL) {
        free(buffer);
        perror("Memory allocation failed");
        return 0;
      }
      buffer = temp;
    }
    buffer[len++] = (char) tolower(c);

    if (c == '\n') {
      break;
    }
  }

  if (len == 0 && c == EOF) {
    free(buffer);
    return 0;
  }

  buffer[len] = '\0';

  if (len > 0 && buffer[len - 1] == '\n') {
    buffer[len - 1] = '\0';
    len--;
  }

  *ret_str = buffer;

  return 1;  // you may need to change this return value
}

static int GetQueries(char ***queries, char *words) {
  int query_capacity = INITIAL_QUERY_SIZE;
  *queries = (char**) malloc(query_capacity*sizeof(char*));

  if (*queries == NULL) {
      perror("Memory allocation failed");
       return -1;
  }

  char *save_ptr;
  int count = 0;
  char *token = strtok_r(words, " ", &save_ptr);

  while (token != NULL) {
    if (count >= query_capacity) {
      query_capacity *= 2;
      char **temp = (char**) realloc(*queries, query_capacity*sizeof(char*));
      if (temp == NULL) {
        perror("Memory reallocation failed");
        for (int i = 0; i < count; i++) {
          free((*queries)[i]);
        }
        free(*queries);
        return -1;
      }
      *queries = temp;
    }
    (*queries)[count++] = strdup(token);
    token = strtok_r(NULL, " ", &save_ptr);
  }

  return count;
}
