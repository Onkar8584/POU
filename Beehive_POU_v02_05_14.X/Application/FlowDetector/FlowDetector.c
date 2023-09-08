/*
================================================================================
File name:    FlowDetector.c
                    
Originator:   Poorana kumar G

Platform:     dsPIC33EV64GM006
Compiler:     XC16 V1.40
IDE     :     MPLAB X IDE V5.20

Description: 
  The purpose of this object is to monitor the flow detector GPIO input and 
  inform when the water flow is detected. The aim now is to detect the water
  flow only. When the minimum IO toggles found in 1 sec time frame, it will be
  considered as water flow found.

Class Methods:
  void FlowDetector(void);
    Called periodically from Scheduler (1 msec), to monitor the GPIO status
    change of Flow detector GPIO and count the no. of toggles. And if minimum
    toggles for water flow is found, it will set flow detection bit.

Method Calling Requirements:
  flowDetector.Detect() must be called 1 msec once in scheduler.

Resources:
  2 GPIOs which are connected with flow detector pulse & connection
  
IoTranslate requirements:
  2 Macros are used to read the GPIO's current status
    #define FlowDetectorPulseDigIn_Read()
    #define FlowDetectorConnDigIn_Read

================================================================================
 History:	
-*-----*-----------*------------------------------------*-----------------------
       09-23-2019  Initial Write                        Poorana kumar G
1.1.0  02-04-2020  As per the Beta requirement changes  Poorana kumar G
                   updated the functions
--------------------------------------------------------------------------------
*/

#include "FlowDetector.h"


Errors_ETYP prevResult;

extern uint8_t flag_scale_reset;

/*
================================================================================
Method name:    FlowDetector
                    
Originator:   Poorana kumar G

Description: 
  Call periodically from Scheduler (1 msec), to monitor the GPIO status change
  of Flow detector GPIO and count the no. of toggles. And if minimum toggles for
  water flow is found, it will set flow detection bit.

  This method should be called using flowDetector.Detect() method.

Resources:
  2 GPIOs which are connected with flow detector pulse & connection

================================================================================
 History:	
-*-----*-----------*------------------------------------*-----------------------
       09-23-2019  Initial Write                            Poorana kumar G
1.1.0  02-04-2020  As per the Beta requirement changes flow Poorana kumar G
                   detector connection check GPIO polling
                   and error report is added.
--------------------------------------------------------------------------------
*/

bool FlowDetector(void)
{
  // Variable to store the toggles
  static uint16_t flowDetectorTogglesW = 0;
  // Variable to store the current status of flow detector GPIO
  bool gpioCurrentStatus = false;

  // Read the current status of the flow detector GPIO
  gpioCurrentStatus = FlowDetectorPulseDigIn_Read();

  // Check with previous status
  if ( gpioCurrentStatus != flowDetector.flags.flowPulsePrevStateFLG) {
      // If status change detected, store it & increment the toggles count
      flowDetector.flags.flowPulsePrevStateFLG = gpioCurrentStatus;
      flowDetectorTogglesW++;
  }

  // Decrement the flow detection timer
  if( --flowDetector.flowDetectorTimerW == 0) {
    // Reload the timer after it reaches 0
    flowDetector.flowDetectorTimerW = FLOW_DETECTOR_TIMER;

    // Read the current status of the flow detector connection GPIO
    gpioCurrentStatus = FlowDetectorConnDigIn_Read();

    // If previous status and current status both are high report the error
    if ( (gpioCurrentStatus) && (flowDetector.flags.flowDetectorConnPrevStateFLG)) {
      faultIndication.Error(FLOW_SENSOR_ERROR);
      flowDetector.flags.flowDetectedFLG = false;
    }
    else {
      faultIndication.Clear(FLOW_SENSOR_ERROR);

      // Check GPIO toggles count with minimum toggles and decide flow detection
      if (flowDetectorTogglesW >= MINIMUM_TOGGLES_FOR_WATER_FLOW) {
        flowDetector.flags.flowDetectedFLG = true;
        
//        flag_scale_reset = 0;   //Flow detected for next Scale detect algorithm event
      }
      else {
        flowDetector.flags.flowDetectedFLG = false;
      }
    }
 
    // Convert the flow toggles per second to get frequency
    update_flowIn_Gallons(flowDetectorTogglesW);
    
    // Backup the connection GPIO status
    flowDetector.flags.flowDetectorConnPrevStateFLG = gpioCurrentStatus;

    // Reset the toggles count
    flowDetectorTogglesW = 0;
  }

  return TASK_COMPLETED;
}

void update_flowIn_Gallons(int pulseval ){
    /*F(Hz)=(8.1*Q-3)±10% Q=(L/MIN)*/ 
    float ftemp = 0;
    ftemp = (float)(pulseval/2);
    ftemp = ftemp / 13.3f;
    flowDetector.currentFlow = ftemp * 0.264172f;
    
    if(flowDetector.flags.flowDetectedFLG == true){
        if((flowDetector.currentFlow >= (flowDetector.prevFlow - FLOW_TOLERANCE))  &&     \
                (flowDetector.currentFlow < (flowDetector.prevFlow + FLOW_TOLERANCE))) {
            flowDetector.flowInGallons = flowDetector.currentFlow;
            flowDetector.prevFlow = flowDetector.currentFlow;
        }
        else{
            flowDetector.prevFlow = flowDetector.currentFlow;
        }
    }
}

float Get_flowIn_Gallons(void){
  return flowDetector.flowInGallons ;  
}

int check_Flow_Threshold(void){
    if(flowDetector.flags.flowDetectedFLG == true) 
    {
        
        if (flowDetector.flowInGallons  < flowDetector.flowLowerBoundryW){     
            prevResult = FLOW_SENSOR_ERROR;
            return FLOW_SENSOR_ERROR;
          }
        else if (flowDetector.flowInGallons  >= (flowDetector.flowLowerBoundryW + flowDetector.flowHysteresisOffsetW)) {
            prevResult = NO_ERROR;
            return NO_ERROR;
          }
        else
        {
           return  prevResult;
        }
    }
    else{
        return NO_ERROR;
    }
}