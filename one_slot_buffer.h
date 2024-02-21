#ifndef _ONE_SLOT_BUFFER_H
#define _ONE_SLOT_BUFFER_H 

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

// Resource
int buffer;

// Global Wall Clock
struct timespec global_clock;

// Populate the database
void init_buf();

// Initialize the global clock
// Call once per test-case
void set_global_clock();

// Common thread handler for all operations
void* _handle_thread(void *);

// Operations
// Remove Operation: Read from the buffer if someone has written into it
void remove_buf(_thread_info_t *myinfo);

// Append Operation: Write into the buffer if it is empty
void append_buf(_thread_info_t *myinfo);

#endif
