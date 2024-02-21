/* Specification for the one-slot buffer problem
 * Contains the resource (buffer), Operations (Remove, Append)
 * and the Path Expression governing the operation
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <limits.h>
#include <unistd.h>
#include <pthread.h>

#include "one_slot_buffer.h"

// The synchro library header
#include "pe_sync.h"

const char REMOVE_OP[] = "remove", APPEND_OP[] = "append", PATH_EXP[] = "path append;remove end";

void init_buf()
{
  buffer=0;
}

void set_global_clock()
{
  clock_gettime(CLOCK_MONOTONIC, &global_clock);
}

void* _handle_thread(void *arg)
{
   _thread_info_t *myinfo = (_thread_info_t *)arg;

  sleep(myinfo->start_time);       // Wait till thread-start-time
  clock_gettime(CLOCK_MONOTONIC, &myinfo->arrival_time);

  if(strcmp(myinfo->op_name, REMOVE_OP) == 0) {
    remove_buf(myinfo);
  } else if(strcmp(myinfo->op_name, APPEND_OP) == 0) {
    append_buf(myinfo);
  }

  // Stats for verification (Thread Start Time, Operation Start Time, Operation End Time)
  printf("%s Thread %d Entry %lld.%09ld Start %lld.%09ld End %lld.%09ld\n", 
      myinfo->op_name, myinfo->tid, 
      (long long)myinfo->arrival_time.tv_sec-global_clock.tv_sec, myinfo->arrival_time.tv_nsec-global_clock.tv_nsec,
      (long long)myinfo->cs_enter_time.tv_sec-global_clock.tv_sec, myinfo->cs_enter_time.tv_nsec-global_clock.tv_nsec,
      (long long)myinfo->cs_exit_time.tv_sec-global_clock.tv_sec, myinfo->cs_exit_time.tv_nsec-global_clock.tv_nsec
      );
  return NULL;
}

// Operations
void remove_buf(_thread_info_t *myinfo)
{
  ENTER_OPERATION(REMOVE_OP);
  clock_gettime(CLOCK_MONOTONIC, &myinfo->cs_enter_time);
  
  int data;
  data = buffer;
  printf("Read %d\n", data);
  
  sleep(myinfo->exec_time);
  clock_gettime(CLOCK_MONOTONIC, &myinfo->cs_exit_time);
  EXIT_OPERATION(REMOVE_OP);
}

void append_buf(_thread_info_t *myinfo)
{
  ENTER_OPERATION(APPEND_OP);
  clock_gettime(CLOCK_MONOTONIC, &myinfo->cs_enter_time);
  
  int data = rand()%INT_MAX;
  buffer = data;
  printf("Wrote %d\n", data);
  
  sleep(myinfo->exec_time);
  clock_gettime(CLOCK_MONOTONIC, &myinfo->cs_exit_time);
  EXIT_OPERATION(APPEND_OP);
}
