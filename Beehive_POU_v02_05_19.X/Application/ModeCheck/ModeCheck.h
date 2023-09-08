/*
================================================================================
File name:    ModeCheck.h
                    
Originator:   Poorana kumar G

Platform:     dsPIC33EV64GM006
Compiler:     XC16 V1.40
IDE     :     MPLAB X IDE V5.20

Description: 
  The purpose of this object is to monitors the 3 temperature selection GPIOs.
  When the mode change is detected in hardware, this object redefines mode
  and as well as temperature range of user selection. When mode change found,
  it will load the default temperature as set temperature.

Class Methods:
  void ModeCheck(void);
    Call periodically from Scheduler (1 sec), to monitor the GPIO status change
    of temperature mode selection GPIOs.

Method Calling Requirements:
  modeCheck.Check() should be called once per 1 second in scheduler.

Resources:
  3 GPIOs for 3 mode pins

IoTranslate requirements:
  #define EmergencyEyeWashModeDigIn_Read()
  #define LavatoryModeDigIn_Read()
  #define SanitationModeDigIn_Read()

================================================================================
 History:	
-*-----*-----------*------------------------------------*-----------------------
       09-24-2019  Initial Write                            Poorana kumar G
1.1.0  01-30-2020  As per the Beta requirement changes      Poorana kumar G
                   updated the functions
2.1.0  05-12-2020  Filter algorithm added to avoid          Poorana kumar G
                   oscillation between modes as part of
                   FMEA test result. If oscillation found
                   device will go with default mode.
2.2.0  07-16-2020  Error report is added when no jumper or  Poorana kumar G
                   more the one jumper installed or mode
                   oscillation found.
--------------------------------------------------------------------------------
*/


#ifndef _MODECHECK_H
#define	_MODECHECK_H

#include "Build.h"
#include "IoTranslate.h"
#include "UserInterface.h"

// Enums for temperature modes
typedef enum {
  EMERGENCY_EYE_WASH_MODE = 0,
  LAVATORY_MODE,
  SANITATION_MODE,
  UNKNOWN_MODE
}TemperatureMode_ETYP;


//CLASS OBJECT DEFINITION
typedef struct {

//  Public Variables
  // Minimum temperature allowed for User selection
  uint16_t temperatureMin;
  // Maximum temperature allowed for User selection
  uint16_t temperatureMax;
  // Backup for the mode read from GPIO
  TemperatureMode_ETYP prevModeFromIO;
  // Backup for the mode decided by function
  TemperatureMode_ETYP prevModeDecided;

// Public Methods
  bool (*Check)(void);

// Private Variables
} ModeCheck_STYP;


// DEFINE CLASS OBJECT DEFAULTS
#define MODE_CHECK_DEFAULTS {       \
  60,                               \
  100,                              \
  UNKNOWN_MODE,                     \
  UNKNOWN_MODE,                     \
  &ModeCheckFunction,               \
}

#define MODE_CONFIRM_TIME       3       // Seconds

//  CLASS METHOD PROTOTYPES
bool ModeCheckFunction(void);


// EXTERN VARIABLES
extern ModeCheck_STYP modeCheck;
extern const uint16_t userTemperatureMinARYW[4];
extern const uint16_t userTemperatureMaxARYW[4];

#endif /* _MODECHECK_H */

