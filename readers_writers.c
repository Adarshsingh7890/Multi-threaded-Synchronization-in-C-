/* Specification for the Readers-Writers problem
 * Contains the resource (database), Operations (Read, Write)
 * and the Path Expression governing the operation
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <limits.h>
#include <unistd.h>
#include <pthread.h>

#include "readers_writers.h"

// The synchronizer header
#include "pe_sync.h"

const char READ_OP[] = "read", WRITE_OP[] = "write", PATH_EXP[] = "path {read}+write end";

void init_db()
{
  int i=0;
  for(i=0;i<DATA_COUNT;i++) {
    database[i] = rand()%INT_MAX;   // Populate the database with random values
  }
}

void set_global_clock()
{
  clock_gettime(CLOCK_MONOTONIC, &global_clock);
}

void* _handle_thread(void *arg)
{
   _thread_info_t *myinfo = (_thread_info_t *)arg;

  sleep(myinfo->start_time);        // Wait till thread-start-time
  clock_gettime(CLOCK_MONOTONIC, &myinfo->arrival_time);

  if(strcmp(myinfo->op_name, READ_OP) == 0) {
    read_db(myinfo);
  } else if(strcmp(myinfo->op_name, WRITE_OP) == 0) {
    write_db(myinfo);
  }

  // Stats for verification (Thread Entry time, Operation Start Time, Operation End Time)
  printf("%s Thread %d Entry %lld.%09ld Start %lld.%09ld End %lld.%09ld\n", 
      myinfo->op_name, myinfo->tid, 
      (long long)myinfo->arrival_time.tv_sec-global_clock.tv_sec, myinfo->arrival_time.tv_nsec-global_clock.tv_nsec,
      (long long)myinfo->cs_enter_time.tv_sec-global_clock.tv_sec, myinfo->cs_enter_time.tv_nsec-global_clock.tv_nsec,
      (long long)myinfo->cs_exit_time.tv_sec-global_clock.tv_sec, myinfo->cs_exit_time.tv_nsec-global_clock.tv_nsec
      );
  return NULL;
}

// Operations
void read_db(_thread_info_t *myinfo)
{
  ENTER_OPERATION(READ_OP);
  clock_gettime(CLOCK_MONOTONIC, &myinfo->cs_enter_time);
  
  int data;
  int key = rand()%DATA_COUNT;
  data = database[key];
  printf("Read %d from %d\n", data, key);
  
  sleep(myinfo->exec_time);
  clock_gettime(CLOCK_MONOTONIC, &myinfo->cs_exit_time);
  EXIT_OPERATION(READ_OP);
}

void write_db(_thread_info_t *myinfo)
{
  ENTER_OPERATION(WRITE_OP);
  clock_gettime(CLOCK_MONOTONIC, &myinfo->cs_enter_time);
  
  int key = rand()%DATA_COUNT;
  int data = rand()%INT_MAX;
  database[key] = data;
  printf("Wrote %d into %d\n", data, key);
  sleep(myinfo->exec_time);
  clock_gettime(CLOCK_MONOTONIC, &myinfo->cs_exit_time);
  EXIT_OPERATION(WRITE_OP);
}
