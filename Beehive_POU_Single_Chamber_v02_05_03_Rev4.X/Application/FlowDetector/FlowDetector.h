/*
================================================================================
File name:    FlowDetector.h
                    
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

#ifndef _FLOWDETECTOR_H
#define	_FLOWDETECTOR_H


#include "Build.h"
#include "IoTranslate.h"
#include "FaultIndication.h"


//CLASS OBJECT DEFINITION
typedef struct {
// Public Variables
  struct {
    // Flow detection flag 0-No Flow 1-Flow
    uint8_t flowDetectedFLG:1;
    // Flow Pulse GPIO status backup 0 - Low 1 - High
    uint8_t flowPulsePrevStateFLG:1;
    // Flow Detector connection GPIO status backup 0 - Low 1 - High
    uint8_t flowDetectorConnPrevStateFLG:1;
  } flags;

// Public Methods
  bool (*Detect)(void);
  void (*updateFlow)(int);
// Private Variables
  uint16_t flowDetectorTimerW;
  uint16_t flowDetectMinuteTimerW;
  float flowLowerBoundryW;
  float flowHysteresisOffsetW;
  float flowInGallons;
  float currentFlow;
  float prevFlow;
} FlowDetector_STYP;


// DEFINE CLASS OBJECT DEFAULTS
#define FLOW_DETECTOR_DEFAULTS {{0,0,0},                    \
                                &FlowDetector,              \
                                &update_flowIn_Gallons,     \
                                FLOW_DETECTOR_TIMER,        \
                                FLOW_DETECTOR_MINUTE_TIMER, \
                                LOW_FLOW_THRESHOLD_DEFAULT, \
                                LOW_FLOW_HYSTERESIS_OFFSET_DEFAULT,\
                                FLOW_IN_DEFAULT,            \
                                FLOW_IN_DEFAULT,            \
                                FLOW_IN_DEFAULT             \
                               }


// CLASS METHOD PROTOTYPES
bool FlowDetector(void);

float Get_flowIn_Gallons(void);

void update_flowIn_Gallons(int pulseval);

int check_Flow_Threshold(void);

// OTHER DEFINITIONS
// Minimum no of pulses
#define MINIMUM_TOGGLES_FOR_WATER_FLOW              10

// 1 second
#define FLOW_DETECTOR_TIMER                         1000

// 60 second
#define FLOW_DETECTOR_MINUTE_TIMER                  60

// Flow / second
#define FLOW_IN_DEFAULT                             0.0f

#define LOW_FLOW_THRESHOLD_DEFAULT                  0.6f // Default value 

#define LOW_FLOW_HYSTERESIS_OFFSET_DEFAULT          0.1f // Default value

#define LOW_FLOW_HYST_OFF_LOWER_BOUNDRY             0.01f // Lower HYSTERESIS_OFFSET boundry 

#define LOW_FLOW_HYST_OFF_UPPER_BOUNDRY             1.0f // Upper HYSTERESIS_OFFSET boundry 

// Flow / second
#define FLOW_TOLERANCE                              0.1f

// EXTERN VARIABLES
extern FlowDetector_STYP flowDetector;

#endif	/* _FLOWDETECTOR_H */

