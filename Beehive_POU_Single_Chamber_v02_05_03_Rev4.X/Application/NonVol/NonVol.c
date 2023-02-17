/*
================================================================================
File name:    NonVol.c
                    
Originator:   Poorana kumar G

Platform:     dsPIC33EV64GM006
Compiler:     XC16 V1.40
IDE     :     MPLAB X IDE V5.20

Description: 
  Handles exchange of parameters between objects and nonVolaltile memory drivers
  and checked with CRCs. This object is not called periodically. It will be used
  by other objects to store the settings into non volatile storage.

Class Methods:
  void NonVol_Write(void);
    To store the settings into non volatile memory.

  void NonVol_Read(void);
    To read the settings from non volatile memory.

Method Calling Requirements:
  nonVol.write() should be called after changed the content to store in flash.
  nonVol.read() should be called to read the content from flash.

Resources:
  Flash read & write driver

IoTranslate requirements:
  None

================================================================================
 History:	
-*-----*-----------*------------------------------------*-----------------------
       09-30-2019  Initial Write                        Poorana kumar G
       10-24-2019  temperatureMin and temperatureMax    Poorana kumar G
                   are initialized based on temperature
                   mode
       11-04-2019  PC-Lint warning is cleared.          Poorana kumar G
2.3.0  09-14-2020  Code flash CRC is added in NVM.          Poorana kumar G
--------------------------------------------------------------------------------
*/

#include "NonVol.h"


/*
================================================================================
Method name:  CalculateCRC
                    
Originator:   Poorana kumar G

Description: 
  This function is used to calculate the crc16 for the given input.

Resources:
  None
================================================================================
 History:	
-*-----*-----------*------------------------------------*-----------------------
       09-30-2019  Initial Write                        Poorana kumar G
--------------------------------------------------------------------------------
*/

uint16_t CalculateCRC(uint8_t *Src, uint16_t length, uint16_t CRCSeed)
{
  uint16_t i;
  for ( i = 0; i < length ; i++) {
    CRCSeed = ByteCRC(Src[i], CRCSeed);
  }

  return CRCSeed;
}


/*
================================================================================
Method name:  NonVol_Init
                    
Originator:   Poorana kumar G

Description: 
  This function is used to check the default values stored in the NVM. And also
  CRC 

  This method should be called using nonVol.init() once at startup.

Resources:
  None
================================================================================
 History:	
-*-----*-----------*------------------------------------*-----------------------
       09-30-2019  Initial Write                            Poorana kumar G
       10-24-2019  temperatureMin and temperatureMax are    Poorana kumar G
                   initialized based on temperature mode
       11-04-2019  PC-Lint warning is cleared by            Poorana kumar G
                   initializing "lastCodeLocation" variable
2.3.0  09-14-2020  Code flash CRC is added in NVM.          Poorana kumar G
--------------------------------------------------------------------------------
*/
void NonVol_Init(void)
{
  // Find the last address of the program
  uint32_t lastCodeLocation = \
          ((uint32_t)((__prog__ uint32_t*)&_PROGRAM_END)) & 0xFFFFFF;

  // Goto next page after this.
  nonVol.nvmAddress = FLASH_GetErasePageAddress(lastCodeLocation) + \
                             (2 * FLASH_ERASE_PAGE_SIZE_IN_INSTRUCTIONS);

  // Read the non volatile settings
  nonVol.read();

  // If CRC is 0xFFFF means default value is not stored.
  if ( nonVol.settings.crc16 == 0xFFFF) {
    // Prepare the default settings and store it
    nonVol.settings.targetTemperature = 0; // Let mode check will decide
    nonVol.settings.flags.fahrenheitCelsiusSelFLG = 0;
    nonVol.settings.flags.standbyHeatEnFLG = 0;
    nonVol.settings.temperatureMode = UNKNOWN_MODE;

    // Calculate the crc16 for code memory
    nonVol.settings.codeFlashCRCW = CLASSB_CRCFlashTest(__RESET_BASE,       \
                            (lastCodeLocation - __RESET_BASE), CEC_SEED);

    // Initialize the PID constants
    PID_CONST_KP            = INITIAL_KP;
    PID_CONST_KI            = INITIAL_KI;
    PID_CONST_KDI           = INITIAL_KDI;
    PID_CONST_KDD           = INITIAL_KDD;
    PID_CONST_HI_ERR_THRESH = INITIAL_HI_ERR_THRESH;
    PID_CONST_PREBURN_LIMIT = INITIAL_PREBURN_LIMIT;

    nonVol.write();
  }
  else {
      //Do nothing
  }

  // Execute EEPROM CRC test
  if(nonVol.ValidateCRC() != true) {
    faultIndication.Error(EEPROM_CRC_TEST_ERROR);
  }
  else {
    // Initialize the backup variables with the mode stored in flash
    // This should be done to detect the mode change done during power OFF.
    modeCheck.prevModeFromIO  = nonVol.settings.temperatureMode;
    modeCheck.prevModeDecided = nonVol.settings.temperatureMode;

    // Check the User setting °C or °F
    if(nonVol.settings.flags.fahrenheitCelsiusSelFLG == 1) {
      modeCheck.temperatureMin = \
            FtoCconvert(userTemperatureMinARYW[nonVol.settings.temperatureMode]);
      modeCheck.temperatureMax = \
            FtoCconvert(userTemperatureMaxARYW[nonVol.settings.temperatureMode]);
    }
    else {
      modeCheck.temperatureMin = \
            userTemperatureMinARYW[nonVol.settings.temperatureMode];
      modeCheck.temperatureMax = \
            userTemperatureMaxARYW[nonVol.settings.temperatureMode];
    }

    // Copy the temperature to UI variable.
    uI.targetTemperature = nonVol.settings.targetTemperature;

    NonVolUpdateTargetTemperature();
  }
}


/*
================================================================================
Method name:  NonVol_Write
                    
Originator:   Poorana kumar G

Description: 
  This function is used to store the edited configuration or settings into flash
  of the controller.

  This method should be called using nonVol.Write().

Resources:
  None
================================================================================
 History:	
-*-----*-----------*------------------------------------*-----------------------
       09-30-2019  Initial Write                        Poorana kumar G
       11-04-2019  PC-Lint warning is cleared by        Poorana kumar G
                   using the return value of the
                   function "FLASH_WriteDoubleWord16".
--------------------------------------------------------------------------------
*/

void NonVol_Write(void)
{
  bool result;
  uint16_t *SettingPTR = ( uint16_t *) &nonVol.settings;
  uint8_t Index = 0;
  uint8_t Size = sizeof(nonVol.settings) / (2 * 2);

  // Update the CRC
  nonVol.settings.crc16 = CalculateCRC((uint8_t *) &nonVol.settings,    \
          sizeof(nonVol.settings)-2, CEC_SEED);

  // Raise flag to ignore the interrupt test because interrupts are disabled
  // during flash erase and write
  // This routine will take around 20 msec to complete the test
  // Stop the timer, reset it and then start
  RESET_RESTART_INTR_MONITOR_TIMER();
  selfTest.flags.ignoreINTRTestFLG = 1;

  FLASH_Unlock(FLASH_UNLOCK_KEY);

  // Erase the page of flash at this address
  result = FLASH_ErasePage(nonVol.nvmAddress);

  // Write 16 bit Data to the first location.
  for( Index = 0; Index < Size; Index++ ) {
    if(result == true) {
      result = FLASH_WriteDoubleWord16((nonVol.nvmAddress + (Index * 4)), \
              SettingPTR[(Index * 2)], SettingPTR[(Index * 2)+ 1]);
    }
    else {
      break;
    }
  }
}


/*
================================================================================
Method name:  NonVol_Read
                    
Originator:   Poorana kumar G

Description: 
  This function is used to read the configuration from flash of the controller
  to the RAM.

  This method should be called using nonVol.Read().

Resources:
  None
================================================================================
 History:	
-*-----*-----------*------------------------------------*-----------------------
       09-30-2019  Initial Write                        Poorana kumar G
--------------------------------------------------------------------------------
*/

void NonVol_Read(void)
{
  uint16_t *SettingPtr = ( uint16_t *) &nonVol.settings;
  uint8_t Index = 0;
  uint8_t Size = sizeof(nonVol.settings) / 2;

  // Read 16 bit Data to the first location.
  for( Index = 0; Index < Size; Index++ ) {
    SettingPtr[Index] = FLASH_ReadWord16((nonVol.nvmAddress + (Index * 2)));
  }
}


/*
================================================================================
Method name:  NonVolValidateCRC
                    
Originator:   Poorana kumar G

Description: 
  This function is used to validate the CRC stored in NVM with the calculated
  one.

  This method should be called using nonVol.ValidateCRC().

Resources:
  None
================================================================================
 History:	
-*-----*-----------*------------------------------------*-----------------------
       10-01-2019  Initial Write                        Poorana kumar G
--------------------------------------------------------------------------------
*/
NonVolSetting_STYP readValue = {0};

bool NonVolValidateCRC(void)
{
  uint16_t *settingPTR = ( uint16_t *) &readValue;
  uint8_t Index = 0;
  uint8_t Size = sizeof(readValue) / 2;

  // Read 16 bit Data to the first location.
  for ( Index = 0; Index < Size; Index++ ) {
    settingPTR[Index] = FLASH_ReadWord16((nonVol.nvmAddress + (Index * 2)));
  }

  if ( readValue.crc16 != 0xFFFF) {
    if ( CalculateCRC((uint8_t *)&readValue,(sizeof(readValue)-2), CEC_SEED) \
            == readValue.crc16) {
      return true;
    }
    else {
      return false;
    }
  }
  // If freshly programmed simply ignore the test and return pass
  else {
    return true;
  }
}


/*
================================================================================
Method name:  NonVolUpdateTargetTemperature
                    
Originator:   Poorana kumar G

Description: 
  This function is used to update the target temperature ADC counts whenever
  there is target temperature is changed by user.

  This method should be called using NonVolUpdateTargetTemperature().

Resources:
  None
================================================================================
 History:	
-*-----*-----------*------------------------------------*-----------------------
       10-01-2019  Initial Write                        Poorana kumar G
       11-04-2019  PC-Lint warning is cleared by type   Poorana kumar G
                   casting the calculated results as
                   "int16_t"
--------------------------------------------------------------------------------
*/
void NonVolUpdateTargetTemperature(void)
{
  // Check the °F or °C flag
  if (nonVol.settings.flags.fahrenheitCelsiusSelFLG == 0) {
    // If °F, copy the setting as it is
    tempControl.targetADCHalfUnitsW = nonVol.settings.targetTemperature;
  }
  else {
    // If °C, convert the setting as °F
    tempControl.targetADCHalfUnitsW =                                       \
            CtoFconvert(nonVol.settings.targetTemperature);
  }

  // Convert the target temperature into half units
  tempControl.targetADCHalfUnitsW =                                         \
          temperatureToADCCount(tempControl.targetADCHalfUnitsW);

  // Convert the over heat temperature into half units
  tempControl.overHeatADCHalfUnits = temperatureToADCCount(OVER_HEAT_TEMPERATURE);
}

