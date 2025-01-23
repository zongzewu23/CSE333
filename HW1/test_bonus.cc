// Copyright (c) 2025 Z. Wu
// name: Zongze Wu
// UW email: zongzewu@uw.edu

#include <set>
#include <string>


#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>

#include "gtest/gtest.h"

extern "C" {
  #include "./HashTable.h"
  #include "./HashTable_priv.h"
  #include "./LinkedList.h"
  #include "./LinkedList_priv.h"
}
#include "./test_suite.h"

using std::set;
using std::string;

namespace hw1 {

static const int kMagicNum = 0xDEADBEEF;

typedef struct payload_st {
  int magic_num;
  int payload;
} TestPayload;



class Test_HashTable : public ::testing::Test {
 protected:
  // Code here will be called before each test executes (ie, before
  // each TEST_F).
  virtual void SetUp() {
    freeInvocations_ = 0;
  }

  // A version of free() that verifies the payload before freeing it.
  static void VerifiedFree(HTValue_t payload) {
    ASSERT_EQ(kMagicNum, (static_cast<TestPayload *>(payload))->magic_num);
    free(payload);
  }

  // A version of VerifiedFree() which counts how many times it's been
  // invoked; this allows us to make assertions.  Note that the counter
  // is reset in SetUp().
  static int freeInvocations_;
  static void InstrumentedVerifiedFree(HTValue_t payload) {
    freeInvocations_++;
    VerifiedFree(payload);
  }
};  // class Test_HashTable



static void TestFNVHash64Helper(const char *input, int len,
                                uint64_t expected_hash) {
    uint64_t hash = FNVHash64((unsigned char *)input, len);
    ASSERT_EQ(hash, expected_hash) << "FNVHash64 failed for input: " << input;
}

TEST_F(Test_HashTable, FNVHash64_Basic) {
    HW1Environment::OpenTestCase();

    ASSERT_EQ(FNVHash64((unsigned char *)"", 0), 0xcbf29ce484222325ULL);

    TestFNVHash64Helper("hello", 5, 11831194018420276491ULL);
    TestFNVHash64Helper("world", 5, 5717881983045765875ULL);
    TestFNVHash64Helper("hello, world!", 13, 16577297119107508372ULL);
    TestFNVHash64Helper("", 0, 14695981039346656037ULL);
    TestFNVHash64Helper("\n\t\r", 3, 3444478664954417987ULL);
    TestFNVHash64Helper("\0\0\0", 3, 15658191375538532279ULL);
  }
}  // namespace hw1
