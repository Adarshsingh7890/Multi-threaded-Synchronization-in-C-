#ifndef _PE_SYNC_H_
#define _PE_SYNC_H_
#include <semaphore.h>
#include <fcntl.h>


enum opType {L, U, P, V, PP, VV};

struct sem_node{
    sem_t* sem;
    struct sem_node* next;
    int sem_number;
};

struct oper {
    enum opType op;
    pthread_mutex_t* lock;
    pthread_cond_t* cond;
    struct sem_node* sem_list;
    int* count;
    int number;
};

struct tree_node {
    struct oper* prologue;
    struct oper* epilogue;
    struct tree_node* leftChild;
    struct tree_node* rightChild;
    __uint32_t isChild;
    char* path_expr;
    int str_length;
};


sem_t noOfCaregivers;

void ENTER_OPERATION(const char *op_name);

void EXIT_OPERATION(const char *op_name);

void INIT_SYNCHRONIZER(const char *path_exp);

#endif