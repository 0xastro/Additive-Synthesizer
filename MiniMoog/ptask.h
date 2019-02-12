#ifndef PTASK_H
#define PTASK_H 

#include <pthread.h>
#include <linux/sched.h>

#include "timemng.h"
#include "shared.h"


/*Tasks Specific MACROS*/
#define NUM_TASKS       8

/*TASK1 PARAMETERS*/
#define SIN_ID          0
#define SIN_PER         5
#define SIN_DL          5
#define SIN_PRIO        80

/*TASK2 PARAMETERS*/
#define SQU_ID          1
#define SQU_PER         5
#define SQU_DL          5
#define SQU_PRIO        80

/*TASK3 PARAMETERS*/
#define TRI_ID          2
#define TRI_PER         5
#define TRI_DL          5
#define TRI_PRIO        70

/*TASK4 PARAMETERS*/
#define AUDIO_ID        3
#define AUDIO_PER       10
#define AUDIO_DL        10
#define AUDIO_PRIO      30

/*TASK4 PARAMETERS*/
#define UI_ID           4
#define UI_PER          30
#define UI_DL           30
#define UI_PRIO         50


/*TASK5 PARAMETERS*/
#define KEY_ID          5
#define KEY_PER         10
#define KEY_DL          10
#define KEY_PRIO        40



//Define theads ID structures
pthread_t       tid[NUM_TASKS]; 
pthread_attr_t  myatt[NUM_TASKS];

struct task_par{
    int arg;            // task argument 
    int period;         // in milliseconds
    int deadline;       // relative deadline in milliseconds
    int priority;       // in [0,99], in POSIX (0,39)
    int dmiss;          // no. of misses
    struct timespec at; // next activ. time
    struct timespec dl; // abs. deadline

    /*RESERVED*/
    //int act_flag;
    //long wcet;
};

//Create a task parametetres structure for all tasks
struct task_par tp[NUM_TASKS];



/*Function Prototypes*/

int task_create(void* (*task) (void*),  //task subroutine
        int idx,  //task index
        int period, //task period
        int reldeadline,//task relative deadline
        int prio  //task priority
        );

void task_terminate();

int get_task_index(void* arg);

int get_task_period(void* arg);

int get_task_prio(void* arg);

void set_activation(int i);

void wait_for_period(int i);

int deadline_miss(int i);




/*Tasks Specific Variables*/
pthread_mutex_t     Rrsrc_mutx, Trsrc_mutx, Srsrc_mutx, UIrsrc_mutx;  //resource lock
pthread_mutexattr_t rsrc_attr;
pthread_barrier_t rsrc_barr;

#endif
