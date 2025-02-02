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

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include "CSE333.h"
#include "HashTable.h"
#include "LinkedList.h"
#include "HashTable_priv.h"

///////////////////////////////////////////////////////////////////////////////
// Internal helper functions.
//
#define INVALID_IDX -1

// Grows the hashtable (ie, increase the number of buckets) if its load
// factor has become too high.
static void MaybeResize(HashTable *ht);

int HashKeyToBucketNum(HashTable *ht, HTKey_t key) {
  return key % ht->num_buckets;
}

// Deallocation functions that do nothing.  Useful if we want to deallocate
// the structure (eg, the linked list) without deallocating its elements or
// if we know that the structure is empty.
static void LLNoOpFree(LLPayload_t freeme) { }
static void HTNoOpFree(HTValue_t freeme) { }

// !!!!!!!!!!!!!!!!!!!!!!!!!DIY static functions!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

// Find the target in chain(LinkedList) by key(HTkey_t)
// Pass in a pointer to a iterator pointer so we can use this parameter as
// a return parameter, which tells us where the iterator found the target,
// so we can access the node and payload by using LLIterator_* functions.
// Return false if not found and set the return parameter to be NULL.
static bool FindInChain(LinkedList *chain, HTKey_t target_key,
                        LLIterator **return_iter);

// This function set the key and value of the parameter pointed to by 'return
// parameter' to the key and value that the iter's payload's HTKeyValue_t's
// key and value. (Deep Breath)
static void SetReturnKV(HTKeyValue_t *keyvalue, LLIterator *iter);

// Similar to the function above but it's not setting the return parameter
// It's setting the payload's HTKeyValue in the LinkedList node to the
// key and value that keyvalue pointing to.
static void SetKeyValue(LLIterator *iter, HTKeyValue_t *keyvalue);

///////////////////////////////////////////////////////////////////////////////
// HashTable implementation.

HTKey_t FNVHash64(unsigned char *buffer, int len) {
  // This code is adapted from code by Landon Curt Noll
  // and Bonelli Nicola:
  //     http://code.google.com/p/nicola-bonelli-repo/
  static const uint64_t FNV1_64_INIT = 0xcbf29ce484222325ULL;
  static const uint64_t FNV_64_PRIME = 0x100000001b3ULL;
  unsigned char *bp = (unsigned char *) buffer;
  unsigned char *be = bp + len;
  uint64_t hval = FNV1_64_INIT;

  // FNV-1a hash each octet of the buffer.
  while (bp < be) {
    // XOR the bottom with the current octet.
    hval ^= (uint64_t) * bp++;
    // Multiply by the 64 bit FNV magic prime mod 2^64.
    hval *= FNV_64_PRIME;
  }
  return hval;
}

HashTable* HashTable_Allocate(int num_buckets) {
  HashTable *ht;
  int i;

  Verify333(num_buckets > 0);

  // Allocate the hash table record.
  ht = (HashTable *) malloc(sizeof(HashTable));
  Verify333(ht != NULL);

  // Initialize the record.
  ht->num_buckets = num_buckets;
  ht->num_elements = 0;
  ht->buckets = (LinkedList **) malloc(num_buckets * sizeof(LinkedList *));
  Verify333(ht->buckets != NULL);
  for (i = 0; i < num_buckets; i++) {
    ht->buckets[i] = LinkedList_Allocate();
  }

  return ht;
}

void HashTable_Free(HashTable *table,
                    ValueFreeFnPtr value_free_function) {
  int i;

  Verify333(table != NULL);

  // Free each bucket's chain.
  for (i = 0; i < table->num_buckets; i++) {
    LinkedList *bucket = table->buckets[i];
    HTKeyValue_t *kv;

    // Pop elements off the chain list one at a time.  We can't do a single
    // call to LinkedList_Free since we need to use the passed-in
    // value_free_function -- which takes a HTValue_t, not an LLPayload_t -- to
    // free the caller's memory.
    while (LinkedList_NumElements(bucket) > 0) {
      Verify333(LinkedList_Pop(bucket, (LLPayload_t *)&kv));
      value_free_function(kv->value);
      free(kv);
    }
    // The chain is empty, so we can pass in the
    // null free function to LinkedList_Free.
    LinkedList_Free(bucket, LLNoOpFree);
  }

  // Free the bucket array within the table, then free the table record itself.
  free(table->buckets);
  free(table);
}

int HashTable_NumElements(HashTable *table) {
  Verify333(table != NULL);
  return table->num_elements;
}

bool HashTable_Insert(HashTable *table,
                      HTKeyValue_t newkeyvalue,
                      HTKeyValue_t *oldkeyvalue) {
  int bucket;
  LinkedList *chain;

  Verify333(table != NULL);
  MaybeResize(table);

  // Calculate which bucket and chain we're inserting into.
  bucket = HashKeyToBucketNum(table, newkeyvalue.key);
  chain = table->buckets[bucket];

  // STEP 1: finish the implementation of InsertHashTable.
  // This is a fairly complex task, so you might decide you want
  // to define/implement a helper function that helps you find
  // and optionally remove a key within a chain, rather than putting
  // all that logic inside here.  You might also find that your helper
  // can be reused in steps 2 and 3.

  // This pointer will be passed in to the FindInChain(), if FIC() returns true
  // then it have to be freed after use, otherwise memory leak will happen.
  // But if FIC() returns false, that means iter is just a local variable
  // points to NULL, no need to free.
  LLIterator *iter;
  HTKey_t target_key = newkeyvalue.key;  // For readability

  // This have to be freed after usage
  HTKeyValue_t *kv_copy = (HTKeyValue_t*) malloc(sizeof(HTKeyValue_t));
  Verify333(kv_copy != NULL);
  // Assign key and value to the newly allocated pointer's struct
  // The reason for this is newkeyvalue seems like a local variable
  // in the caller.
  kv_copy->key = newkeyvalue.key;
  kv_copy->value = newkeyvalue.value;

  if (FindInChain(chain, target_key, &iter)) {
    SetReturnKV(oldkeyvalue, iter);  // Set up return parameter
    // Set the key and value pointed by payload to the new key and value
    SetKeyValue(iter, kv_copy);

    // The client don't have the ownership for this kv_copy, because it's
    // never pushed on the list, so free it here
    free(kv_copy);
    LLIterator_Free(iter);  // Free the iter since the FindInChain returns true

    return true;
  } else {
    // Simply push the kv_copy to the chain and the client has it's ownership
    LinkedList_Push(chain, kv_copy);
    table->num_elements++;
    return false;
  }
}

bool HashTable_Find(HashTable *table,
                    HTKey_t key,
                    HTKeyValue_t *keyvalue) {
  Verify333(table != NULL);

  // STEP 2: implement HashTable_Find.
  int bucket = HashKeyToBucketNum(table, key);
  LinkedList *chain = table->buckets[bucket];
  LLIterator *iter;  // Same as in the HashTable_Insert

  if (FindInChain(chain, key, &iter)) {
    SetReturnKV(keyvalue, iter);
    LLIterator_Free(iter);
    return true;
  }
  return false;  // you may need to change this return value
}

bool HashTable_Remove(HashTable *table,
                      HTKey_t key,
                      HTKeyValue_t *keyvalue) {
  Verify333(table != NULL);

  // STEP 3: implement HashTable_Remove.
  int bucket = HashKeyToBucketNum(table, key);
  LinkedList *chain = table->buckets[bucket];
  LLIterator *iter;

  if (FindInChain(chain, key, &iter)) {
    // Similar to what SetReturnKV does, except we free the payload here
    LLPayload_t payload;

    LLIterator_Get(iter, &payload);

    keyvalue->key = ((HTKeyValue_t*)payload)->key;
    keyvalue->value = ((HTKeyValue_t*) payload)->value;

    free(payload);

    // Now we can use LLNoOpFree to free this node
    LLIterator_Remove(iter, &LLNoOpFree);
    LLIterator_Free(iter);
    table->num_elements--;

    return true;
  }

  return false;
}


///////////////////////////////////////////////////////////////////////////////
// HTIterator implementation.

HTIterator* HTIterator_Allocate(HashTable *table) {
  HTIterator *iter;
  int         i;

  Verify333(table != NULL);

  iter = (HTIterator *) malloc(sizeof(HTIterator));
  Verify333(iter != NULL);

  // If the hash table is empty, the iterator is immediately invalid,
  // since it can't point to anything.
  if (table->num_elements == 0) {
    iter->ht = table;
    iter->bucket_it = NULL;
    iter->bucket_idx = INVALID_IDX;
    return iter;
  }

  // Initialize the iterator.  There is at least one element in the
  // table, so find the first element and point the iterator at it.
  iter->ht = table;
  for (i = 0; i < table->num_buckets; i++) {
    if (LinkedList_NumElements(table->buckets[i]) > 0) {
      iter->bucket_idx = i;
      break;
    }
  }
  Verify333(i < table->num_buckets);  // make sure we found it.
  iter->bucket_it = LLIterator_Allocate(table->buckets[iter->bucket_idx]);
  return iter;
}

void HTIterator_Free(HTIterator *iter) {
  Verify333(iter != NULL);
  if (iter->bucket_it != NULL) {
    LLIterator_Free(iter->bucket_it);
    iter->bucket_it = NULL;
  }
  free(iter);
}

bool HTIterator_IsValid(HTIterator *iter) {
  Verify333(iter != NULL);

  // STEP 4: implement HTIterator_IsValid.
  // The ht must be Not NULL, Not empty, and not passed the end
  return (iter->ht != NULL && iter->ht->num_elements > 0
                            && iter->bucket_idx != INVALID_IDX);
}

bool HTIterator_Next(HTIterator *iter) {
  Verify333(iter != NULL);

  // STEP 5: implement HTIterator_Next.
  // The iter has to be valid
  if (HTIterator_IsValid(iter) == false) {
    return false;
  }

  // If the LL_Next returns true, implying that the current chain still have
  // nodes to iterate, so automatically return true
  // If false, then free the current LLIterator and find the next bucket
  // that contains nodes and create a new LLIterator for that bucket
  if (!LLIterator_Next(iter -> bucket_it)) {
    LLIterator_Free(iter->bucket_it);
    iter -> bucket_idx++;

    while (iter -> bucket_idx < iter -> ht -> num_buckets) {
      if (LinkedList_NumElements(iter ->ht->buckets[iter ->bucket_idx])>0) {
        // The nearest non-empty bucket, take its index
        // and create a LLIterator for it
        int num = iter -> bucket_idx;
        iter->bucket_it = LLIterator_Allocate(iter -> ht->buckets[num]);
        return true;
      } else {
        // Go to the next bucket
        iter -> bucket_idx++;
      }
    }

    // No node found, this iterator is now past the end and invalided
    iter -> bucket_it = NULL;
    iter -> bucket_idx = INVALID_IDX;
    return false;
  }
  return true;
}

bool HTIterator_Get(HTIterator *iter, HTKeyValue_t *keyvalue) {
  Verify333(iter != NULL);

  // STEP 6: implement HTIterator_Get.
  // Iter must be valid and ht must be not empty
  if (!HTIterator_IsValid(iter) || HashTable_NumElements(iter->ht) == 0) {
    return false;
  }
  // Client has the ownership, So SetReturnKV works here
  SetReturnKV(keyvalue, iter->bucket_it);
  return true;
}

bool HTIterator_Remove(HTIterator *iter, HTKeyValue_t *keyvalue) {
  HTKeyValue_t kv;

  Verify333(iter != NULL);

  // Try to get what the iterator is pointing to.
  if (!HTIterator_Get(iter, &kv)) {
    return false;
  }

  // Advance the iterator.  Thanks to the above call to
  // HTIterator_Get, we know that this iterator is valid (though it
  // may not be valid after this call to HTIterator_Next).
  HTIterator_Next(iter);

  // Lastly, remove the element.  Again, we know this call will succeed
  // due to the successful HTIterator_Get above.
  Verify333(HashTable_Remove(iter->ht, kv.key, keyvalue));
  Verify333(kv.key == keyvalue->key);
  Verify333(kv.value == keyvalue->value);

  return true;
}

static void MaybeResize(HashTable *ht) {
  HashTable *newht;
  HashTable tmp;
  HTIterator *it;

  // Resize if the load factor is > 3.
  if (ht->num_elements < 3 * ht->num_buckets)
    return;

  // This is the resize case.  Allocate a new hashtable,
  // iterate over the old hashtable, do the surgery on
  // the old hashtable record and free up the new hashtable
  // record.
  newht = HashTable_Allocate(ht->num_buckets * 9);

  // Loop through the old ht copying its elements over into the new one.
  for (it = HTIterator_Allocate(ht);
       HTIterator_IsValid(it);
       HTIterator_Next(it)) {
    HTKeyValue_t item, unused;

    Verify333(HTIterator_Get(it, &item));
    HashTable_Insert(newht, item, &unused);
  }

  // Swap the new table onto the old, then free the old table (tricky!).  We
  // use the "no-op free" because we don't actually want to free the elements;
  // they're owned by the new table.
  tmp = *ht;
  *ht = *newht;
  *newht = tmp;

  // Done!  Clean up our iterator and temporary table.
  HTIterator_Free(it);
  HashTable_Free(newht, &HTNoOpFree);
}

static bool FindInChain(LinkedList *chain,
                        HTKey_t target_key,
                        LLIterator **return_iter) {
  // No way to find the target, set the return parameter to be NULL
  // so the caller doesn't have to free it
  if (LinkedList_NumElements(chain) == 0) {
    *return_iter = NULL;
    return false;
  }

  HTKeyValue_t *payload;
  LLIterator *iter = LLIterator_Allocate(chain);
  if (iter == NULL) {
    *return_iter = NULL;
    return false;
  }

  // do-while is clever here so iterator won't miss the first node
  do {
    LLIterator_Get(iter, (LLPayload_t*) &payload);
    if (payload->key == target_key) {
      *return_iter = iter;  // The caller need to free this iter.
      return true;
    }
  } while (LLIterator_Next(iter));

  // Couldn't find the target, set return_iter to NULL and free the iter
  *return_iter = NULL;
  LLIterator_Free(iter);
  return false;
}

static void SetKeyValue(LLIterator *iter, HTKeyValue_t *keyvalue) {
  HTKeyValue_t *payload;  // Freed by stack

  // Get the pointer to the target node's payload
  LLIterator_Get(iter, (LLPayload_t*) &payload);

  // Set the target node's payload's key and value
  payload->key = keyvalue->key;
  payload->value = keyvalue->value;
}

static void SetReturnKV(HTKeyValue_t *keyvalue, LLIterator *iter ) {
  LLPayload_t payload;  // Freed by stack

  // Get the pointer points to the kv by using return parameter
  LLIterator_Get(iter, &payload);

  // Set the return parameter's kv to what iter got
  keyvalue->key = ((HTKeyValue_t*)payload)->key;
  keyvalue->value = ((HTKeyValue_t*) payload)->value;
}
