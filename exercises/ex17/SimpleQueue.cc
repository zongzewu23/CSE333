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

 #include "SimpleQueue.h"

 SimpleQueue::SimpleQueue() {
  // initialize the lock when the queue is constructed
  pthread_mutex_init(&mutex, nullptr);
   size = 0;
   front = nullptr;
   end = nullptr;
 }
 
 SimpleQueue::~SimpleQueue() {
   while (front != nullptr) {
     node *next = front->next;
     delete front;
     front = next;
   }
   // destroy the lock when the queue is destruct
   pthread_mutex_destroy(&mutex);
 }
 
 void SimpleQueue::Enqueue(string item) {
  pthread_mutex_lock(&mutex);  // acquire the lock when entering the function
   node *new_node = new node();
   new_node->next = nullptr;
   new_node->item = item;
   if (end != nullptr) {
     end->next = new_node;
   } else  {
     front = new_node;
   }
   end = new_node;
   size++;
   pthread_mutex_unlock(&mutex);  // release the lock when leaving the function
 }
 
 bool SimpleQueue::Dequeue(string *result) {
  pthread_mutex_lock(&mutex);  // acquire the lock when is about to modify data
   if (size == 0) {
    pthread_mutex_unlock(&mutex);  // release lock before return
     return false;
   }
   *result = front->item;
   node *next = front->next;
   delete front;
   if (end == front) {
     end = front = next;
   } else {
     front = next;
   }
   size--;
   pthread_mutex_unlock(&mutex);  // release the lock before return
   return true;
 }
 
 int SimpleQueue::Size() const {
  pthread_mutex_lock(&mutex);  // acquire the lock when accessing the fields
  int result = size;
  pthread_mutex_unlock(&mutex);  // release when won't modify data anymore
  return result;
}
 
 bool SimpleQueue::IsEmpty() const {
  pthread_mutex_lock(&mutex);  // protact data fields
  int result = size;
  pthread_mutex_unlock(&mutex);  // release before return, so it executes
  return result == 0;
}