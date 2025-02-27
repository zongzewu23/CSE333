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

// Feature test macro enabling strdup (c.f., Linux Programming Interface p. 63)
#define _XOPEN_SOURCE 600

#include "./FileParser.h"

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <ctype.h>
#include <string.h>

#include "../HW1/CSE333.h"
#include "./MemIndex.h"


///////////////////////////////////////////////////////////////////////////////
// Constants and declarations of internal-only helpers
///////////////////////////////////////////////////////////////////////////////
#define ASCII_UPPER_BOUND 0x7F
#define READ_SIZE 4096

// Since our hash table dynamically grows, we'll start with a small
// number of buckets.
#define HASHTABLE_INITIAL_NUM_BUCKETS 2

// Frees a WordPositions.positions's payload, which is just a
// DocPositionOffset_t.
static void NoOpFree(LLPayload_t payload) { }

// Frees a WordPositions struct.
static void FreeWordPositions(HTValue_t payload) {
  WordPositions *pos = (WordPositions*) payload;
  LinkedList_Free(pos->positions, &NoOpFree);
  free(pos->word);
  free(pos);
}

// Add a normalized word and its byte offset into the WordPositions HashTable.
static void AddWordPosition(HashTable *tab, char *word,
                            DocPositionOffset_t pos);

// Parse the passed-in string into normalized words and insert into a HashTable
// of WordPositions structures.
static void InsertContent(HashTable *tab, char *content);


///////////////////////////////////////////////////////////////////////////////
// Publically-exported functions
///////////////////////////////////////////////////////////////////////////////
char* ReadFileToString(const char *file_name, int *size) {
  struct stat file_stat;
  char *buf;
  int fd;
  // int result; never being used
  ssize_t num_read;
  size_t left_to_read, offset = 0;

  // STEP 1.
  // Use the stat system call to fetch a "struct stat" that describes
  // properties of the file. ("man 2 stat"). You can assume we're on a 64-bit
  // system, with a 64-bit off_t field.

  if (stat(file_name, &file_stat) == -1) {
    return NULL;
  }

  // STEP 2.
  // Make sure this is a "regular file" and not a directory or something else.
  // We suggest using the S_ISREG macro described in "man 7 inode", but you
  // could also check the bitmask as described in "man 2 stat".

  if (!S_ISREG(file_stat.st_mode)) {
    return NULL;
  }

  // STEP 3.
  // Attempt to open the file for reading (see also "man 2 open").
  fd = open(file_name, O_RDONLY);
  if (fd == -1) {
    return NULL;
  }

  // STEP 4.
  // Allocate space for the file, plus 1 extra byte to
  // '\0'-terminate the string.

  buf = (char*) malloc(sizeof(char)*file_stat.st_size + 1);
  if (buf == NULL) {
    return NULL;
  }

  // STEP 5.
  // Read in the file contents using the read() system call (see also
  // "man 2 read"), being sure to handle the case that read() returns -1 and
  // errno is either EAGAIN or EINTR.  Also, note that it is not an error for
  // read to return fewer bytes than what you asked for; you'll need to put
  // read() inside a while loop, looping until you've read to the end of file
  // or a non-recoverable error.  Read the man page for read() carefully, in
  // particular what the return values -1 and 0 imply.

  // Initialize the remaining bytes to read as the total file size
  left_to_read = file_stat.st_size;
  while (left_to_read > 0) {
    // Read up to READ_SIZE bytes or the remaining bytes, whichever is smaller
    num_read = read(fd, buf + offset,
                    left_to_read < READ_SIZE? left_to_read : READ_SIZE);
    // failed to read
    if (num_read == -1) {
      // If the error is not an interrupt (EINTR)
      // or resource temporarily unavailable (EAGAIN), handle the failure
      if (errno != EINTR && errno != EAGAIN) {
        close(fd);
        free(buf);
        perror("failed on read");
        return NULL;
      }
      continue;  // read again
    }
    // EOF
    if (num_read == 0) {
      break;
    }
    left_to_read -= num_read;
    offset += num_read;
  }

  // Great, we're done!  We hit the end of the file and we read
  // filestat.st_size - left_to_read bytes. Close the file descriptor returned
  // by open() and return through the "size" output parameter how many bytes
  // we read.
  close(fd);
  *size = file_stat.st_size - left_to_read;

  // Null-terminate the string.
  buf[*size] = '\0';
  return buf;
}

HashTable* ParseIntoWordPositionsTable(char *file_contents) {
  HashTable *tab;
  int i, file_len;

  if (file_contents == NULL) {
    return NULL;
  }

  file_len = strlen(file_contents);
  if (file_len == 0) {
    return NULL;
  }

  // Verify that the file contains only ASCII text.  We won't try to index any
  // files that contain non-ASCII text; unfortunately, this means we aren't
  // Unicode friendly.
  for (i = 0; i < file_len; i++) {
    if (file_contents[i] == '\0' ||
        (unsigned char) file_contents[i] > ASCII_UPPER_BOUND) {
      free(file_contents);
      return NULL;
    }
  }

  // Great!  Let's split the file up into words.  We'll allocate the hash
  // table that will store the WordPositions structures associated with
  // each word.
  tab = HashTable_Allocate(HASHTABLE_INITIAL_NUM_BUCKETS);
  Verify333(tab != NULL);

  // Loop through the file, splitting it into words and inserting a record for
  // each word.
  InsertContent(tab, file_contents);

  // If we found no words, return NULL instead of a zero-sized hashtable.
  if (HashTable_NumElements(tab) == 0) {
    HashTable_Free(tab, &FreeWordPositions);
    tab = NULL;
  }

  // Now that we've finished parsing the document, we can free up the
  // filecontents buffer and return our built-up table.
  free(file_contents);
  return tab;
}

void FreeWordPositionsTable(HashTable *table) {
  HashTable_Free(table, &FreeWordPositions);
}


///////////////////////////////////////////////////////////////////////////////
// Internal helper functions

static void InsertContent(HashTable *tab, char *content) {
  char *cur_ptr = content,
    *word_start = content;

  // STEP 6.
  // This is the interesting part of Part A!
  //
  // "content" contains a C string with the full contents of the file.  You
  // need to implement a loop that steps through the file content one character
  // at a time, testing to see whether a character is an alphabetic character.
  // If a character is alphabetic, it's part of a word.  If a character is not
  // alphabetic, it's part of the boundary between words.  You can use
  // string.h's "isalpha()" macro to test whether a character is alphabetic or
  // not (see also "man isalpha").
  //
  // For example, here's a string with its words underlined with "=" and
  // boundary characters underlined with "+":
  //
  // The  Fox  Can't   CATCH the  Chicken.
  // ===++===++===+=+++=====+===++=======+
  //
  // Any time you detect the start of a word, you should use the "wordstart"
  // pointer to remember where the word started.  You should also use the
  // "tolower()" macro to convert alphabetic characters to lowercase (ie,
  // *curptr = tolower(*curptr)).  Finally, as a hint, you can overwrite
  // boundary characters with '\0' (null terminators) in the buffer to create
  // valid C strings out of each parsed word.
  //
  // Each time you find a word that you want to record in the hashtable, call
  // AddWordPosition() helper with appropriate arguments, e.g.,
  //     AddWordPosition(tab, wordstart, pos);
  int file_len = strlen(content);
  char *file_end = content + file_len;
  // keep looping until hit the file's end
  while (cur_ptr < file_end) {
    // if the current char is an alpha char
    if (isalpha((unsigned char) *cur_ptr)) {
      // to lower case anyway
      *cur_ptr = tolower(*cur_ptr);
      // nect char to be a non-alpha, so it's a complete string
      if (!isalpha((unsigned char) *(cur_ptr + 1))) {
        *(cur_ptr + 1) = '\0';  // implementation as hint
        // add this word and it's position to the HT
        AddWordPosition(tab, word_start,
                    (DocPositionOffset_t) (word_start - content));
      }
      // go to next char
      cur_ptr++;
    } else {
      // if the current char is non-alphabatic char, then word_start should
      // go to the next char, prepareing to point to the alphabatic char
      // but if the next char is still not alphabatic, then it will go to the
      // next char until it's an alphabatic char or reach the end of the file.
      word_start = cur_ptr + 1;
      cur_ptr++;
    }
  }  // end while-loop
}

static void AddWordPosition(HashTable *tab, char *word,
                            DocPositionOffset_t pos) {
  HTKey_t hash_key;
  HTKeyValue_t kv;
  WordPositions *wp;

  // Hash the string.
  hash_key = FNVHash64((unsigned char*) word, strlen(word));

  // Have we already encountered this word within this file?  If so, it's
  // already in the hashtable.
  if (HashTable_Find(tab, hash_key, &kv)) {
    // Yes; we just need to add a position in using LinkedList_Append(). Note
    // how we're casting the DocPositionOffset_t position variable to an
    // LLPayload_t to store it in the linked list payload without needing to
    // malloc space for it.  Ugly, but it works!
    wp = (WordPositions*) kv.value;

    // Ensure we don't have hash collisions (two different words that hash to
    // the same key, which is very unlikely).
    Verify333(strcmp(wp->word, word) == 0);

    LinkedList_Append(wp->positions, (LLPayload_t) (int64_t) pos);
  } else {
    // STEP 7.
    // No; this is the first time we've seen this word.  Allocate and prepare
    // a new WordPositions structure, and append the new position to its list
    // using a similar ugly hack as right above.
    HTKeyValue_t old_kv;  // Passed in parameter, no usage, auto destroyed
    // Passed in word is not on the heap, therefore create one on the heap
    char *insert_word = (char*) malloc(strlen(word) + 1);
    Verify333(insert_word != NULL);
    // copy the passed in word to this new word on the heap
    strncpy(insert_word, word, strlen(word) + 1);

    // create a new linkedList
    LinkedList *list = LinkedList_Allocate();
    Verify333(list != NULL);

    // append this pos, so it's ascending order
    LinkedList_Append(list, (LLPayload_t) (int64_t) pos);

    // prepare the wordPosition structure
    wp = (WordPositions*) malloc(sizeof(WordPositions));
    wp->word = insert_word;
    wp->positions =  list;

    // prepare the keyvalue structure
    kv.key = hash_key;
    kv.value = (HTValue_t) wp;

    // insert kv
    HashTable_Insert(tab, kv, &old_kv);
    // do not need to free old_kv, because old_kv is a local variable and
    // HT_Insert will leave old_kv as before since there is no such a key.
  }
}
