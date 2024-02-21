#ifndef _MONKEY_CROSSING_H
#define _MONKEY_CROSSING_H 

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
// Denotes orossing on east
void east_crossing(_thread_info_t *myinfo);

// Denotes orossing on west
void west_crossing(_thread_info_t *myinfo);

#endif
