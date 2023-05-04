/*
================================================================================
File name:    OptoCouplerControl.h
                    
Originator:   Poorana kumar G

Platform:     dsPIC33EV64GM006
Compiler:     XC16 V1.40
IDE     :     MPLAB X IDE V5.20

Description: 
  This object provides the Opto-coupler control algorithm based on the AC line
  cross detection input and the relay control state machine state. 

Class Methods:
  void OptoCouplerModulate(void);
    Call periodically from Scheduler (1msec), to control the opto-coupler.
    Whenever AC line cross is detected, this function will modulate the opto
    coupler as per the relay control state and power cycle calculated.

Method Calling Requirements:
  optoCouplerControl.Modulate() should be called once per 1 millisecond in
  scheduler.

Resources:
  2 GPIOs for control the 2 opto couplers

IoTranslate requirements:
  The following #defines to be ON and OFF the Opto couplers
    #define OptoCoupler1ControlDigOut_ON()
    #define OptoCoupler1ControlDigOut_OFF()
    #define OptoCoupler2ControlDigOut_ON()
    #define OptoCoupler2ControlDigOut_OFF()

================================================================================
 History:	
-*-----*-----------*------------------------------------*-----------------------
       09-25-2019  Initial Write                        Poorana kumar G
--------------------------------------------------------------------------------
*/

#ifndef _OPTOCOUPLERCONTROL_H
#define _OPTOCOUPLERCONTROL_H

#include "Build.h"
#include "IoTranslate.h"
#include "TemperatureControl.h"
#include "FaultIndication.h"

//CLASS OBJECT DEFINITION
typedef struct {
//Public Variables
  struct {
    // Set after 1 ms from the AC line cross detected
    uint8_t  msAfterLCFLG:1;
    // To indicate the on cycle of modulation is completed
    uint8_t  modulationFinishedFLG:1;
    // Toggles whenever one cycle of modulation completed
    uint8_t  offsetPhaseFLG:1;
    // Opto coupler Control status 0 - OFF 1 - ON
    uint8_t  optoCouplerStatusFLG:1;
  } flags;
  // Power cycle to be applied to opto coupler
  uint8_t powerCycle;
  // Forced Power cycle received form UART - Debugging purpose only
  uint8_t forcePowerCycle;

// Public Methods
  // The function used to modulate the opto control
  bool (*Modulate)(void);

// Private Variables
  // Opto coupler powering Mode
  uint8_t powerMode;
  // Powering Modes reminder form power cycle
  uint8_t powerReminder;
  // crossings count
  uint8_t crossings;
  // AC Line cross count
  uint8_t lcCount;
  // 1 sec timer to calculate frequency
  uint16_t lcCheckTimer;
} OptoCouplerControl_STYP;

// DEFINE CLASS OBJECT DEFAULTS
#define OPTO_COUPLER_CONTROL_DEFAULTS   {                       \
                                          {0,0,0,0},            \
                                          0,                    \
                                          0,                    \
                                          &OptoCouplerModulate, \
                                          0,                    \
                                          0,                    \
                                          0,                    \
                                          0,                    \
                                          0,                    \
                                        }

// OTHER DEFINITIONS
#define OFF                             0
#define ON                              1
#define MAX_POWER_MODE                  9
#define MAX_CROSSING_CYCLES             9
#define MIN_AC_LINE_TOGGLES_COUNT       94      // 47 Hz
#define MAX_AC_LINE_TOGGLES_COUNT       126     // 63 Hz
#define ONE_SEC_IN_MS                   1000    // ms

//  CLASS METHOD PROTOTYPES
bool OptoCouplerModulate(void);


// EXTERN VARIABLES
extern OptoCouplerControl_STYP optoCouplerControl;

#endif /* _OPTOCOUPLERCONTROL_H */

