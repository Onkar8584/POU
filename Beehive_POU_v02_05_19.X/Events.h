/*
================================================================================
File name:    Events.c
                    
Originator:   Poorana kumar G

Platform:     dsPIC33EV64GM006
Compiler:     XC16 V1.40
IDE     :     MPLAB X IDE V5.20

Description: 
  To handle the Timer interrupt, Event.c is used. In this interrupt service
  routine it will make interrupt flag as high and check the AC line cross input.

Class Methods:
  void TimerISRFunction(void); 

Method Calling Requirements:
  TimerISRFunction() should be called at Timer ISR.

Resources:
  2 GPIO are required.

IoTranslate requirements:
  #define ACLineCrossDigIn_Read()   // To Read the status of AC Line cross GPIO

================================================================================
 History:	
-*-----*-----------*------------------------------------*-----------------------
       09-30-2019  Initial Write                        Poorana kumar G
1.1.0  02-10-2020  As per the Beta requirement changes  Poorana kumar G
                   updated the functions
--------------------------------------------------------------------------------
*/

#ifndef _EVENTS_H
#define _EVENTS_H


#include "Build.h"
#include "IoTranslate.h"
#include "Scheduler.h"
#include "OptoCouplerControl.h"
#include "FaultIndication.h"

// Interrupt monitor macros
#define TIMER_EXPECTED_COUNT        50
#define TOLERANCE                   2

void TimerISRFunction(void);
void InterruptMonitorTimerISRFunction(void);


#endif /* _EVENTS_H */

