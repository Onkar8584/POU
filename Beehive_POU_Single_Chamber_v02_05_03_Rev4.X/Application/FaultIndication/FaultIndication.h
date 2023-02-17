/*
================================================================================
File name:    FaultCondition.h
                    
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
       09-25-2019  Initial Write                            Poorana kumar G
1.1.0  01-30-2020  As per the Beta requirement changes      Poorana kumar G
                   updated the functions
2.1.0  05-14-2020  Error cods are updated as per the        Poorana kumar G
                   changes.
2.2.0  07-16-2020  New enum & error code is added for       Poorana kumar G
                   temperature mode error.
--------------------------------------------------------------------------------
*/

#ifndef _FAULTINDICATION_H
#define _FAULTINDICATION_H

#include "Build.h"
#include "IoTranslate.h"
#include "NonVol.h"

#define FAULT_BUFFER_SIZE       10

// Enums for errors
typedef enum {
  NO_ERROR = 0,
  DC_SUPPLY_ERROR,
  LEAKAGE_ERROR,
  OVER_RUN_ERROR,
  OVER_HEAT_ERROR,
  FLOW_SENSOR_ERROR,
  FLOW_DIRECTION_ERROR,
  AC_LINE_FREQUENCY_ERROR,
  THERMISTOR1_OPEN_ERROR,
  THERMISTOR2_OPEN_ERROR,
  THERMISTOR3_OPEN_ERROR,
  THERMISTOR4_OPEN_ERROR,
  THERMISTOR5_OPEN_ERROR,
  THERMISTOR6_OPEN_ERROR,
  THERMISTOR1_SHORT_ERROR,
  THERMISTOR2_SHORT_ERROR,
  THERMISTOR3_SHORT_ERROR,
  THERMISTOR4_SHORT_ERROR,
  THERMISTOR5_SHORT_ERROR,
  THERMISTOR6_SHORT_ERROR,
  CPU_REGISTER_TEST_ERROR,
  PROGRAM_COUNTER_TEST_ERROR,
  INTERRUPT_TEST_ERROR,
  CLOCK_TEST_ERROR,
  FLASH_CRC_TEST_ERROR,
  RAM_TEST_ERROR,
  IO_TEST_ERROR,
  EEPROM_CRC_TEST_ERROR,
  BUTTON_MALFUNCTION_ERROR,
  TEMPERATURE_MODE_ERROR
}Errors_ETYP;

//CLASS OBJECT DEFINITION
typedef struct { 

//  Public Variables
  struct {
    // 1-Enable 0-Disable
    uint8_t buzzerToggleEnableFLG:1;
  }flags;
  uint8_t faultCount;
  Errors_ETYP faultsListARY[FAULT_BUFFER_SIZE];

// Public Methods
  bool (*Control)(void);
  void (*Error)(Errors_ETYP);
  void (*Clear)(Errors_ETYP);
  bool (*errorExists)(Errors_ETYP);

//  Private Variables
  // Heart Beat LED timer
  uint16_t heartBeatLEDTimerW;
} FaultIndication_STYP;

// DEFINE CLASS OBJECT DEFAULTS
#define FAULT_INDICATION_DEFAULTS   {                           \
                                      {0},                      \
                                      0,                        \
                                      {0,0,0,0,0,0,0,0,0,0},    \
                                      &BuzzerLEDControl,        \
                                      &FaultReport,             \
                                      &FaultClear,              \
                                      &FaultExists,             \
                                      HBLED_NORMAL_BLINK_TIME,  \
                                    }

#define HBLED_NORMAL_BLINK_TIME     4   // * 250 milli seconds - 0.5 Hz
#define HBLED_ERROR_BLINK_TIME      1   // * 250 milli seconds - 2 Hz

#define NO_FAULTS                   0

// Fault codes - As per the standard error codes through out all the AO Smith
// Products. As per the requirement from Mike.
#define ERROR_CODE_DC_SUPPLY                6
#define ERROR_CODE_LEAKAGE                  31
#define ERROR_CODE_OVER_RUN                 5
#define ERROR_CODE_OVER_HEAT                2
#define ERROR_CODE_FLOW_SENSOR_ERROR        43
#define ERROR_CODE_FLOW_DIRECTION           330
#define ERROR_CODE_AC_LINE_FREQUENCY        9
#define ERROR_CODE_THERMISTOR1_OPEN         28
#define ERROR_CODE_THERMISTOR2_OPEN         27
#define ERROR_CODE_THERMISTOR3_OPEN         3
#define ERROR_CODE_THERMISTOR4_OPEN         4
#define ERROR_CODE_THERMISTOR5_OPEN         25
#define ERROR_CODE_THERMISTOR6_OPEN         26
#define ERROR_CODE_THERMISTOR1_SHORT        28
#define ERROR_CODE_THERMISTOR2_SHORT        27
#define ERROR_CODE_THERMISTOR3_SHORT        3
#define ERROR_CODE_THERMISTOR4_SHORT        4
#define ERROR_CODE_THERMISTOR5_SHORT        25
#define ERROR_CODE_THERMISTOR6_SHORT        26
#define ERROR_CODE_CPU_REGISTER_TEST        6
#define ERROR_CODE_PROGRAM_COUNTER_TEST     6
#define ERROR_CODE_INTERRUPT_TEST           6
#define ERROR_CODE_CLOCK_TEST               6
#define ERROR_CODE_FLASH_CRC_TEST           6
#define ERROR_CODE_RAM_TEST                 6
#define ERROR_CODE_IO_TEST                  6
#define ERROR_CODE_EEPROM_CRC_TEST          6
#define ERROR_CODE_BUTTON_MALFUNCTION       8
#define ERROR_CODE_TEMPERATURE_MODE         6

//  CLASS METHOD PROTOTYPES
bool BuzzerLEDControl(void);
void FaultReport(Errors_ETYP ErrorId);
void FaultClear(Errors_ETYP FaultId);
bool FaultExists(Errors_ETYP faultId);

// EXTERN VARIABLES
extern FaultIndication_STYP faultIndication;

#endif /* _FAULTINDICATION_H */
