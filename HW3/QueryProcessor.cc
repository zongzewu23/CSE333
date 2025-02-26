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

#include "./QueryProcessor.h"

#include <iostream>
#include <algorithm>
#include <list>
#include <string>
#include <vector>

extern "C" {
  #include "./libhw1/CSE333.h"
}

using std::list;
using std::sort;
using std::string;
using std::vector;

namespace hw3 {

// A function return a list of DIEHeader, this list is an intersection between
// dih_list and next_dih_list, that is saying, if the element(s) in
// diHeader_list obtained so far are also contained in the
// next(new query)_diheader_list, then those elements are considered to be the
// overlap between two list. So we will keep this element and update its
// num_positions(as rank later on)
static list<DocIDElementHeader> QueryOverlapping(const list<DocIDElementHeader>
  & dih_list, const list<DocIDElementHeader>& next_dih_list);

// This function simply read through the diHeader_list which contains the
// final headers created by the current .idx file, to make a list of QueryResult
// that contains every document_names(filename) and it's rank where the
// queried word(s) appears in any .idx file.
// The final_result is returned by return parameter.
static void UpdateResult(vector<hw3::QueryProcessor::QueryResult>*
  final_result, DocTableReader* dtr, const list<DocIDElementHeader>& dih_list);

QueryProcessor::QueryProcessor(const list<string> &index_list, bool validate) {
  // Stash away a copy of the index list.
  index_list_ = index_list;
  array_len_ = index_list_.size();
  Verify333(array_len_ > 0);

  // Create the arrays of DocTableReader*'s. and IndexTableReader*'s.
  dtr_array_ = new DocTableReader*[array_len_];
  itr_array_ = new IndexTableReader*[array_len_];

  // Populate the arrays with heap-allocated DocTableReader and
  // IndexTableReader object instances.
  list<string>::const_iterator idx_iterator = index_list_.begin();
  for (int i = 0; i < array_len_; i++) {
    FileIndexReader fir(*idx_iterator, validate);
    dtr_array_[i] = fir.NewDocTableReader();
    itr_array_[i] = fir.NewIndexTableReader();
    idx_iterator++;
  }
}

QueryProcessor::~QueryProcessor() {
  // Delete the heap-allocated DocTableReader and IndexTableReader
  // object instances.
  Verify333(dtr_array_ != nullptr);
  Verify333(itr_array_ != nullptr);
  for (int i = 0; i < array_len_; i++) {
    delete dtr_array_[i];
    delete itr_array_[i];
  }

  // Delete the arrays of DocTableReader*'s and IndexTableReader*'s.
  delete[] dtr_array_;
  delete[] itr_array_;
  dtr_array_ = nullptr;
  itr_array_ = nullptr;
}

// This structure is used to store a index-file-specific query result.
typedef struct {
  DocID_t doc_id;  // The document ID within the index file.
  int rank;        // The rank of the result so far.
} IdxQueryResult;

vector<QueryProcessor::QueryResult>
QueryProcessor::ProcessQuery(const vector<string> &query) const {
  Verify333(query.size() > 0);

  // STEP 1.
  // (the only step in this file)
  vector<QueryProcessor::QueryResult> final_result;
  DocTableReader* dtr;  // for reading the DocTable
  IndexTableReader* itr;  //  for reading the IndexTable
  DocIDTableReader* ditr;  // for reading the DocIDTbale
  // A list contain all the DocIDHeaders queried from the current .idx file
  list<DocIDElementHeader> dih_list;
  bool found_next;  // decide when to stop looping

  // query words in query one by one, stop when looped through the query
  // or the queried words doesn't appear in any file
  for (int i = 0; i < array_len_; i++) {
    dtr = dtr_array_[i];  // get the DocTableReader for this .idx file
    itr = itr_array_[i];  // get the IndexTableReader for this .idx file

    // query the first word
    ditr = itr->LookupWord(query[0]);
    // found at least one file that contains the word
    if (ditr != nullptr) {
      found_next = true;

      // obtain all of the docId that its doc contains the word
      dih_list = ditr->GetDocIDList();
      delete ditr;  // delete this ditr to prevent memory leak
      // query the next word
      for (size_t j = 1; j < query.size(); j++) {
        ditr = itr->LookupWord(query[j]);

        if (ditr != nullptr) {
          // this next word also exists in some file(s), obtain the DocIDList
          list<DocIDElementHeader> next_dih_list = ditr->GetDocIDList();
          // get the DocIdHeader that contains all of the queried words so far
          dih_list = QueryOverlapping(dih_list, next_dih_list);
          delete ditr;  // delete to prevent memory leak
        } else {
          // this current queried word is not in any file, there is no need to
          // query the next word in the current .idx file
          found_next = false;
          break;
        }
      }
      if (found_next) {
        UpdateResult(&final_result, dtr, dih_list);
      }
    }
  }

  // Sort the final results.
  sort(final_result.begin(), final_result.end());
  return final_result;
}

static list<DocIDElementHeader> QueryOverlapping(const list<DocIDElementHeader>
  & dih_list, const list<DocIDElementHeader>& next_dih_list) {
    list<DocIDElementHeader> overlap;  // the list to return

    // manully read through two list, get the overlapped header
    // and push them to return list
    for (DocIDElementHeader header : dih_list) {
      for (DocIDElementHeader next_header : next_dih_list) {
        if (header.doc_id == next_header.doc_id) {
          // update the num_positions(rank)
          int32_t sum_pos = header.num_positions + next_header.num_positions;
          overlap.push_back(DocIDElementHeader(header.doc_id, sum_pos));
        }
      }
    }
    return overlap;
}

static void UpdateResult(vector<hw3::QueryProcessor::QueryResult>*
  final_result, DocTableReader* dtr, const list<DocIDElementHeader>& dih_list) {
    // for each header, get their num_positions as the rank, get their docID
    // to look up for filename. Then push it to the final_result list.
    for (DocIDElementHeader header : dih_list) {
      QueryProcessor::QueryResult qr;
      qr.rank = header.num_positions;
      dtr->LookupDocID(header.doc_id, &qr.document_name);
      final_result->push_back(qr);
    }
}

}  // namespace hw3
