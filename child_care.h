#ifndef _CHILD_CARE_H
#define _CHILD_CARE_H 

#include <time.h>

// Thread specific info - For auditing
struct _thread_info
{
  const char *op_name;
	int tid;
  int start_time, exec_time; // In sec
	struct timespec arrival_time, cs_enter_time, cs_exit_time;
};
typedef struct _thread_info _thread_info_t;

// Global Wall Clock
struct timespec global_clock;

// Initialize the global clock
// Call once per test-case
void set_global_clock();

// Common thread handler for all operations
void* _handle_thread(void *);

// Operations
// Denotes arrival of caregiver 
// caregiver is allowed to enter anytime
void caregiver_arrive(_thread_info_t *myinfo);
// Denotes the departure of caregiver
// caregiver is allowed to leave when there are no children
void caregiver_leave(_thread_info_t *myinfo);
// Denotes the arrival of child
// Child is allowed to enter when there is caregiver supervision
void child_arrive(_thread_info_t *myinfo);
//Denotes the departure of child
// Child is allowed to leave anytime
void child_leave(_thread_info_t *myinfo);



#endif
