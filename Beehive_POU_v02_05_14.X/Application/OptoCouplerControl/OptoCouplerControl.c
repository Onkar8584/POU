/*
================================================================================
File name:    OptoCouplerControl.c
                    
Originator:   Poorana kumar G

Platform:     dsPIC33EV64GM006
Compiler:     XC16 V1.40
IDE     :     MPLAB X IDE V5.20

Description: 
  This object provides the Opto-coupler control algorithm based on the AC line
  cross detection input and the relay control state machine state. 

Class Methods:
  void OptoCouplerModulate(void);
    Call periodically from Scheduler (1msec),  to control the opto-coupler.
    Whenever AC line cross is detected,  this function will modulate the opto
    coupler as per the relay control state and power cycle calculated.

Method Calling Requirements:
  optoCouplerControl.Modulate() should be called once per 1 millisecond in
  scheduler.

Resources:
  2 GPIOs for control the 2 opto couplers
  2 GPIOs for its feedback

IoTranslate requirements:
  The following #defines to be ON and OFF the Opto couplers
    #define OptoCoupler1ControlDigOut_ON()
    #define OptoCoupler1ControlDigOut_OFF()
    #define OptoCoupler2ControlDigOut_ON()
    #define OptoCoupler2ControlDigOut_OFF()
    #define OptoCoupler1FBStatusDigIn_Read()
    #define OptoCoupler2FBStatusDigIn_Read()

================================================================================
 History:	
-*-----*-----------*------------------------------------*-----------------------
       09-25-2019  Initial Write                        Poorana kumar G
--------------------------------------------------------------------------------
*/


#include "OptoCouplerControl.h"

#define DebounceIterations 5

// Sequence of power cycle in each mode
const uint8_t ModulationSequence[MAX_POWER_MODE][MAX_CROSSING_CYCLES] =  
{
  {OFF, OFF, OFF, OFF, OFF, OFF, OFF, OFF, OFF},    // Mode 0
  {OFF, OFF, OFF, OFF, ON,  OFF, OFF, OFF, OFF},    // Mode 1
  {OFF, OFF, ON,  OFF, OFF, OFF, ON,  OFF, OFF},    // Mode 2
  {OFF, ON,  OFF, OFF, ON,  OFF, ON,  OFF, OFF},    // Mode 3
  {OFF, ON,  OFF, ON,  OFF, ON,  OFF, ON,  OFF},    // Mode 4
  {ON,  OFF, ON,  OFF, ON,  ON,  OFF, ON,  ON },    // Mode 5
  {ON,  OFF, ON,  ON,  ON,  OFF, ON,  ON,  ON },    // Mode 6
  {ON,  ON,  ON,  OFF, ON,  ON,  ON,  ON,  ON },    // Mode 7
  {ON,  ON,  ON,  ON,  ON,  ON,  ON,  ON,  ON }     // Mode 8
};

extern uint16_t Timer_1ms;
extern uint8_t flag_1msTimer;

extern uint16_t Timer_1ms2;
extern uint8_t flag_1msTimer2;

uint8_t flag_once = 0, debounce_call = 0;
uint8_t Flag_Error = 1;

uint8_t flag_once2 = 0, debounce_call2 = 0;
uint8_t Flag_Error2 = 1;
/*
================================================================================
Method name:  OptoCouplerModulate
                    
Originator:   Poorana kumar G

Description: 
  Call periodically from Scheduler (1msec),  to control the opto-coupler.
  Whenever AC line cross is detected,  this function will modulate the opto
  coupler as per the relay control state and power cycle calculated.

  This method should be called using optoCouplerControl.Modulate().

Resources:
  2 GPIOs to control 2 opto coupler 
  2 GPIOs for its feedback

================================================================================
 History:	
-*-----*-----------*------------------------------------*-----------------------
       09-24-2019  Initial Write                        Poorana kumar G
--------------------------------------------------------------------------------
*/

bool OptoCouplerModulate(void)
{
  uint8_t Power = 0;

  // Check the ms after line cross flag
  if ( optoCouplerControl.flags.msAfterLCFLG) {
    // Clear the flag
    optoCouplerControl.flags.msAfterLCFLG = 0;

    // Here we cannot read the opto control pin and process against the feed
    // back status. If the control pin is shorted with ground, the control pin
    // will be read as 0 and feed back will be 1 so it will cause the
    // erroneous behavior board assumes that it as a valid one
    // If both the Control and feedback pins are in same level, report error
    if((tempControl.relayStatus != RELAY_CONTROL_LOWFLOW) &&    \
            (tempControl.relayStatus != RELAY_CONTROL_SHUTDOWN)){
			/**********************************************************************************
			*	Code section commented below corresponds to instaneous read of 
			*	Optocoupler_1 Feedback status Digital Input
			*/        
//        if ( optoCouplerControl.flags.optoCouplerStatusFLG ==       \
//                            OptoCoupler1FBStatusDigIn_Read()) 
////        {
////            faultIndication.Error(IO_TEST_ERROR);
////        }
            /********************************************************************************/
        
			/**********************************************************************************
			*	Code section below corresponds to debounce iterations added for IO_TEST_ERROR
			*	Flag_Error = Optocoupler_1 Feedback status Digital Input
				flag_1msTimer = 100ms timer for Flag_Error 
				DebounceIterations = Number of debounce iteration i.e. 5.
				
			*/        
            if(Flag_Error == 1)  
            {                
                Flag_Error = OptoCoupler1FBStatusDigIn_Read();
            }   
        
            if(Flag_Error == 0)  
            {                
                if(flag_once == 0)
                {
                        flag_once = 1;
                        Timer_1ms = 0;
                }
              
                if(flag_1msTimer == 1)
                {                    
                    flag_once = 0;
                    
                    Timer_1ms = 0;
                    
                    flag_1msTimer = 0;
                    Flag_Error = OptoCoupler1FBStatusDigIn_Read();
                    
                     if(Flag_Error == 0)  
                        debounce_call++;
                     else
                     {
                         faultIndication.Clear(IO_TEST_ERROR);
                         debounce_call = 0;    
                         Flag_Error = 1;
                     }
                }
                if(debounce_call > DebounceIterations)
                {
                    debounce_call = 0;          
                    Flag_Error = 1;
                    flag_once = 0;
                    faultIndication.Error(IO_TEST_ERROR);
                }
              
            }
//        
            /******************************************************************************/
			
			
 			/**********************************************************************************
			*	Code section commented below corresponds to instaneous read of 
			*	Optocoupler_2 Feedback status Digital Input
			*/
        
        // If both the Control and feedback pins are in same level, report error
//        if ( optoCouplerControl.flags.optoCouplerStatusFLG ==       \
//                OptoCoupler2FBStatusDigIn_Read()) {
//            faultIndication.Error(IO_TEST_ERROR);
//        }
			/***********************************************************************************/
			
			/**********************************************************************************
			*	Code section below corresponds to debounce iterations added for IO_TEST_ERROR
			*	Flag_Error2 = Optocoupler_2 Feedback status Digital Input
				flag_1msTimer2 = 100ms timer for Flag_Error2 
				DebounceIterations = Number of debounce iteration i.e. 5.
				
			*/			                        
            if(Flag_Error2 == 1)  
            {             
                Flag_Error2 = OptoCoupler2FBStatusDigIn_Read();
            }
        
            if(Flag_Error2 == 0)  
            {                
                if(flag_once2 == 0)
                {
                        flag_once2 = 1;
                        Timer_1ms2 = 0;
                }
              
                if(flag_1msTimer2 == 1)
                {
//                    flag_once2 = 0;
                    flag_1msTimer2 = 0;
                    
                    Timer_1ms2 = 0;
                    
                    
                    Flag_Error2 = OptoCoupler2FBStatusDigIn_Read();
                    
                     if(Flag_Error2 == 0)  
                        debounce_call2++;
                     else
                     {
                         faultIndication.Clear(IO_TEST_ERROR);
                         debounce_call2 = 0;        
                         Flag_Error2 = 1;
                         flag_once2 = 0;
                     }
                }
                if(debounce_call2 > DebounceIterations)
                {
                    debounce_call2 = 0;          
                    Flag_Error2 = 1;
                    flag_once2 = 0;
                   faultIndication.Error(IO_TEST_ERROR);
                }
              
            }        

    }

    // Check the conditions to control the opto coupler
    if ( (faultIndication.faultCount == NO_FAULTS) &&               \
            ((optoCouplerControl.forcePowerCycle) ||                \
            (tempControl.relayStatus == RELAY_CONTROL_CONTROL) ||   \
            (tempControl.relayStatus == RELAY_CONTROL_LOWFLOW) ||   \
            (tempControl.relayStatus == RELAY_CONTROL_STBYHEAT)) ) {
      // If one round of modulation is finished, refresh the power mode
      if ( optoCouplerControl.flags.modulationFinishedFLG) {
        optoCouplerControl.flags.modulationFinishedFLG = 0;

        // Check the power cycle
        if ( optoCouplerControl.forcePowerCycle) {
          Power = optoCouplerControl.forcePowerCycle;
        }
        else {
          Power = optoCouplerControl.powerCycle;
        }

        // Calculate the power mode from power cycle variable
        optoCouplerControl.powerMode =                              \
                (Power + optoCouplerControl.powerReminder) / 15;
        optoCouplerControl.powerReminder +=                         \
                (Power - (15 * optoCouplerControl.powerMode));

        // If power mode is greater than maximum define mode
        if ( optoCouplerControl.powerMode >= MAX_POWER_MODE) {
          optoCouplerControl.powerMode = (MAX_POWER_MODE - 1);
        }
      }

      // Check the current cycle to be ON or OFF
      if ( ModulationSequence[optoCouplerControl.powerMode]         \
           [optoCouplerControl.crossings]) {
            OptoCoupler1ControlDigOut_ON();
            OptoCoupler2ControlDigOut_ON();

        optoCouplerControl.flags.optoCouplerStatusFLG = ON;
      }
      else {
            OptoCoupler1ControlDigOut_OFF();
            OptoCoupler2ControlDigOut_OFF();

        optoCouplerControl.flags.optoCouplerStatusFLG = OFF;
      }

      // Increment the crossings count
      optoCouplerControl.crossings++;

      // Check the crossings and detect mode finished & toggle offset phase
      if ( (optoCouplerControl.flags.offsetPhaseFLG)) {
        if ( (optoCouplerControl.crossings == MAX_CROSSING_CYCLES)) {
          optoCouplerControl.crossings = 0;
          optoCouplerControl.flags.offsetPhaseFLG = 0;
          optoCouplerControl.flags.modulationFinishedFLG = 1;
        }
      }
      // Check the crossings and detect mode finished & toggle offset phase
      else {
        if (optoCouplerControl.crossings == (MAX_CROSSING_CYCLES-1)) {
          optoCouplerControl.crossings = 1;
          optoCouplerControl.flags.offsetPhaseFLG = 1;
          optoCouplerControl.flags.modulationFinishedFLG = 1;
        }
      }
    }
    else {
      // If no need to ON opto coupler
      OptoCoupler1ControlDigOut_OFF();
      OptoCoupler2ControlDigOut_OFF();

      optoCouplerControl.flags.optoCouplerStatusFLG = OFF;

      // Reset the flags. So from next cycle control start freshly
      optoCouplerControl.crossings = 0;
      optoCouplerControl.flags.offsetPhaseFLG = 0;
      optoCouplerControl.flags.modulationFinishedFLG = 0;
      optoCouplerControl.powerMode = 0;
      optoCouplerControl.powerReminder = 0;
    }
  }

  
  return TASK_COMPLETED;
}

