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
       09-30-2019  Initial Write                            Poorana kumar G
1.1.0  02-10-2020  As per the Beta requirement changes      Poorana kumar G
                   updated the functions. Buzzer control
                   timer callback removed.
--------------------------------------------------------------------------------
*/

#include "Events.h"
#include "SelfTest.h"
#include "Application/OptoCouplerControl/OptoCouplerControl.h"

#define Max_CountACError 12

extern uint16_t debug_PowerADCLocalW ;

static volatile uint16_t timerISRCounts = 0;

uint8_t flag_AcLineError = 0,counter_AcLineError=0;

/*
================================================================================
Method name:  TimerISRFunction
                    
Originator:   Poorana kumar G

Description: 
  To handle the Timer interrupt, Event.c is used. In this interrupt service
  routine it will make interrupt flag as high and check the AC line cross input.

Resources:
  1 GPIO for AC line detection 

================================================================================
 History:	
-*-----*-----------*------------------------------------*-----------------------
       09-30-2019  Initial Write                        Poorana kumar G
--------------------------------------------------------------------------------
*/

void TimerISRFunction(void)
{
  static bool LineCrossFlg = 0;
  static bool LineStatusFlg = 0;

  // Set the interrupt flag to execute the scheduler
  scheduler.flags.interruptFLG = 1;

  // If GPIO status change detected
  if ( LineCrossFlg) {
    LineCrossFlg = 0;

    // Set the flag
    optoCouplerControl.flags.msAfterLCFLG = 1;
  }
  else {
    // If line change detected
    if ( LineStatusFlg ^ ACLineCrossDigIn_Read()) {
      // Backup the AC line current status
      LineStatusFlg = ACLineCrossDigIn_Read();
      LineCrossFlg = 1;
      optoCouplerControl.lcCount++;
    }
  }
  
  // One second timer
  
if ( ++optoCouplerControl.lcCheckTimer >= ONE_SEC_IN_MS ) {
    // Check the AC line frequency is within range
  if ( (optoCouplerControl.lcCount < MIN_AC_LINE_TOGGLES_COUNT) || \
            (optoCouplerControl.lcCount > MAX_AC_LINE_TOGGLES_COUNT)) {
//  if(debug_PowerADCLocalW > 2000)
//            faultIndication.Error(AC_LINE_FREQUENCY_ERROR);
            counter_AcLineError++;
            if(counter_AcLineError > Max_CountACError)
                    flag_AcLineError = 1;
    }
    else {
      faultIndication.Clear(AC_LINE_FREQUENCY_ERROR);
      flag_AcLineError = 0;
    }
  
  if(flag_AcLineError)
  {
      faultIndication.Error(AC_LINE_FREQUENCY_ERROR);
  }
    // Reset the timer
    optoCouplerControl.lcCheckTimer = 0;
    // Reset the AC line cross count
    optoCouplerControl.lcCount = 0;
  }
  timerISRCounts++;
}


/*
================================================================================
Method name:  InterruptMonitorTimerISRFunction
                    
Originator:   Poorana kumar G

Description: 
  To handle the Timer interrupt which is used to monitor the Timer interrupt.

Resources:
  None

================================================================================
 History:	
-*-----*-----------*------------------------------------*-----------------------
       09-30-2019  Initial Write                        Poorana kumar G
--------------------------------------------------------------------------------
*/

void InterruptMonitorTimerISRFunction(void)
{
  if ( selfTest.flags.ignoreINTRTestFLG) {
    selfTest.flags.ignoreINTRTestFLG = 0;

    // Stop the timer, reset it and then start
    RESET_RESTART_INTR_MONITOR_TIMER();
  }
  else {
    if ( (timerISRCounts < (TIMER_EXPECTED_COUNT - TOLERANCE)) ||   \
            (timerISRCounts > (TIMER_EXPECTED_COUNT + TOLERANCE))) {
      faultIndication.Error(INTERRUPT_TEST_ERROR);
    }
  }
  timerISRCounts = 0;
}