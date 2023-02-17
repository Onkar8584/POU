/*
================================================================================
File name:    Scheduler.c
                    
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

  int32_t Scheduler_add_task(void (*Taskptr)(void), uint16_t initialDelayW,
  uint16_t TimeW);
    To add the task in the scheduler 

Resources:
  None

IoTranslate requirements:
  None

================================================================================
 History:	
-*-----*-----------*------------------------------------*-----------------------
       09-23-2019  Initial Write                        Poorana kumar G
--------------------------------------------------------------------------------
*/


#include "Scheduler.h"
#include "FaultIndication.h"
#include "SelfTest.h"

/*
================================================================================
Method name:    Scheduler_add_task
                    
Originator:   Poorana kumar G

Description: 
  This function is used to add the task to the scheduler. This function will
  receive the pointer of the function which have the task to be executed and the
  time to execute the task. The function pointer and the time interval are
  stored to the task array and the number of tasks are increased. If it unable
  to add the task, this function will return -1.

  This method should be called using scheduler.AddTask() method.

Resources:
  None
================================================================================
 History:	
-*-----*-----------*------------------------------------*-----------------------
       09-23-2019  Initial Write                        Poorana kumar G
--------------------------------------------------------------------------------
*/

int32_t Scheduler_add_task(bool (*Taskptr)(void), uint16_t initialDelayW, uint16_t TimeW)
{
  if ( scheduler.noOfTasks < SCHEDULER_MAX_TASKS) {
    // Store the task in the scheduler
    scheduler.scheduledTasks[scheduler.noOfTasks].counterW = initialDelayW;
    scheduler.scheduledTasks[scheduler.noOfTasks].scheduledTimeW = (TimeW - 1);
    scheduler.scheduledTasks[scheduler.noOfTasks].Taskptr = Taskptr;
    scheduler.scheduledTasks[scheduler.noOfTasks].taskStatus = TASK_COMPLETED;

    // Increment the No.of tasks listed in the scheduler
    scheduler.noOfTasks++;

    // Return Success
    return 0;
  }

  // Return Error
  return -1;
}

/*
================================================================================
Method name:    Scheduler_run
                    
Originator:     Poorana kumar G

Description: 
  Main Monitors the interruptFLG in Scheduler_STYP continuously. Whenever it
  found that bit as '1' i.e 1 ms tick occurs, it will call the Scheduler.run()
  to execute the scheduled tasks. Scheduler have task pointer and the counter to
  execute the task when it expires. For each ticks the counter gets decremented,
  and when it reaches zero scheduler executes the task and reload the counter.
  In this way it will execute all the tasks scheduled whenever tick is getting
  triggered.

  This method should be called using scheduler.run() method.

Resources:
  None
================================================================================
 History:	
-*-----*-----------*------------------------------------*-----------------------
       09-23-2019  Initial Write                        Poorana kumar G
--------------------------------------------------------------------------------
*/

void Scheduler_run(void)
{
  uint8_t Tasks_index = 0;

  for( Tasks_index = 0; Tasks_index < scheduler.noOfTasks; Tasks_index++)
  {
    // If Counter is non zero, decrement it and go to check next task
    if(scheduler.scheduledTasks[Tasks_index].counterW) {
      scheduler.scheduledTasks[Tasks_index].counterW--;
      
      if (scheduler.scheduledTasks[Tasks_index].taskStatus == TASK_NOT_COMPLETED) {
        scheduler.scheduledTasks[Tasks_index].taskStatus =          \
                scheduler.scheduledTasks[Tasks_index].Taskptr();
      }
    }
    else {
      // When Counter is zero, Execute the task and reload it with backup time
      scheduler.scheduledTasks[Tasks_index].taskStatus =            \
              scheduler.scheduledTasks[Tasks_index].Taskptr();

      scheduler.scheduledTasks[Tasks_index].counterW =              \
              scheduler.scheduledTasks[Tasks_index].scheduledTimeW;
    }            
  }

  // Check interrupt flag is set again
  if ( (scheduler.flags.interruptFLG) && (!selfTest.flags.ignoreINTRTestFLG)) {
    // Increment the over run counter
    scheduler.overRunCounter++;
    
    if(scheduler.overRunCounter > MAX_OVERRUNS) {
      faultIndication.Error(OVER_RUN_ERROR);
    }
  }
  else {
    scheduler.overRunCounter = 0;
  }
}
