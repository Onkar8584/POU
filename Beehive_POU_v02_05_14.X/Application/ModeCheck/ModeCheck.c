/*
================================================================================
File name:    ModeCheck.c
                    
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

#include "ModeCheck.h"
#include "NonVol.h"

// Temperature Min, Max and Default setting for each mode (in °F)
const uint16_t userTemperatureMinARYW[4] = {85, 80, 140, 0};
const uint16_t userTemperatureMaxARYW[4] = {85, 150, 180, 0};       //sanitory mode temp reverted from 200 to 180 degree as per comments from mike
const uint16_t UserDefaultARYW[4] = {85, 105, 140, 0};

/*
================================================================================
Method name:  ModeCheckFunction
                    
Originator:   Poorana kumar G

Description: 
  This function is used to check the mode selection pins status periodically.

  This method should be called using modeCheck.Check().

Resources:
  3 GPIOs for 3 mode pins

================================================================================
 History:	
-*-----*-----------*------------------------------------*-----------------------
       09-24-2019  Initial Write                            Poorana kumar G
1.1.0  01-30-2020  Beta requirement changes                 Poorana kumar G
                   Default mode is changed as Lavatory mode.
                   Standby heating is disabled in Emergency
                   Eye wash mode. Lavatory mode range is
                   changed as 80°F-150°F and default is 90°F.
2.1.0  05-12-2020  Filter algorithm added to avoid          Poorana kumar G
                   oscillation between modes as part of
                   FMEA test result. If oscillation
                   found device will go with default mode.
2.2.0  07-16-2020  Error report is added when no jumper or  Poorana kumar G
                   more the one jumper installed or mode
                   oscillation found.
--------------------------------------------------------------------------------
*/

bool ModeCheckFunction(void)
{
  // Must be initialized with the time to report if the error detected at power up
  static uint8_t modeTimer = MODE_CONFIRM_TIME+1;
  TemperatureMode_ETYP curModeFromIO;
  TemperatureMode_ETYP curModeDecided;

  // Check Emergency Eye wash mode GPIO
  if ( EmergencyEyeWashModeDigIn_Read() == false) {
    // If any other mode is also low, go with unknown mode
    if ( (LavatoryModeDigIn_Read() == false) || \
            (SanitationModeDigIn_Read() == false))
    {
      curModeFromIO = UNKNOWN_MODE;
    }
    // Otherwise go with Emergency Eye wash mode
    else {
      curModeFromIO = EMERGENCY_EYE_WASH_MODE;
    }
  }
  // Check Sanitation mode GPIO
  else if ( SanitationModeDigIn_Read() == false) {
    // If any other mode is also low, go with unknown mode
    if ( LavatoryModeDigIn_Read() == false) {
      curModeFromIO = UNKNOWN_MODE;
    }
    // Otherwise go with Sanitary mode
    else {
      curModeFromIO = SANITATION_MODE;
    }
  }
  // Check Sanitation mode GPIO
  else if ( LavatoryModeDigIn_Read() == false) {
    curModeFromIO = LAVATORY_MODE;
  }
  // If no other GPIOs are low, go with Lavatory mode
  else {
    curModeFromIO = UNKNOWN_MODE;
  }

  // If mode change is detected from IO proceed with unknown mode
  if ( modeCheck.prevModeFromIO != curModeFromIO) {
    modeCheck.prevModeFromIO = curModeFromIO;
    curModeDecided = UNKNOWN_MODE;
  }
  else {
    // Otherwise go with detected mode from the GPIO
    curModeDecided = curModeFromIO;
  }

  // If change found in decided mode backup it & reload the timer
  if ( modeCheck.prevModeDecided != curModeDecided) {
    modeCheck.prevModeDecided = curModeDecided;
    modeTimer = MODE_CONFIRM_TIME+1;
  }

  // Down count the timer
  if( modeTimer != 0) {
    modeTimer--;

    if ( modeTimer == 0) {
      if ( curModeDecided == UNKNOWN_MODE) {
        faultIndication.Error(TEMPERATURE_MODE_ERROR);
      }
      else {
        faultIndication.Clear(TEMPERATURE_MODE_ERROR);
      }

      // If mode change detected, save it and load defaults
      if ( curModeDecided != nonVol.settings.temperatureMode) {
        nonVol.settings.temperatureMode = curModeDecided;

        // Discard the current progress going on in UI
        uI.ResetUI();

        // In Emergency Eye wash & Sanitation mode unconditionally disables the standby heating.
        if (curModeDecided != LAVATORY_MODE) {
          nonVol.settings.flags.standbyHeatEnFLG = 0;
        }

        // Check the User setting °C or °F
        if(nonVol.settings.flags.fahrenheitCelsiusSelFLG == 1) {
          modeCheck.temperatureMin = FtoCconvert(userTemperatureMinARYW[curModeDecided]);
          modeCheck.temperatureMax = FtoCconvert(userTemperatureMaxARYW[curModeDecided]);
          nonVol.settings.targetTemperature = FtoCconvert(UserDefaultARYW[curModeDecided]);
        }
        else {
          modeCheck.temperatureMin = userTemperatureMinARYW[curModeDecided];
          modeCheck.temperatureMax = userTemperatureMaxARYW[curModeDecided];
          nonVol.settings.targetTemperature = UserDefaultARYW[curModeDecided];
        }
        uI.targetTemperature = nonVol.settings.targetTemperature;

        uI.DigitConvert(uI.targetTemperature);
        // Load the default temperature into non volatile storage
        nonVol.write();
    
        NonVolUpdateTargetTemperature();
      }
    }
  }

  return TASK_COMPLETED;
}

