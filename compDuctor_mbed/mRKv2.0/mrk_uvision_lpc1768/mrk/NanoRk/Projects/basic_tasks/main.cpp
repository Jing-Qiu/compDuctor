#include "mbed.h"                    
#include <nrk.h>
#include <include.h>
#include <ulib.h>
#include <stdio.h>
#include <hal.h>
#include <nrk_error.h>
#include <nrk_timer.h>
#include <nrk_stack_check.h>
#include <nrk_stats.h>
NRK_STK Stack1[NRK_APP_STACKSIZE];
nrk_task_type TaskOne;
void Task1(void);

NRK_STK Stack2[NRK_APP_STACKSIZE];
nrk_task_type TaskTwo;
void Task2 (void);

NRK_STK Stack3[NRK_APP_STACKSIZE];
nrk_task_type TaskThree;
void Task3 (void);

void nrk_create_taskset();

DigitalOut task1(p21);
DigitalOut task2(p15);
DigitalOut task3(p16);

// You do not need to modify this function
struct __FILE { int handle; };

int main(void)

  {
			
		  task1 = 0; 
		  task2 = 0;
		  task3 = 0;
    	nrk_setup_ports();
			nrk_init();
			nrk_create_taskset();
			nrk_start();
			return 0;

	}


	void Task1()

	{
		
		while(1)
		{
			static int i = 1;
			/*
			static int i = 1;
			nrk_led_toggle(ORANGE_LED);
			for(i=0;i<397528;i++);
			printf("Task 1 count = %d\n\r",i);
			i++;
			*/
			nrk_led_toggle(ORANGE_LED);
			printf("Task 1 count = %d\r\n",i);
			i++;
			
      task1 = 1;
			task2 = 0;
			task3 = 0;
			
			wait_ms(200);
			
			task1 = 0; 
		  task2 = 0;
		  task3 = 0;
			

			nrk_wait_until_next_period();
		}
		
	}

	
	
	void Task2()
	{
		
		while(1)
		{
			static int i = 100;
			/*
			static int i = 1;
			nrk_led_toggle(BLUE_LED);
			for(i=0;i<400000;i++);
			printf("Task 2 count = %d\n\r",i);
			i++;
			*/
			nrk_led_toggle(BLUE_LED);
			
			printf("Task 2 count = %d\r\n",i);
			i++;
			task1 = 0;
			task2 = 1;
			task3 = 0;
			wait_ms(300);
			task1 = 0; 
		  task2 = 0;
		  task3 = 0;
			nrk_wait_until_next_period();
		}
		
	}
	
		void Task3()
	{
		
		while(1)
		{
			static int i = 1000;
			/*
			static int i = 1;
			nrk_led_toggle(BLUE_LED);
			for(i=0;i<400000;i++);
			printf("Task 2 count = %d\n\r",i);
			i++;
			*/
			nrk_led_toggle(RED_LED);
			
			printf("Task 3 count = %d\r\n",i);
			i++;
			task1 = 0;
			task2 = 0;
			task3 = 1;
			wait_ms(200);
			task1 = 0; 
		  task2 = 0;
		  task3 = 0;
			nrk_wait_until_next_period();
		}
		
	}

void nrk_create_taskset()

{
	
    nrk_task_set_entry_function( &TaskOne, Task1);
    nrk_task_set_stk( &TaskOne, Stack1, NRK_APP_STACKSIZE);
    TaskOne.prio = 2;
    TaskOne.FirstActivation = TRUE;
    TaskOne.Type = BASIC_TASK;
    TaskOne.SchType = PREEMPTIVE;
    TaskOne.period.secs = 1;
	
	  TaskOne.relative_deadline.secs = 0;
	  TaskOne.relative_deadline.nano_secs = 800*NANOS_PER_MS;
	
    TaskOne.period.nano_secs = 0; //800*NANOS_PER_MS;
    TaskOne.cpu_reserve.secs = 0;
    TaskOne.cpu_reserve.nano_secs = 0 ;//200*NANOS_PER_MS;
    TaskOne.offset.secs = 0;
    TaskOne.offset.nano_secs= 300*NANOS_PER_MS;
    nrk_activate_task (&TaskOne);
	
	

		nrk_task_set_entry_function( &TaskTwo, Task2);
		nrk_task_set_stk( &TaskTwo, Stack2, NRK_APP_STACKSIZE);
		TaskTwo.prio = 2;
		TaskTwo.FirstActivation = TRUE;
		TaskTwo.Type = BASIC_TASK;
		TaskTwo.SchType = PREEMPTIVE;
		TaskTwo.period.secs = 0;
		
		TaskTwo.relative_deadline.secs = 0;
		TaskTwo.relative_deadline.nano_secs = 500*NANOS_PER_MS;
		
		TaskTwo.period.nano_secs = 700*NANOS_PER_MS;
		TaskTwo.cpu_reserve.secs = 0;
		TaskTwo.cpu_reserve.nano_secs = 0; //200*NANOS_PER_MS;
		TaskTwo.offset.secs = 0;
		TaskTwo.offset.nano_secs= 0;
		nrk_activate_task (&TaskTwo);
		
		nrk_task_set_entry_function( &TaskThree, Task3);
		nrk_task_set_stk( &TaskThree, Stack3, NRK_APP_STACKSIZE);
		TaskThree.prio = 2;
		TaskThree.FirstActivation = TRUE;
		TaskThree.Type = BASIC_TASK;
		TaskThree.SchType = PREEMPTIVE;
		TaskThree.period.secs = 0;
		
		
		TaskThree.relative_deadline.secs = 0;
		TaskThree.relative_deadline.nano_secs = 0;
		
		TaskThree.period.nano_secs = 500*NANOS_PER_MS;
		TaskThree.cpu_reserve.secs = 0;
		TaskThree.cpu_reserve.nano_secs = 0; //200*NANOS_PER_MS;
		TaskThree.offset.secs = 0;
		TaskThree.offset.nano_secs= 0;
		nrk_activate_task (&TaskThree);
	
}
