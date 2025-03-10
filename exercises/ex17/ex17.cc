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

 #include <iostream>
 #include <thread>
 #include <chrono>
 #include <cstdlib>
 #include <string>
 #include <ctime>
 
 #include "SimpleQueue.h"
 
 using std::cout;
 using std::endl;
 using std::string;
 using std::thread;
 
 const int NUM_PIES = 6;                 // # pies created by each producer
 static SimpleQueue queue;               // queue of pies
 static unsigned int seed = time(NULL);  // initialize random sleep time
 static pthread_mutex_t write_lock;      // mutex for cout
 
 // Thread safe print that prints the given str on a line
 void thread_safe_print(string str) {
   pthread_mutex_lock(&write_lock);
   // Only one thread can hold the lock at a time, making it safe to
   // use cout. If we didn't lock before using cout, the order of things
   // put into the stream could be mixed up.
   cout << str << endl;
   pthread_mutex_unlock(&write_lock);
 }
 
 // Produces NUM_PIES pies of the given type
 // You should NOT modify this method at all
 void producer(string pie_type) {
   for (int i = 0; i < NUM_PIES; i++) {
     queue.Enqueue(pie_type);
     thread_safe_print(pie_type + " pie ready!");
     int sleep_time = rand_r(&seed) % 500 + 1;
     std::this_thread::sleep_for(std::chrono::milliseconds(sleep_time));
   }
 }
 
 // Eats 2 * NUM_PIES pies
 // You should NOT modify this method at all
 void consumer() {
   for (int i = 0; i < NUM_PIES * 2; i++) {
     bool successful = false;
     string pie_type;
     while (!successful) {
       while (queue.IsEmpty()) {
         // Sleep for a bit and then check again
         int sleep_time = rand_r(&seed) % 800 + 1;
         std::this_thread::sleep_for(std::chrono::milliseconds(sleep_time));
       }
       successful = queue.Dequeue(&pie_type);
     }
     thread_safe_print(pie_type + " pie eaten!");
   }
 }

 // wrapper funtion that allows us to call the producer, and without modifying
 // the return type of the producer.
 void *thread_producer(void *arg) {
  // arg here is a pointer to the string
  producer(*reinterpret_cast<string *>(arg));
  return nullptr;
 }

 // wrapper funtion that allows us to call the consumer, and without modifying
 // the return type of the consumer.
 // parameter void *arg is not voidable, just pass in a nullptr later
 void *thread_consumer(void *arg) {
  consumer();
  return nullptr;
 }
 
 int main(int argc, char **argv) {
   pthread_mutex_init(&write_lock, NULL);
   // Your task: Make the two producers and the single consumer
   // all run concurrently (hint: use pthreads)
  //  producer("Apple");
  //  producer("Blackberry");
  //  consumer();
  // thread numbers that can operate the thread later on
  pthread_t prod1, prod2, cons1;
  string apple = "Apple";
  string blackberry = "Blackberry";
  // create a thread for apple producer, now produce apple pie concurrently
  // with others(prod2, cons1)
  if (pthread_create(&prod1, nullptr, &thread_producer,
                      reinterpret_cast<void *>(&apple)) != 0) {
    std::cerr << "pthread_create() failed." << std::endl;
    pthread_mutex_destroy(&write_lock);  // destory before return, ensure exe
    return EXIT_FAILURE;
  }

  // create a thread for blackberry producer, now produce blackbarry pie
  // concurrently with others(prod1, cons1)
  if (pthread_create(&prod2, nullptr, &thread_producer,
                      reinterpret_cast<void *>(&blackberry)) != 0) {
    std::cerr << "pthread_create() failed." << std::endl;
    pthread_mutex_destroy(&write_lock);  // destory before return, ensure exe
    return EXIT_FAILURE;
  }

  // create a thread for consumer, now eating both apple pie and blackbarry pie
  // concurrently while the prod1 and prod2 are producing pies
  if (pthread_create(&cons1, nullptr, &thread_consumer,
                      nullptr) != 0) {
    std::cerr << "pthread_create() failed." << std::endl;
    pthread_mutex_destroy(&write_lock);  // destory before return, ensure exe
    return EXIT_FAILURE;
  }

  // wait for the prod1 to produce all the pies it wants to produce
  // in case main function returns before it finishes its job
  if (pthread_join(prod1, nullptr) != 0) {
    std::cerr << "pthread_join() failed." << std::endl;
    pthread_mutex_destroy(&write_lock);  // destory before return, ensure exe
    return EXIT_FAILURE;
  }

  // wait for the prod2 to produce all the pies it wants to produce
  // in case main function returns before it finishes its job
  if (pthread_join(prod2, nullptr) != 0) {
    std::cerr << "pthread_join() failed." << std::endl;
    pthread_mutex_destroy(&write_lock);  // destory before return, ensure exe
    return EXIT_FAILURE;
  }

  // wait for the consumer to eat all the pies it has to eat
  // in case main function returns before it finishes its job
  if (pthread_join(cons1, nullptr) != 0) {
    std::cerr << "pthread_join() failed." << std::endl;
    pthread_mutex_destroy(&write_lock);  // destory before return, ensure exe
    return EXIT_FAILURE;
  }

   pthread_mutex_destroy(&write_lock);
   return EXIT_SUCCESS;
 }