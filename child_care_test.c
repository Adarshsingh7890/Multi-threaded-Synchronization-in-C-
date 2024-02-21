/* Test Driver for Child-care problem solved using the 
 * Path Expressions based synchronizer
 */

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#include "child_care.h"
#include "pe_sync.h"

extern const char PATH_EXP[];
extern const char CAREGIVER_ARRIVE[], CAREGIVER_LEAVE[], CHILD_ARRIVE[], CHILD_LEAVE[];

// Function which creates a bunch of Caregivers and Child threads and runs them together
void create_caregiver_child_threads(int n_caregiver, int n_child, 
    int *g_arr_time, int *c_arr_time, int *g_leave_time, int *c_leave_time)
{
	pthread_t caregiver_t[2*n_caregiver], child_t[2*n_child];
  _thread_info_t *g_ti[2*n_caregiver], *c_ti[2*n_child];
  printf("Creating %d Caregiver threads and %d Children threads\n", n_caregiver, n_child);
  int i;
  int c_thread = 0;
  for(i=0;i<n_caregiver;i++) {
    g_ti[i] = malloc(sizeof(_thread_info_t));
    g_ti[i]->start_time = g_arr_time[c_thread];
    g_ti[i]->tid = c_thread++;
    g_ti[i]->exec_time = 1;
    g_ti[i]->op_name = CAREGIVER_ARRIVE;
    pthread_create(&caregiver_t[i], NULL, _handle_thread, g_ti[i]);
  }
  c_thread = 0;
  for(;i<2*n_caregiver;i++) {
    g_ti[i] = malloc(sizeof(_thread_info_t));
    g_ti[i]->start_time = g_leave_time[c_thread];
    g_ti[i]->tid = c_thread++;
    g_ti[i]->exec_time = 1;
    g_ti[i]->op_name = CAREGIVER_LEAVE;
    pthread_create(&caregiver_t[i], NULL, _handle_thread, g_ti[i]);
  }
  c_thread = 0;
  for(i=0;i<n_child;i++) {
    c_ti[i] = malloc(sizeof(_thread_info_t));
    c_ti[i]->start_time = c_arr_time[c_thread];
    c_ti[i]->tid = c_thread++;
    c_ti[i]->exec_time = 1;
    c_ti[i]->op_name = CHILD_ARRIVE;
    pthread_create(&child_t[i], NULL, _handle_thread, c_ti[i]);
  }
  c_thread=0;
  for(;i<2*n_child;i++) {
    c_ti[i] = malloc(sizeof(_thread_info_t));
    c_ti[i]->start_time = c_leave_time[c_thread];
    c_ti[i]->tid = c_thread++;
    c_ti[i]->exec_time = 1;
    c_ti[i]->op_name = CHILD_LEAVE;
    pthread_create(&child_t[i], NULL, _handle_thread, c_ti[i]);
  }

  printf("Caregiver threads and Children threads scheduled!\n");

  for(i=0;i<2*n_caregiver;i++) {
    pthread_join(caregiver_t[i], NULL);
    free(g_ti[i]);
  }
  for(i=0;i<2*n_child;i++) {
    pthread_join(child_t[i], NULL);
    free(c_ti[i]);
  }

  printf("Caregiver Threads and Children threads completed!\n");
}

void init()
{
  // Initialize the synchronizer
  INIT_SYNCHRONIZER(PATH_EXP);
}

int main(int argc, char* argv[]) 
{
  // Initialize all, globally once
  init();

  int n_test_cases, i;
  scanf("%d", &n_test_cases);

  while(n_test_cases > 0) {
    int n_caregivers, n_childs, g_arr_time[10], c_arr_time[10], g_leave_time[10], c_leave_time[10];
    scanf("%d", &n_caregivers);
    scanf("%d", &n_childs);

    for(i=0;i<n_caregivers;i++)
      scanf("%d", &g_arr_time[i]);
    for(i=0;i<n_childs;i++)
      scanf("%d", &c_arr_time[i]);

    for(i=0;i<n_caregivers;i++)
      scanf("%d", &g_leave_time[i]);
    for(i=0;i<n_childs;i++)
      scanf("%d", &c_leave_time[i]);

    set_global_clock();
    create_caregiver_child_threads(n_caregivers, n_childs, g_arr_time, c_arr_time, g_leave_time, c_leave_time);
    n_test_cases--;
  }

  return 0;
}