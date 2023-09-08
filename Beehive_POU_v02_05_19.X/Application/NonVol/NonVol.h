/*
================================================================================
File name:    NonVol.h
                    
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
       09-24-2019  Initial Write                            Poorana kumar G
       11-04-2019  PC-Lint warning is cleared by            Poorana kumar G
                   changing CRC_SEED as 0xFFFF
2.3.0  09-14-2020  Code flash CRC is added in NVM.          Poorana kumar G
2.3.2  05-11-2021  PID constant changes as per the          Dnyaneshwar
                   constant provided from Mike
--------------------------------------------------------------------------------
*/

#ifndef _NONVOL_H
#define	_NONVOL_H

#include "Build.h"
#include "ModeCheck.h"
#include "UserInterface.h"
#include "TemperatureControl.h"
#include "SelfTest.h"

// The structure to store the non volatile settings
typedef struct {
  // Target temperature
  uint16_t targetTemperature;
  // Temperature Mode
  TemperatureMode_ETYP temperatureMode;

  struct {
    // Standby heat operation enable flag 0-Disable 1-Enabled
    uint8_t standbyHeatEnFLG:1;
    // Temperature display format 0-Fahrenheit 1-Celsius
    uint8_t fahrenheitCelsiusSelFLG:1;
  }flags;

  // First critical error
  uint8_t firstCriticalError;
  // Code Flash CRC
  uint16_t codeFlashCRCW;
  uint16_t reserved;
  // PID constants configured through UART
  float pidConstantsARYF[6];                           
  // CRC for the setting
  uint16_t crc16;                                   
} __attribute__((packed)) NonVolSetting_STYP;

//CLASS OBJECT DEFINITION

typedef struct {

//  Public Variables
  NonVolSetting_STYP settings;

// Public Methods
  void (*init)(void);
  void (*write)(void);
  void (*read)(void);
  bool (*ValidateCRC)(void);

// Private Variables
  uint32_t nvmAddress;
} NonVol_STYP;


// DEFINE CLASS OBJECT DEFAULTS

#define NON_VOL_DEFAULTS {          \
  {                                 \
    0,                              \
    0,                              \
    {0,0},                          \
    0,                              \
    0,                              \
    0,                              \
    {0,0,0,0,0,0},                  \
    0                               \
  },                                \
  &NonVol_Init,                     \
  &NonVol_Write,                    \
  &NonVol_Read,                     \
  &NonVolValidateCRC,               \
  0                                 \
}


//  CLASS METHOD PROTOTYPES

void NonVol_Init(void);
void NonVol_Write(void);
void NonVol_Read(void);
bool NonVolValidateCRC(void);


// OTHER DEFINITIONS

#define PID_CONST_KP                nonVol.settings.pidConstantsARYF[0]
#define PID_CONST_KI                nonVol.settings.pidConstantsARYF[1]
#define PID_CONST_KDI               nonVol.settings.pidConstantsARYF[2]
#define PID_CONST_KDD               nonVol.settings.pidConstantsARYF[3]
#define PID_CONST_HI_ERR_THRESH     nonVol.settings.pidConstantsARYF[4]
#define PID_CONST_PREBURN_LIMIT     nonVol.settings.pidConstantsARYF[5]

#define INITIAL_KP                  (0.0015f)       //Values provided from Mike for POU April Updates
#define INITIAL_KI                  (0.0024f)       //Values provided from Mike for POU April Updates
#define INITIAL_KDI                 (0.75f)
#define INITIAL_KDD                 (0.05f)
#define INITIAL_HI_ERR_THRESH       (32.0620f)
#define INITIAL_PREBURN_LIMIT       (0.0625f)

/*#define INITIAL_KP                  (0.075f)
#define INITIAL_KI                  (0.005f)
#define INITIAL_KDI                 (5.0f)
#define INITIAL_KDD                 (1.0f)
#define INITIAL_HI_ERR_THRESH       (1.0f / DegPerADHalfUnit)
#define INITIAL_PREBURN_LIMIT       (0.5f/8.0f)*/

#define CEC_SEED                    0xFFFF

// EXTERN VARIABLES

extern NonVol_STYP nonVol;
extern __attribute__((space(prog))) uint32_t _PROGRAM_END;
extern uint16_t ByteCRC(uint8_t byte, uint16_t crcSeed) ;
uint16_t CalculateCRC(uint8_t *Src, uint16_t length, uint16_t CRCSeed);
void NonVolUpdateTargetTemperature(void);

#endif /* _NONVOL_H */

