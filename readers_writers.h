#ifndef _READERS_WRITERS_H
#define _READERS_WRITERS_H 

#include <time.h>

#define DATA_COUNT 100

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
int database[DATA_COUNT];

// Global Wall Clock
struct timespec global_clock;

// Populate the database
void init_db();

// Initialize the global clock, Call once per test-case
// NOT thread safe
void set_global_clock();

// Common thread handler for all operations
void* _handle_thread(void *);

// Operations
// Read Operation: Read Data from the resource if nobody is writing into it, else wait
void read_db(_thread_info_t *myinfo);

// Write Operation: Write data into the db if nobody is reading/writing from it, else wait
void write_db(_thread_info_t *myinfo);

#endif
