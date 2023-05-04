/*
================================================================================
File name:    FaultCondition.c
                    
Originator:   Poorana kumar G

Platform:     dsPIC33EV64GM006
Compiler:     XC16 V1.40
IDE     :     MPLAB X IDE V5.20

Description: 
  The purpose of this object is to control all the LEDs and buzzer in the board
  based on the status and errors detected.

Class Methods:
  void BuzzerLEDControl(void);
    Call periodically from Scheduler (250 msec), to control the GPIO of the LEDs
   and Buzzer to indicate the status of the board as well as the error codes.

Method Calling Requirements:
  faultIndication.Control() should be called once per 250 millisecond in
  scheduler.

Resources:
  3 GPIOs connected with LEDs are required.

IoTranslate requirements:
  #define CelsiusLEDDigOut_ON()            // To turn ON the Celsius LED
  #define CelsiusLEDDigOut_OFF()          // To turn OFF the Celsius LED
  #define FahrenheitLEDDigOut_ON()       // To turn ON the Fahrenheit LED
  #define FahrenheitLEDDigOut_OFF()     // To turn OFF the Fahrenheit LED
  #define HeartBeatLEDDigOut_ON()        // To turn ON the Heart Beat LED
  #define HeartBeatLEDDigOut_OFF()      // To turn OFF the Heart Beat LED
  #define HeartBeatLEDDigOut_TOGGLE()      // To toggle the Heart Beat LED

================================================================================
 History:	
-*-----*-----------*------------------------------------*-----------------------
       10-03-2019  Initial Write                            Poorana kumar G
1.1.0  01-30-2020  As per the Beta requirement changes      Poorana kumar G
                   updated the functions
2.1.0  05-14-2020  Error cods are updated as per the        Poorana kumar G
                   changes.
2.2.0  07-16-2020  Reset the error flag when first error    Poorana kumar G
                   detected is added in fault report
                   function.
2.3.0  09-14-2020  Buzzer will be switched ON only when     Poorana kumar G
                   the leakage detected.
2.3.1  12-28-2020  Fault Storing in Eeprom is removed       Dnyaneshwar

--------------------------------------------------------------------------------
*/

#include "FaultIndication.h"

#define DebounceCounter 10

uint8_t counter_ErrorDetect=0;

uint8_t Flag_ErrorScaleBank1=0, Flag_ErrorScaleBank2=0;

/*
================================================================================
Method name:  IsCriticalError
                    
Originator:   Poorana kumar G

Description: 
  To check the error critical one or not.

  This method should be called using IsCriticalError(Errors_ETYP).

Resources:
  None
================================================================================
 History:	
-*-----*-----------*------------------------------------*-----------------------
       14-05-2020  Initial Write                        Poorana kumar G
--------------------------------------------------------------------------------
*/

static inline bool IsCriticalError(Errors_ETYP faultId)
{
  bool retVal = false;

  // Return true if the fault ID is for Class B safety errors
  if (((faultId >= CPU_REGISTER_TEST_ERROR) && (faultId <= RAM_TEST_ERROR)) || (faultId == EEPROM_CRC_TEST_ERROR)) {
    retVal = true;
  }
  
  return retVal;
}


/*
================================================================================
Method name:  BuzzerLEDControl
                    
Originator:   Poorana kumar G

Description: 
  Call periodically from Scheduler (250 msec), to control the GPIO of the LEDs
  and Buzzer to indicate the status of the board as well as the error codes.

  This method should be called using faultIndication.Control().

Resources:
  3 GPIOs for 3 LEDs
================================================================================
 History:	
-*-----*-----------*------------------------------------*-----------------------
       10-03-2019  Initial Write                            Poorana kumar G
1.1.0  01-30-2020  Bi-Color LED is removed as per beta      Poorana kumar G
                   requirement changes
2.3.0  09-14-2020  Buzzer will be switched ON only when     Poorana kumar G
                   the leakage detected.
--------------------------------------------------------------------------------
*/

bool BuzzerLEDControl(void)
{
  // Control °C/°F LED based on user selection
  if ( uI.flags.displayEnFLG) {
    if ( !nonVol.settings.flags.fahrenheitCelsiusSelFLG) {
    FahrenheitLEDDigOut_ON();
    CelsiusLEDDigOut_OFF();
    }
    else {
      FahrenheitLEDDigOut_OFF();
      CelsiusLEDDigOut_ON();
    }
  }

  // Heart beat LED Toggling
  if ( --faultIndication.heartBeatLEDTimerW == 0) {
    HeartBeatLEDDigOut_TOGGLE();

    // If error detected, Toggle the LED fast
    if ( faultIndication.faultCount != NO_FAULTS) {
      faultIndication.heartBeatLEDTimerW = HBLED_ERROR_BLINK_TIME;
    }
    // If nor error detected, Toggle the LED slowly
    else {
      faultIndication.heartBeatLEDTimerW = HBLED_NORMAL_BLINK_TIME;
    }
  }

  // If error detected, turn ON the buzzer
  if ( faultIndication.errorExists(LEAKAGE_ERROR) == true) {
    if ( faultIndication.flags.buzzerToggleEnableFLG == 0) {
      BuzzerDigOut_ON();
      faultIndication.flags.buzzerToggleEnableFLG = 1;
    }
  }
  
  //   If error detected, turn ON the buzzer
  if ( faultIndication.errorExists(SCALE_DETECTION_ERROR) == true) {
    if ( faultIndication.flags.buzzerToggleEnableFLG == 0) {
      BuzzerDigOut_ON();
      faultIndication.flags.buzzerToggleEnableFLG = 1;      
    }
  }
  
  return TASK_COMPLETED;
}


/*
================================================================================
Method name:  FaultReport
                    
Originator:   Poorana kumar G

Description: 
  Add the error occurred in the error buffer. Before add that it will check that
  error is already existing or not.

  This method should be called using faultIndication.Error(Errors_ETYP).

Resources:
  None
================================================================================
 History:	
-*-----*-----------*------------------------------------*-----------------------
       10-03-2019  Initial Write                            Poorana kumar G
2.2.0  07-16-2020  Reset the error flag when first error    Poorana kumar G
                   detected is added.
--------------------------------------------------------------------------------
*/

void FaultReport(Errors_ETYP faultId)
{
  uint8_t faultIndex = 0;

  if ( faultIndication.faultCount < FAULT_BUFFER_SIZE) {
    for ( faultIndex = 0; faultIndex < faultIndication.faultCount; faultIndex++) {
      if(faultIndication.faultsListARY[faultIndex] == faultId) {
        return;
    }

    }

    // Reset the flag to 0 to show "Err" in display when first error is logged
    // It is not sure the previous status of the bit when the previous error is cleared
    if (faultIndication.faultCount == 0) {
        uI.flags.errorDisplayFLG = 0;
    }
    
    counter_ErrorDetect++;
    
//    faultIndication.faultsListARY[faultIndication.faultCount] = faultId;
//    faultIndication.faultCount++;
//    faultIndication.faultsListARY[faultIndication.faultCount] = faultId;
//    counter_ErrorDetect = faultId;    
    

    if(faultId == 26)       //10 for ch1
    {
        if(counter_ErrorDetect > DebounceCounter)
        {          
            faultIndication.faultsListARY[faultIndication.faultCount] = faultId;
            faultIndication.faultCount++;
        }
    }
            
    else
    {
            faultIndication.faultsListARY[faultIndication.faultCount] = faultId;
            faultIndication.faultCount++;
            
            if(faultId == 31)       //Bank1 Scale detection error
            {
              
//                flag_active = 1;
                Flag_ErrorScaleBank1 = 1;
            }
                
            
            if(faultId == 32)       //10 for ch1
            {
                Flag_ErrorScaleBank2 = 1;            
//                flag_active = 1;
            }
    }
//    

//    faultIndication.faultCount++;
//    if ( IsCriticalError(faultId) == true) {
//      if ( nonVol.settings.firstCriticalError == 0) {
//        nonVol.settings.firstCriticalError = (uint8_t) faultId;
//        nonVol.write();
//      }
//    }
  }
}


/*
================================================================================
Method name:  FaultClear
                    
Originator:   Poorana kumar G

Description: 
  To remove the error occurred from the error buffer.

  This method should be called using faultIndication.Clear(Errors_ETYP).

Resources:
  None
================================================================================
 History:	
-*-----*-----------*------------------------------------*-----------------------
       10-03-2019  Initial Write                            Poorana kumar G
2.3.0  09-15-2020  After cleared all the faults, digits     Poorana kumar G
                   updated with the temperature to display
--------------------------------------------------------------------------------
*/

void FaultClear(Errors_ETYP faultId)
{
  uint8_t faultIndex = 0;

  if ( (faultIndication.faultCount != NO_FAULTS) && \
          ( faultIndication.faultCount < FAULT_BUFFER_SIZE)) {

    if ( IsCriticalError(faultId) == false) {
      for ( faultIndex = 0; faultIndex < faultIndication.faultCount; faultIndex++) {
        if (faultIndication.faultsListARY[faultIndex] == faultId) {
          break;
        }
      }

      if ( faultIndex < faultIndication.faultCount) {
        faultIndication.faultCount--;
        
        counter_ErrorDetect = 0;

        // Once all the errors are cleared
        if ( faultIndication.faultCount == NO_FAULTS)
        {
          // Convert the temperature into 7 segment digits
          uI.DigitConvert(uI.targetTemperature);
        }

        for ( ; faultIndex < faultIndication.faultCount; faultIndex++) {
          faultIndication.faultsListARY[faultIndex] = \
                  faultIndication.faultsListARY[(faultIndex+1)];
        }
      }
    }
  }
}


/*
================================================================================
Method name:  FaultExists
                    
Originator:   Poorana kumar G

Description: 
  To check the error is already exists or not.

  This method should be called using faultIndication.errorExists(Errors_ETYP).

Resources:
  None
================================================================================
 History:	
-*-----*-----------*------------------------------------*-----------------------
       10-03-2019  Initial Write                        Poorana kumar G
--------------------------------------------------------------------------------
*/

bool FaultExists(Errors_ETYP faultId)
{
  uint8_t faultIndex = 0;
  bool errorExixts = false;

  if ( (faultIndication.faultCount != NO_FAULTS)) {
    for ( faultIndex = 0; faultIndex < faultIndication.faultCount; faultIndex++) {
      if (faultIndication.faultsListARY[faultIndex] == faultId) {
        errorExixts = true;
        break;
      }
    }
  }
  
  return errorExixts;
}


