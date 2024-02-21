/* Specification for the child-care problem
 * Imagine you are running a child-care centre. State law stipulates 
 * that every child-care centre must be manned by at least one 
 * caregiver, before children can be dropped off. Caregivers arrive at the 
 * child care centre at any time. Children are allowed to be dropped 
 * off if there exists at least one caregiver. Children can leave anytime. 
 * The last caregiver is allowed to leave only if there are no more children.
 *
 * Operations: CaregiverArrive, CaregiverLeave, ChildArrive, ChildLeave
 * IMPLEMENT:
 * - Provide the path expressions in PATH_EXP[]
 * - Operations CaregiverArrive, CaregiverLeave, ChildArrive, ChildLeave
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <limits.h>
#include <unistd.h>
#include <pthread.h>

#include "child_care.h"

// The synchro library header
#include "pe_sync.h"

const char CHILD_ARRIVE[] = "ChildArrive", CHILD_LEAVE[] = "ChildLeave", 
            CAREGIVER_ARRIVE[] = "CaregiverArrive", CAREGIVER_LEAVE[] = "CaregiverLeave";

// TODO: Write the Path Expression solving the Child-care problem
const char PATH_EXP[] = "path {ChildArrive;ChildLeave} end path {CaregiverArrive;CaregiverLeave} end";

void set_global_clock()
{
  clock_gettime(CLOCK_MONOTONIC, &global_clock);
}

void* _handle_thread(void *arg)
{
   _thread_info_t *myinfo = (_thread_info_t *)arg;

   sleep(myinfo->start_time);        // Wait till thread-start-time
  clock_gettime(CLOCK_MONOTONIC, &myinfo->arrival_time);

  if(strcmp(myinfo->op_name, CAREGIVER_ARRIVE) == 0) { 
    caregiver_arrive(myinfo);
  } else if(strcmp(myinfo->op_name, CAREGIVER_LEAVE) == 0) {
    caregiver_leave(myinfo);
  } else if(strcmp(myinfo->op_name, CHILD_ARRIVE) == 0) {
    child_arrive(myinfo);
  } else if(strcmp(myinfo->op_name, CHILD_LEAVE) == 0) {
    child_leave(myinfo);
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
void caregiver_arrive(_thread_info_t *myinfo)
{
  ENTER_OPERATION(CAREGIVER_ARRIVE);
  clock_gettime(CLOCK_MONOTONIC, &myinfo->cs_enter_time);

  // YOUR CODE HERE

  sleep(myinfo->exec_time);
  clock_gettime(CLOCK_MONOTONIC, &myinfo->cs_exit_time);
  EXIT_OPERATION(CAREGIVER_ARRIVE);
}

void caregiver_leave(_thread_info_t *myinfo)
{
  ENTER_OPERATION(CAREGIVER_LEAVE);
  clock_gettime(CLOCK_MONOTONIC, &myinfo->cs_enter_time);

  // YOUR CODE HERE

  sleep(myinfo->exec_time);
  clock_gettime(CLOCK_MONOTONIC, &myinfo->cs_exit_time);
  EXIT_OPERATION(CAREGIVER_LEAVE);
}

void child_arrive(_thread_info_t *myinfo)
{
  ENTER_OPERATION(CHILD_ARRIVE);
  clock_gettime(CLOCK_MONOTONIC, &myinfo->cs_enter_time);

  // YOUR CODE HERE

  sleep(myinfo->exec_time);
  clock_gettime(CLOCK_MONOTONIC, &myinfo->cs_exit_time);
  EXIT_OPERATION(CHILD_ARRIVE);
}

void child_leave(_thread_info_t *myinfo)
{
  ENTER_OPERATION(CHILD_LEAVE);
  clock_gettime(CLOCK_MONOTONIC, &myinfo->cs_enter_time);

  // YOUR CODE HERE

  sleep(myinfo->exec_time);
  clock_gettime(CLOCK_MONOTONIC, &myinfo->cs_exit_time);
  EXIT_OPERATION(CHILD_LEAVE);
}