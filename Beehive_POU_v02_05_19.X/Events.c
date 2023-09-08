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
2.5.14 04-28-2023  As per discussion AC Line frequency      Onkar Raut
 *                 error delayed by 10 Sec using 
 *                 Max_CountACError
2.5.15 06-07-2023  In TimerISR Avg.Diff.Temperature 
--------------------------------------------------------------------------------
*/

#include "Events.h"
#include "SelfTest.h"
#include "Application/OptoCouplerControl/OptoCouplerControl.h"

#define Max_CountACError 12

#define DIFFERENTIALCOUNTER 30
#define AVGDIFFTEMPTHRESHOLD 10.0

extern uint16_t debug_PowerADCLocalW ;

static volatile uint16_t timerISRCounts = 0;

uint8_t flag_AcLineError = 0,counter_AcLineError=0;

extern uint16_t DifferentialChamberTemp;

uint8_t DiffTempCounter = 0,flag_AvgDiffTempError = 0;

uint16_t DiffTempAccum = 0;
float AvgDiffChamberTemp = 0;

float Copy_AvgDiffChamberTemp;
uint16_t Copy_DiffTempAccum = 0;
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
       09-30-2019  Initial Write                                 Poorana kumar G
 *     06-07-2023  Average Differential Temperature calculated   Onkar G. Raut.
 *                 Event flag set for Avg. Diff. Temperature
 *                 is greater than 10 between thermistor 1 & 2
 *                 
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
  
  /********************************************************************************************
   *    Two Chamber Algorithm Block
   */
   if(flowDetector.flags.flowDetectedFLG == true)   //Flow Detected
   {
        DifferentialChamberTemp = adcCountToTemperature(adcRead.adcDataARYW[CHAMBER_TEMPERATURE1]) - 
                                  adcCountToTemperature(adcRead.adcDataARYW[CHAMBER_TEMPERATURE2]);

        DifferentialChamberTemp = abs(DifferentialChamberTemp); //Absolute value of differential temperature

        DiffTempAccum += DifferentialChamberTemp;//Accumlated value for Differential temperature
        Copy_DiffTempAccum = DiffTempAccum;
        DiffTempCounter++;

        if(DiffTempCounter >= DIFFERENTIALCOUNTER)           // If samples are more than 30
        {
            DiffTempCounter = 0;
            AvgDiffChamberTemp = DiffTempAccum / DIFFERENTIALCOUNTER;    //Calculate average differential temperature
    //        Copy_DiffTempAccum = DiffTempAccum;
            DiffTempAccum = 0;                          //

            if(AvgDiffChamberTemp >= AVGDIFFTEMPTHRESHOLD)  //If avg. diff. temperature is greater than 10
                flag_AvgDiffTempError = 1;  //Set Flag Handled in TemperatureControl.c

            Copy_AvgDiffChamberTemp = AvgDiffChamberTemp;
            AvgDiffChamberTemp = 0;         
        }
   }
   else
   {
       DifferentialChamberTemp = 0;
       AvgDiffChamberTemp = 0;
        flag_AvgDiffTempError = 0;
        DiffTempCounter = 0;
        DiffTempAccum = 0;  
   }
   /*****************************************************************************/
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