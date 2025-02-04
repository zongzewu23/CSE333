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

#include "./CrawlFileTree.h"

#include <dirent.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include "libhw1/CSE333.h"
#include "./DocTable.h"
#include "./FileParser.h"

//////////////////////////////////////////////////////////////////////////////
// Internal helper functions and constants
//////////////////////////////////////////////////////////////////////////////
#define MAX_PATHNAME_LENGTH 1024  // max len of a directory item's path + name

struct entry_st {
  char *path_name;
  bool is_dir;
};

// Return the relative ordering of two strings, according to the signature
// required by "man 3 qsort".
int alphasort(const void *v1, const void *v2) {
  struct entry_st *e1 = (struct entry_st*) v1,
    *e2 = (struct entry_st*) v2;
  return strncmp(e1->path_name, e2->path_name, MAX_PATHNAME_LENGTH);
}

// Recursively descend into the passed-in directory, looking for files and
// subdirectories.  Any encountered files are processed via HandleFile(); any
// subdirectories are recursively handled by HandleDir().
//
// Note that opendir()/readdir() iterates through a directory's entries in an
// unspecified order; since we need the ordering to be consistent in order
// to generate consistent DocTables and MemIndices, we do two passes over the
// contents: the first to extract the data necessary for populating
// entry_name_st and the second to actually handle the recursive call.
static void HandleDir(char *dirpath, DIR *d,
                      DocTable **doctable, MemIndex **index);

// Read and parse the specified file, then inject it into the MemIndex.
static void HandleFile(char *file_path, DocTable **doctable, MemIndex **index);


//////////////////////////////////////////////////////////////////////////////
// Externally-exported functions
//////////////////////////////////////////////////////////////////////////////

bool CrawlFileTree(char *rootdir, DocTable **doctable, MemIndex **index) {
  struct stat root_stat;
  DIR *rd;

  // Verify we got some valid args.
  if (rootdir == NULL || doctable == NULL || index == NULL) {
    return false;
  }

  // Verify that rootdir is a directory.
  if (stat((char*) rootdir, &root_stat) == -1) {
    // We got some kind of error stat'ing the file. Give up
    // and return an error.
    return false;
  }
  if (!S_ISDIR(root_stat.st_mode)) {
    // It isn't a directory, so give up.
    return false;
  }

  // Try to open the directory using opendir().  If we fail, (e.g., we don't
  // have permissions on the directory), return a failure. ("man 3 opendir")
  rd = opendir(rootdir);
  if (rd == NULL) {
    return false;
  }

  // Since we're able to open the directory, allocate our objects.
  *doctable = DocTable_Allocate();
  Verify333(*doctable != NULL);
  *index = MemIndex_Allocate();
  Verify333(*index != NULL);

  // Begin the recursive handling of the directory.
  HandleDir(rootdir, rd, doctable, index);

  // All done.  Release and/or transfer ownership of resources.
  Verify333(closedir(rd) == 0);
  return true;
}


//////////////////////////////////////////////////////////////////////////////
// Internal helper functions
//////////////////////////////////////////////////////////////////////////////

static void HandleDir(char *dirpath, DIR *d, DocTable **doctable,
                      MemIndex **index) {
  // We make two passes through the directory.  The first gets the list of
  // all the metadata necessary to process its entries; the second iterates
  // does the actual recursive descent.
  int entries_capacity = 16;
  struct entry_st *entries = (struct entry_st*)
      malloc(sizeof(struct entry_st) * entries_capacity);
  Verify333(entries != NULL);

  int i;
  int path_name_len;
  struct dirent *dirent;
  struct stat st;

  int num_entries;

  // First pass, to populate the "entries" list of item metadata.
  //
  // STEP 1.
  // Change/add to this loop to use the "readdir()" system call to
  // read the directory entries in the loop ("man 3 readdir").
  // Exit out of the loop when we reach the end of the directory.
  num_entries = 0;
  for (;;) {
    // STEP 2.
    // If the directory entry is named "." or "..", ignore it.  Use the C
    // "continue" expression to begin the next iteration of the loop.  What
    // field in the dirent could we use to find out the name of the entry?
    // How do you compare strings in C?

    // Open dir
    dirent = readdir(d);
    if (dirent == NULL)
      break;
    // if it's a root dir then skip this
    if (strcmp(dirent->d_name, ".") == 0
            || strcmp(dirent->d_name, "..") == 0 ) {
      continue;
    }

    //
    // Record the name and directory status.
    //

    // Resize the entries array if it's too small.
    if (num_entries == entries_capacity) {
      entries_capacity *= 2;
      entries = (struct entry_st*)
        realloc(entries, sizeof(struct entry_st) * entries_capacity);
      Verify333(entries != NULL);
    }

    // We need to append the name of the file to the name of the directory
    // we're in to get the full filename. So, we'll malloc space for:
    //     dirpath + "/" + dirent->d_name + '\0'
    path_name_len = strlen(dirpath) + 1 + strlen(dirent->d_name) + 1;
    entries[num_entries].path_name =
                  (char*) malloc(path_name_len * sizeof(char));
    Verify333(entries[num_entries].path_name != NULL);
    if (dirpath[strlen(dirpath)-1] == '/') {
      // No need to add an additional '/'.
      snprintf(entries[num_entries].path_name, path_name_len,
               "%s%s", dirpath, dirent->d_name);
    } else {
      // We do need to add an additional '/'.
      snprintf(entries[num_entries].path_name, path_name_len,
               "%s/%s", dirpath, dirent->d_name);
    }

    // Use the "stat()" system call to ask the operating system to give us
    // information about the file identified by the directory entry (see
    // also "man 2 stat").
    if (stat(entries[num_entries].path_name, &st) == 0) {
      // STEP 3.
      // Test to see if the file is a "regular file" (you can use the S_ISREG()
      // macro described in "man 7 inode" or you can check the bitmask as
      // described in "man 2 stat").  If so, we'll process the file by
      // eventually invoking the HandleFile() private helper function in our
      // second pass.
      //
      // On the other hand, if the file turns out to be a directory (which you
      // can find out using the S_ISDIR() macro. Again, look it up if you don't
      // know how to use it), then we'll need to recursively process i
      // using/ HandleDir() in our second pass.
      //
      // If it is neither, skip the file.

      // if this is a regular file set the is_dir flag to be false
      if (S_ISREG(st.st_mode)) {
        entries[num_entries].is_dir = false;
      } else if (S_ISDIR(st.st_mode)) {
        // otherwise set it true
        entries[num_entries].is_dir = true;
      }
    }
    // update the number of the dir and file here
    num_entries++;
  }  // end iteration over directory contents ("first pass").

  // Sort the directory's metadata alphabetically.
  qsort(entries, num_entries, sizeof(struct entry_st), &alphasort);

  // Second pass, processing the now-sorted directory metadata.
  for (i = 0; i < num_entries; i++) {
    if (!entries[i].is_dir) {
      HandleFile(entries[i].path_name, doctable, index);
    } else {
      DIR *sub_dir = opendir(entries[i].path_name);
      if (sub_dir != NULL) {
        HandleDir(entries[i].path_name, sub_dir, doctable, index);
        closedir(sub_dir);
      }
    }

    // Free the memory we'd allocated for the entries.
    free(entries[i].path_name);
  }
  free(entries);
}

static void HandleFile(char *file_path, DocTable **doctable,
                       MemIndex **index) {
  int file_len = 0;
  HashTable *tab = NULL;
  DocID_t doc_id;
  HTIterator *it;

  // STEP 4.
  // Invoke ParseIntoWordPositionsTable() to build the word hashtable out
  // of the file.

  tab = ParseIntoWordPositionsTable(ReadFileToString(file_path, &file_len));
  if (!tab) {
    return;
  }

  // STEP 5.
  // Invoke DocTable_Add() to register the new file with the doctable.

  doc_id = DocTable_Add(*doctable, file_path);

  // Loop through the newly-built hash table.
  it = HTIterator_Allocate(tab);
  Verify333(it != NULL);
  while (HTIterator_IsValid(it)) {
    WordPositions *wp;
    HTKeyValue_t kv;

    // STEP 6.
    // Use HTIterator_Remove() to extract the next WordPositions structure out
    // of the hashtable. Then, use MemIndex_AddPostingList() to add the word,
    // document ID, and positions linked list into the inverted index.

    // as descriptions above
    HTIterator_Remove(it, &kv);
    wp = (WordPositions*) kv.value;
    MemIndex_AddPostingList(*index, wp->word, doc_id, wp->positions);

    // Since we've transferred ownership of the memory associated with both
    // the "word" and "positions" field of this WordPositions structure, and
    // since we've removed it from the table, we can now free the
    // WordPositions structure!
    free(wp);
  }
  HTIterator_Free(it);

  // We're all done with the word hashtable for this file, since we've added
  // all of its contents to the inverted index. Free the table and return.
  FreeWordPositionsTable(tab);
}
