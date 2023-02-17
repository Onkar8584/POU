/*
================================================================================
File name:    Scheduler.h
                    
Originator:   Poorana kumar G

Platform:     dsPIC33EV64GM006
Compiler:     XC16 V1.40
IDE     :     MPLAB X IDE V5.20

Description: 
  This is a supervisory object called from Main() when 1ms interrupt sets a flag
  to run the scheduler.   It calls all the time based functions and handles all
  fault and events generated in class level objects.

  Whenever interrupt occurs it will down count the counter for each task and
  when it expired, scheduler execute the task and reloads the counter.

Class Methods:
  void Scheduler_run(void);
    Execute the tasks scheduled whenever timer interrupt occurs

  int32_t Scheduler_add_task(void (*Taskptr)(void), uint32_t TimeW);
    To add the task and time interval into the scheduler 

Resources:
 None
================================================================================
 History:	
-*-----*-----------*------------------------------------*-----------------------
       09-23-2019  Initial Write                        Poorana kumar G
2.3.0  09-15-2020  UI Scheduled time is changed as 2 ms Poorana kumar G
--------------------------------------------------------------------------------
*/


#ifndef _SCHEDULER_H_
#define _SCHEDULER_H_

#include "Build.h"

//CLASS OBJECT DEFINITION
typedef struct {  

//  Public Variables
  struct {
    // INPUT:Set by Timer ISR every 1 ms to let main() know to call Scheduler.run()
    uint8_t interruptFLG:1;
  } flags;
  // OUTPUT: No of tasks scheduled in the scheduler
  uint8_t noOfTasks;
  struct {
    // INPUT: The counter which will be down counted after each ticks
    uint16_t counterW;
    // INPUT: Time interval to execute the task
    uint16_t scheduledTimeW;
    // INPUT: The pointer of the function which have to be executed
    bool (* Taskptr)(void);
    // The boolean variable to store the task completion
    bool taskStatus;
  }scheduledTasks[SCHEDULER_MAX_TASKS];

// Public Methods
  void (* run)(void);
  int32_t (* AddTask)(bool (*Taskptr)(void), uint16_t initialDelayW, uint16_t TimeW);
  
  uint8_t overRunCounter;
} Scheduler_STYP;


//  CLASS METHOD PROTOTYPES
void Scheduler_run(void);
int32_t Scheduler_add_task(bool (*Taskptr)(void), uint16_t initialDelayW, uint16_t TimeW);


// DEFINE CLASS OBJECT DEFAULTS
#define SCHEDULER_DEFAULTS {                \
  .run              = &Scheduler_run,       \
  .AddTask          = &Scheduler_add_task,  \
  .overRunCounter  = 0                     \
}

// OTHER DEFINITIONS

//Scheduler tasks interval time in milliseconds
#ifdef DISABLE_INLET_THERMISTOR
#define ADC_READ_INTERVAL               60//14
#else
#define ADC_READ_INTERVAL               12
#endif
#define FAULT_INDICATION_INTERVAL       250
#define FLOW_DETECTOR_INTERVAL          1
#define MODE_CHECK_INTERVAL             1250
#define OPTOCOUPLER_CONTROL_INTERVAL    1
#define SELFTEST_INTERVAL               500
#define SERIAL_DEBUG_INTERVAL           500
#define TEMPERATURE_CONTROL_INTERVAL    500
// UI scheduled time - 2 msec - Finalized by client
#define USER_INTERFACE_INTERVAL         2

//Scheduler tasks initial interval time in milliseconds to avoid tasks triggered on same time
#define ADC_READ_INITIAL_DELAY              0
#define FAULT_INDICATION_INITIAL_DELAY      20
#define FLOW_DETECTOR_INITIAL_DELAY         0
#define MODE_CHECK_INITIAL_DELAY            40
#define OPTOCOUPLER_CONTROL_INITIAL_DELAY   0
#define SELFTEST_INITIAL_DELAY              5060
#define SERIAL_DEBUG_INITIAL_DELAY          80
#define TEMPERATURE_CONTROL_INITIAL_DELAY   6100
#define USER_INTERFACE_INITIAL_DELAY        0

// Task status booleans
#define TASK_COMPLETED                      0
#define TASK_NOT_COMPLETED                  1

#define MAX_OVERRUNS                        20

// EXTERN VARIABLES
extern Scheduler_STYP scheduler;


#endif /*_SCHEDULER_H_*/

