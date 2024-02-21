/* Test Driver for Readers-Writers problem implementing the Path Expressions based synchronizer
 */

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#include "readers_writers.h"
#include "pe_sync.h"

extern const char PATH_EXP[], READ_OP[], WRITE_OP[];

// Function which creates a bunch of reader and writer tasks and runs
// them together
void create_read_write_threads(int n_reader, int n_writer, int *r_start_time, int *w_start_time, int *r_exec_time, int *w_exec_time)
{
	pthread_t read_t[n_reader], write_t[n_writer];
  _thread_info_t *r_ti[n_reader], *w_ti[n_writer];
  printf("Creating %d Readers and %d Writers\n", n_reader, n_writer);
  int i;
  int c_tid = 0;
  for(i=0;i<n_reader;i++) {
    r_ti[i] = malloc(sizeof(_thread_info_t));
    r_ti[i]->tid = c_tid++;
    r_ti[i]->start_time = r_start_time[i];
    r_ti[i]->exec_time = r_exec_time[i];
    r_ti[i]->op_name = READ_OP;
    pthread_create(&read_t[i], NULL, _handle_thread, r_ti[i]);
  }
  for(i=0;i<n_writer;i++) {
    w_ti[i] = malloc(sizeof(_thread_info_t));
    w_ti[i]->tid = c_tid++;
    w_ti[i]->start_time = w_start_time[i];
    w_ti[i]->exec_time = w_exec_time[i];
    w_ti[i]->op_name = WRITE_OP;
    pthread_create(&write_t[i], NULL, _handle_thread, w_ti[i]);
  }

  printf("Readers and Writers created Successfully!\n");

  for(i=0;i<n_reader;i++) {
    pthread_join(read_t[i], NULL);
    free(r_ti[i]);
  }
  for(i=0;i<n_writer;i++) {
    pthread_join(write_t[i], NULL);
    free(w_ti[i]);
  }

  printf("Read and Write Tasks completed!\n");
}

void init()
{
  // Initialize the database
  init_db();
  // Initialize the synchronizer
  INIT_SYNCHRONIZER(PATH_EXP);
}

int main(int argc, char* argv[]) 
{
  // Initialize all, Done globally once
  init();

  int n_test_cases, i;
  scanf("%d", &n_test_cases);

  while(n_test_cases > 0) {
    int n_readers, n_writers, r_start_time[10], w_start_time[10], r_exec_time[10], w_exec_time[10];
    scanf("%d", &n_readers);
    scanf("%d", &n_writers);

    for(i=0;i<n_readers;i++)
      scanf("%d", &r_start_time[i]);
    for(i=0;i<n_writers;i++)
      scanf("%d", &w_start_time[i]);
    
    for(i=0;i<n_readers;i++)
      scanf("%d", &r_exec_time[i]);
    for(i=0;i<n_writers;i++)
      scanf("%d", &w_exec_time[i]);

    set_global_clock();
    create_read_write_threads(n_readers, n_writers, r_start_time, w_start_time, r_exec_time, w_exec_time);
    n_test_cases--;
  }

  return 0;
}
