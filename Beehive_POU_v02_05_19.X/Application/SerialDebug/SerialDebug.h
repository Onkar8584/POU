/*
================================================================================
File name:    SerialDebug.h
                    
Originator:   Poorana kumar G

Platform:     dsPIC33EV64GM006
Compiler:     XC16 V1.40
IDE     :     MPLAB X IDE V5.20

Description: 
  The purpose of this object is to transmit the status of the system collected
  from other objects, forms a packet and to send it through UART. Through the
  same port it can allow the developer to change some variables. This facility
  is only for development and testing purposes.

Class Methods:
  void SerialDebugProcess(void);
    Call periodically from Scheduler (500 msec), to print the status of the
    board 500 ms once through serial port. And the same function receive the
    frame from serial port and process that also.

  static void PrintSting(const char * string, int i);
    Copy the string to the buffer. The parameters are the address of the source
    string and the index.

  static char PrintFloat(float Number, int8_t Digits, int8_t NumFrac);
    Convert the float into ASCII characters and fill it into the buffer. The
    parameters are the number to be converted, Number of digits and the digits
    after the fractions. This function return the number of digits filled in the
    buffer.

  static char PrintInteger(int16_t Number, int8_t Digits);
    Convert the integer into ASCII characters and fill it into the buffer. The
    parameters are the number to be converted and Number of digits. This
    function return the number of digits filled in the buffer.

Method Calling Requirements:
  Serial.DebugFunction() should be called once per 500 millisecond in
  scheduler.

Resources:
  UART Tx and Rx

IoTranslate requirements:
  None

================================================================================
 History:	
-*-----*-----------*------------------------------------*-----------------------
       10-10-2019  Initial Write                        Poorana kumar G
--------------------------------------------------------------------------------
*/

#ifndef _SERIALDEBUG_H
#define _SERIALDEBUG_H

#include "Build.h"
#include "IoTranslate.h"
#include "TemperatureControl.h"

#ifdef DEBUG_MACRO

// Enums for UART serial debug status
typedef enum {
  DEBUG_SERIAL_STATE_IDLE = 0,
  DEBUG_SERIAL_STATE_WAITING,
  DEBUG_SERIAL_STATE_COMMAND_READY,
}DebugState_ETYP;


//CLASS OBJECT DEFINITION

typedef struct {

//  Public Variables
// None

// Public Methods
  bool (*DebugFunction)(void);
  void (*PrintVersion)(void);

// Private Variables
  DebugState_ETYP debugRxStatus;
  uint16_t debugTimeStampW;
  uint8_t debugRxIndex;
  uint8_t debugTxARY[160];
  uint8_t debugRxARY[16];
} SerialDebug_STYP;


// DEFINE CLASS OBJECT DEFAULTS

#define SERIAL_DEBUG_DEFAULTS {                         \
                                &SerialDebugProcess,    \
                                &SerialPrintVersion,    \
                                DEBUG_SERIAL_STATE_IDLE,\
                                0,                      \
                                0,                      \
                              }

#define NUMBER_OF_PARAMETERS            8 // Total Serial Debug Parameters constants
#define START_OF_FLOW_PARAMETER         6 // Total PID constants + First Flow parameters
#define FLOW_LOWER_BOUNDRY_PARAM        6   //flowLowerBoundryW parameter id number
#define FLOW_HYSTERESIS_OFFSET_PARAM    7   // flowHysteresisOffsetW parameter id number
#define DRY_FIRE_THRESHOLD_PARAM        8   // dry fire threshold parameter id number


//  CLASS METHOD PROTOTYPES
bool SerialDebugProcess(void);
void SerialPrintVersion(void);


// EXTERN VARIABLES
extern SerialDebug_STYP Serial;
extern char SWVersion[];

#endif /* DEBUG_MACRO */

#endif /* _SERIALDEBUG_H */

