#ifndef PTASK_H
#define PTASK_H 

#define _GNU_SOURCE

#include <pthread.h>
#include <linux/sched.h>
#include <semaphore.h>

#include "timemng.h"
#include "shared.h"
#include "task_configs.h"

/*-------------------------------------------------------------------*/
struct task_par{
	int 	arg;            // task argument 
	int 	period;			// in milliseconds
	int	 	deadline;		// relative deadline in milliseconds
	int 	priority;		// in [0,99], in POSIX (0,39)
	int 	dmiss;			// no. of misses
	struct 	timespec at;	// next activ. time
	struct 	timespec dl;	// abs. deadline
};

//Create a task parametetres structure for all tasks
struct task_par	tp[NUM_TASKS];

pthread_t       tid[NUM_TASKS]; 
pthread_attr_t  myatt[NUM_TASKS];


/*Function Prototypes*/
/*-------------------------------------------------------------------*/

int task_create(void* (*task) (void*),	//task subroutine
		int idx,						//task index
		int period,						//task period
		int reldeadline,				//task relative deadline
		int prio						//task priority
		);

void task_terminate(void);

int get_task_index(void* arg);

int get_task_period(void* arg);

int get_task_prio(void* arg);

void set_activation(int i);

void wait_for_period(int i);

int deadline_miss(int i);
/*-------------------------------------------------------------------*/

/*Tasks Specific Variables
    * 3 mutex @Srsrc_mutx, @Trsrc_mutx and @Rrsrc_mutx 
    *       are used to protect the waveforms generations
    * mutex attribute @rsrc_attr is defined
    *       for error checking
    * a barrier @rsrc_barr is defined to achieve syncronizaton
    *       between the sound dependent threads 
    *       [audio, sine_src, tri_src, squar_src]
 	*/

pthread_mutex_t		Rrsrc_mutx, 
					Trsrc_mutx,
					Srsrc_mutx, 
					UIrsrc_mutx;

pthread_mutexattr_t	rsrc_attr;

pthread_barrier_t	rsrc_barr;


#endif
