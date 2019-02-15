
/*///////////////////////////////////////////////////////////
 * This Module was written for managing Perioidic Tasks for 
 * Real time system.
 * Ref of implementation:
 * Paper: Ptask: an Educational C Library for Programming Real-
 *      Time Systems on Linux.
 *      Authors: Giorgio Buttazzo, Giuseppe Lipari
 * ///////////////////////////////////////////////////////////
 */ 

#include "ptask.h"

/*------------------------------------------task_create
 *
 * This function create a thread  using pthread_create
 * and setting/initiating the SCHED_RR schedule
 * Inputs:
 *	@task 			task routine
 *	@idx 			task index
 *  @period			task period
 *	@reldeadline 	task relative deadline
 *	@prio			task priority
 *
 * Returns @err, where @err :=0 when the pthread_create
 * call completes successfully.
 ------------------------------------------------------*/
int task_create(void* (*task) (void*), int idx, int period, int reldeadline, int prio) {

	//pthread_attr_t myatt;
	struct sched_param mypar;
	int err;
	/*Assing task parameters to the structure tp of each task*/
	tp[idx].arg			= idx; 
	tp[idx].period		= period;
	tp[idx].deadline	= reldeadline;
	tp[idx].priority	= prio;
	tp[idx].dmiss		= 0; 

	pthread_attr_init(&myatt[idx]);
	pthread_attr_setinheritsched(&myatt[idx], PTHREAD_EXPLICIT_SCHED);

	pthread_attr_setschedpolicy(&myatt[idx], SCHED_RR);

	mypar.sched_priority = tp[idx].priority;
	pthread_attr_setschedparam(&myatt[idx], &mypar);

	err = pthread_create(&tid[idx], &myatt[idx], task, (void*)(&tp[idx])) ;

	return err;
}
/*-------------------------------------------task_terminate
 *
 * This function join/susbends all running threads
 * in the system using the pthread_join call
 --------------------------------------------------------*/
void task_terminate() {	
	int i;
	for (i = 0; i < NUM_TASKS; i++) {
		pthread_join(tid[i],NULL);
		printf("DEBUG\t\tTASK %i terminated\n", i);
	}
}
/*-------------------------------------------get_task_index
 *
 *Returns @tp->arg
 --------------------------------------------------------*/
int get_task_index(void* arg) {
	struct task_par *tp;
	tp = (struct task_par *)arg;

	return tp->arg;
}
/*------------------------------------------get_task_period
 *
 * Returns @tp->period
 --------------------------------------------------------*/
int get_task_period(void* arg) {
	struct task_par *tp;
	tp = (struct task_par *)arg;

	return tp->period;
}
/*---------------------------------------get_task_periority
 *
 * Returns @tp->priority
 --------------------------------------------------------*/
int get_task_priority(void* arg) {
	struct task_par *tp;
	tp = (struct task_par *)arg;

	return tp->priority;
}

/*-------------------------------------------set_activation
 *
 --------------------------------------------------------*/
void set_activation(int i) {
	tspec t;

	clock_gettime(CLOCK_MONOTONIC, &t);
	time_copy(&(tp[i].at), t);
	time_copy(&(tp[i].dl), t);
	time_add_ms(&(tp[i].at), tp[i].period);
	time_add_ms(&(tp[i].dl), tp[i].deadline);
}
/*-----------------------------------------wait_for_period
 *
--------------------------------------------------------*/
void wait_for_period(int i) {
	clock_nanosleep(CLOCK_MONOTONIC, TIMER_ABSTIME, &(tp[i].at), NULL);

	time_add_ms(&(tp[i].at), tp[i].period);
	time_add_ms(&(tp[i].dl), tp[i].period);
}
/*--------------------------------------------deadline_miss
 *
 --------------------------------------------------------*/
int deadline_miss(int i) {
    tspec now;
    clock_gettime(CLOCK_MONOTONIC, &now);
    
    if (time_cmp(now, tp[i].dl) > 0){
        return 1;
    }
    else{
        return 0;
    }
}



