#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <limits.h>
#include <unistd.h>
#include <pthread.h>

#include "monkey_crossing.h"

// The synchro library header
#include "pe_sync.h"

const char EAST_CROSSING[] = "EastCrossing", WEST_CROSSING[] = "WestCrossing";

// TODO: Write the Path Expression solving the Monkey-Crossing problem
const char PATH_EXP[] = "path /5EastCrossing\\+/5WestCrossing\\ end";

void set_global_clock()
{
  clock_gettime(CLOCK_MONOTONIC, &global_clock);
}

void* _handle_thread(void *arg)
{
   _thread_info_t *myinfo = (_thread_info_t *)arg;

   sleep(myinfo->start_time);        // Wait till thread-start-time
  clock_gettime(CLOCK_MONOTONIC, &myinfo->arrival_time);

  if(strcmp(myinfo->op_name, EAST_CROSSING) == 0) { 
    east_crossing(myinfo);
  } else if(strcmp(myinfo->op_name, WEST_CROSSING) == 0) {
    west_crossing(myinfo);
  } 

  // Stats for verification
  printf("%s Thread %d Entry %lld.%09ld Start %lld.%09ld End %lld.%09ld\n", 
      myinfo->op_name, myinfo->tid, 
      (long long)myinfo->arrival_time.tv_sec-global_clock.tv_sec, myinfo->arrival_time.tv_nsec-global_clock.tv_nsec, 
      (long long)myinfo->cs_enter_time.tv_sec-global_clock.tv_sec, myinfo->cs_enter_time.tv_nsec-global_clock.tv_nsec,
      (long long)myinfo->cs_exit_time.tv_sec-global_clock.tv_sec, myinfo->cs_exit_time.tv_nsec-global_clock.tv_nsec
      );
  return NULL;
}

// Operations
void east_crossing(_thread_info_t *myinfo)
{
    ENTER_OPERATION(EAST_CROSSING);
    clock_gettime(CLOCK_MONOTONIC, &myinfo->cs_enter_time);

    // YOUR CODE HERE

    sleep(myinfo->exec_time);
    clock_gettime(CLOCK_MONOTONIC, &myinfo->cs_exit_time);
    EXIT_OPERATION(EAST_CROSSING);
}

void west_crossing(_thread_info_t *myinfo)
{
    ENTER_OPERATION(WEST_CROSSING);
    clock_gettime(CLOCK_MONOTONIC, &myinfo->cs_enter_time);

    // YOUR CODE HERE

    sleep(myinfo->exec_time);
    clock_gettime(CLOCK_MONOTONIC, &myinfo->cs_exit_time);
    EXIT_OPERATION(WEST_CROSSING);
}
