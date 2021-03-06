/** @file libscheduler.c
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "libscheduler.h"
#include "../libpriqueue/libpriqueue.h"


/**
  Stores information making up a job to be scheduled including any statistics.

  You may need to define some global variables or a struct to store your job queue elements.
*/

//
// Job Struct will be created each time to schedule something new
//
typedef struct _job_t
{
	int job_number;
	int job_priority;
	int job_arrival;
	int job_time_remaining;
	int job_time_running;
} job_t;

//
// Core Strucutre to Form the Size of our Cores (how many job lists we can insert into)
//
typedef struct _core_{
	//How Many cores we will have
	int num_of_cores;
	//Create a job list for each core, job_t**
	job_t** job_list;
} core_t;

//global variables to be used
core_t m_core;
scheme_t m_scheme;
priqueue_t* m_job_queue;

int comparer(const void* job_1, const void* job_2){

	//Grab the Job Pointers
	job_t* job1 = (job_t *)job_1;
	job_t* job2 = (job_t *)job_2;

	//Calculate their arrival times
	int arrival_time_difference = job1->job_arrival - job2->job_arrival;

	if(m_scheme == FCFS){
		return arrival_time_difference;
	}else if(m_scheme== SJF){
		//Calculate the running time difference
		int running_time_difference = job1->job_time_running - job2->job_time_running;

		// In Shortest Job First, if their run times are the same, run whichever arrived first

		if(running_time_difference==0){
			return arrival_time_difference;
		}else{
			return running_time_difference;
		}

	}else if(m_scheme == PSJF){
		//Calculate the remaining time difference
		int remaining_time_difference = job1->job_time_remaining - job2->job_time_remaining;
		//PSJF is like SRTF, if their remaining time is the same, return their arrival time
		if(remaining_time_difference==0){
			return arrival_time_difference;
		}else{
			return remaining_time_difference;
		}

	}else if(m_scheme == PRI){
		//Non preemptive priority based calculate the difference between priorities
		int priority_difference = job1->job_priority - job2->job_priority;
		//PRI returns the arrival time if their priorities are the same
		if(priority_difference == 0){
			return arrival_time_difference;
		}else{
			return priority_difference;
		}
	}else if(m_scheme == PPRI){
		//Preemptive priority based calculate the difference between priorities

		int priority_difference = job1->job_priority - job2->job_priority;
		//PPRI returns the arrival time if their priorities are the same

		if(priority_difference == 0){
			return arrival_time_difference;
		}else{
			return priority_difference;
		}

	}else if(m_scheme == RR){
		return 0;
	}
	printf("Failed Scheme\n");
	return 0;
}



// Job Constructor
job_t* New_Job(int job_number, int time, int running_time, int priority){
	job_t* new_job_ptr = (job_t*)malloc(sizeof(job_t));
	new_job_ptr->job_number = job_number;
	new_job_ptr->job_priority = priority;
	new_job_ptr->job_arrival = time;
	new_job_ptr->job_time_running = running_time;
	new_job_ptr->job_time_remaining = running_time;
	return new_job_ptr;
}

//Core Initializer
void Build_Core(core_t *core, int num){
	core->num_of_cores = num;
	core->job_list = (job_t**)malloc(sizeof(job_t*) * num);

	//Initialize each core's job list
	for(int i = 0; i < num; i++){
		core->job_list[i] = NULL;
	}
}
/**
  Initalizes the scheduler.

  Assumptions:
    - You may assume this will be the first scheduler function called.
    - You may assume this function will be called once once.
    - You may assume that cores is a positive, non-zero number.
    - You may assume that scheme is a valid scheduling scheme.

  @param cores the number of cores that is available by the scheduler. These cores will be known as core(id=0), core(id=1), ..., core(id=cores-1).
  @param scheme  the scheduling scheme that should be used. This value will be one of the six enum values of scheme_t
*/
void scheduler_start_up(int cores, scheme_t scheme)
{
	m_scheme = scheme;

	//Create the Priority Queue
	m_job_queue = (priqueue_t*)malloc(sizeof(priqueue_t));
	//DONE: build scheme comparison
	priqueue_init(m_job_queue,&comparer);
	//Build the Core's Job Lists
	Build_Core(&m_core,cores);
}


/**
  Called when a new job arrives.

  If multiple cores are idle, the job should be assigned to the core with the
  lowest id.
  If the job arriving should be scheduled to run during the next
  time cycle, return the zero-based index of the core the job should be
  scheduled on. If another job is already running on the core specified,
  this will preempt the currently running job.
  Assumptions:
    - You may assume that every job wil have a unique arrival time.

  @param job_number a globally unique identification number of the job arriving.
  @param time the current time of the simulator.
  @param running_time the total number of time units this job will run before it will be finished.
  @param priority the priority of the job. (The lower the value, the higher the priority.)
  @return index of core job should be scheduled on
  @return -1 if no scheduling changes should be made.

 */
int scheduler_new_job(int job_number, int time, int running_time, int priority)
{
	// Create the New Job to Be Inserted
	job_t* newJob = New_Job(job_number,time,running_time,priority);
	printf("Scheduling a new job\n");
	//Search for an Empty Core to Place the Job In
	int core_scheduled_number = 0;
	while(m_core.job_list[core_scheduled_number]!=NULL){
		printf("Core %d is not empty\n",core_scheduled_number);
		core_scheduled_number++;
	}

	// If the number is outside the core length, place it in the queue to schedule later
	if(core_scheduled_number >= m_core.num_of_cores){
		//TODO: If preemptive replace something

		if(m_scheme == PPRI || m_scheme == PSJF){
			//Preemptive, find where to replace if you should
			int insert_index = -1;

			for(int i=0; i < m_core.num_of_cores;i++){
				//Find which core to place it in

			}

			return insert_index;
			//End of TODO
		}else{
			//Non Preemptive just place in the priqueue_t
			priqueue_offer(m_job_queue,newJob);
			return -1;
		}


	}else{
		printf("Placing job in Core: %d\n",core_scheduled_number);
		//There is a place in the core to place it
		m_core.job_list[core_scheduled_number] = newJob;
		return core_scheduled_number;

	}

	return -1;
}


/**
  Called when a job has completed execution.

  The core_id, job_number and time parameters are provided for convenience. You may be able to calculate the values with your own data structure.
  If any job should be scheduled to run on the core free'd up by the
  finished job, return the job_number of the job that should be scheduled to
  run on core core_id.

  @param core_id the zero-based index of the core where the job was located.
  @param job_number a globally unique identification number of the job.
  @param time the current time of the simulator.
  @return job_number of the job that should be scheduled to run on core core_id
  @return -1 if core should remain idle.
 */
int scheduler_job_finished(int core_id, int job_number, int time)
{
	return -1;
}


/**
  When the scheme is set to RR, called when the quantum timer has expired
  on a core.

  If any job should be scheduled to run on the core free'd up by
  the quantum expiration, return the job_number of the job that should be
  scheduled to run on core core_id.

  @param core_id the zero-based index of the core where the quantum has expired.
  @param time the current time of the simulator.
  @return job_number of the job that should be scheduled on core cord_id
  @return -1 if core should remain idle
 */
int scheduler_quantum_expired(int core_id, int time)
{
	return -1;
}


/**
  Returns the average waiting time of all jobs scheduled by your scheduler.

  Assumptions:
    - This function will only be called after all scheduling is complete (all jobs that have arrived will have finished and no new jobs will arrive).
  @return the average waiting time of all jobs scheduled.
 */
float scheduler_average_waiting_time()
{
	return 0.0;
}


/**
  Returns the average turnaround time of all jobs scheduled by your scheduler.

  Assumptions:
    - This function will only be called after all scheduling is complete (all jobs that have arrived will have finished and no new jobs will arrive).
  @return the average turnaround time of all jobs scheduled.
 */
float scheduler_average_turnaround_time()
{
	return 0.0;
}


/**
  Returns the average response time of all jobs scheduled by your scheduler.

  Assumptions:
    - This function will only be called after all scheduling is complete (all jobs that have arrived will have finished and no new jobs will arrive).
  @return the average response time of all jobs scheduled.
 */
float scheduler_average_response_time()
{
	return 0.0;
}


/**
  Free any memory associated with your scheduler.

  Assumptions:
    - This function will be the last function called in your library.
*/
void scheduler_clean_up()
{

}


/**
  This function may print out any debugging information you choose. This
  function will be called by the simulator after every call the simulator
  makes to your scheduler.
  In our provided output, we have implemented this function to list the jobs in the order they are to be scheduled. Furthermore, we have also listed the current state of the job (either running on a given core or idle). For example, if we have a non-preemptive algorithm and job(id=4) has began running, job(id=2) arrives with a higher priority, and job(id=1) arrives with a lower priority, the output in our sample output will be:

    2(-1) 4(0) 1(-1)

  This function is not required and will not be graded. You may leave it
  blank if you do not find it useful.
 */
void scheduler_show_queue()
{

}
