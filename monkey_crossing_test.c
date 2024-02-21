/* Test Driver for Monkey-Crossing problem solved using the 
 * Path Expressions based synchronizer
 */

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#include "child_care.h"
#include "pe_sync.h"

extern const char PATH_EXP[];
extern const char EAST_CROSSING[], WEST_CROSSING[];

// Function which creates a bunch of east and west monkey threads and runs them together
void create_monkey_threads(int n_east_monkeys, int n_west_monkeys, 
    int *e_arr_time, int *w_arr_time)
{
    pthread_t east_monkey_t[n_east_monkeys], west_monkey_t[n_west_monkeys];
    _thread_info_t *e_ti[n_east_monkeys], *w_ti[n_west_monkeys];
    printf("Creating %d East monkey threads and %d West monkey threads\n", n_east_monkeys, n_west_monkeys);
    int i;
    int c_thread = 0;
    for(i=0;i<n_east_monkeys;i++) {
        e_ti[i] = malloc(sizeof(_thread_info_t));
        e_ti[i]->start_time = e_arr_time[c_thread];
        e_ti[i]->tid = c_thread++;
        e_ti[i]->exec_time = 1;
        e_ti[i]->op_name = EAST_CROSSING;
        pthread_create(&east_monkey_t[i], NULL, _handle_thread, e_ti[i]);
    }
    c_thread = 0;
    for(i=0;i<n_west_monkeys;i++) {
        w_ti[i] = malloc(sizeof(_thread_info_t));
        w_ti[i]->start_time = w_arr_time[c_thread];
        w_ti[i]->tid = n_east_monkeys + c_thread++;
        w_ti[i]->exec_time = 1;
        w_ti[i]->op_name = WEST_CROSSING;
        pthread_create(&west_monkey_t[i], NULL, _handle_thread, w_ti[i]);
    }


    printf("East monkey threads and West monkey threads scheduled!\n");

    for(i=0;i<n_east_monkeys;i++) {
        pthread_join(east_monkey_t[i], NULL);
        free(e_ti[i]);
    }
    for(i=0;i<n_west_monkeys;i++) {
        pthread_join(west_monkey_t[i], NULL);
        free(w_ti[i]);
    }

    printf("East monkey threads and West monkey threads completed!\n");
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
    int n_west_monkeys, n_east_monkeys, w_arr_time[10], e_arr_time[10];
    scanf("%d", &n_east_monkeys);
    scanf("%d", &n_west_monkeys);
    
    printf("%d east monkeys and %d west monkeys\n", n_east_monkeys, n_west_monkeys);
    for(i=0;i<n_east_monkeys;i++)
      scanf("%d", &e_arr_time[i]);
    for(i=0;i<n_west_monkeys;i++)
      scanf("%d", &w_arr_time[i]);
    
    set_global_clock();
    create_monkey_threads(n_east_monkeys, n_west_monkeys, e_arr_time, w_arr_time);
    n_test_cases--;
  }

  return 0;
}
