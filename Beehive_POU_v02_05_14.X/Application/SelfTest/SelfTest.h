/*
================================================================================
File name:    SelfTest.h
                    
Originator:   Poorana kumar G

Platform:     dsPIC33EV64GM006
Compiler:     XC16 V1.40
IDE     :     MPLAB X IDE V5.20

Description: 
  The purpose of this class is to execute the class B safety tests at power ON
  as well as periodically. All the tests are executed sequentially
  This will use the safety routines provided by microchip 
  "16-bit PIC24 dsPIC ClassB V2.40".

Class Methods:
  void ClassB_POST(void);
    Call this function after the power up initialization done for the MCU.
    This has to be executed to ensure the safety tests are pass before enter
    into application.


  void ClassB_RunTimeTest(void);
    Call periodically from Scheduler (500 msec), to execute the class B safety
    test periodically to ensure the peripherals are working fine.

Method Calling Requirements:
  selfTest.POST() should be called at Power ON.
  selfTest.RunTimeTest() should be called at scheduler periodically.

Resources:
  This object will access the class B library provided by microchip.
  // Enable the clock input to timer and start it
  #define ENABLE_TIMER_FOR_CLOCK_TEST()
  // Disable the clock input to timer and stop it
  #define DISABLE_TIMER_AFTER_CLOCK_TEST()

================================================================================
 History:	
-*-----*-----------*------------------------------------*-----------------------
       10-01-2019  Initial Write                            Poorana kumar G
2.3.0  09-14-2020  Code flash CRC check is changed to check Poorana kumar G
                   the CRC stored in NVM against calculated
                   one.
--------------------------------------------------------------------------------
*/

#ifndef _SELFTEST_H
#define _SELFTEST_H

#include "Build.h"
#include "IoTranslate.h"
#include "classb.h"
#include "nonVol.h"
#include "FaultIndication.h"

typedef struct{

//  Public Variables
  struct {
    // Temperature control Shut Down flag 0-ON 1-Shutdown        
    uint8_t ignoreINTRTestFLG:1;
  } flags;

// Public Methods
  void (*POST)(void);
  bool (*RunTimeTest)(void);

//  Private Variables
  uint16_t cpuRegisterTestTimerW;
  uint16_t pcTestTimerW;
  uint16_t ramTestTimerW;
  uint16_t flashCRCTestTimerW;
  uint16_t eepromCRCTestTimerW;
  uint16_t clcokTestTimerW;
}  SelfTest_STYP;

// DEFINE CLASS OBJECT DEFAULTS

#define SELF_TEST_DEFAULTS {                                \
                                {1},                        \
                                &ClassB_POST,               \
                                &ClassB_RunTimeTest,        \
                                CPU_REGISTER_TEST_INTERVAL, \
                                PC_REGISTER_TEST_INTERVAL,  \
                                RAM_TEST_INTERVAL,          \
                                FLASH_TEST_INTERVAL,        \
                                EEPROM_TEST_INTERVAL,       \
                                CLOCK_TEST_INTERVAL,        \
                            }

#define CPU_REGISTER_TEST_INTERVAL      120  // * 500ms = 60 seconds
#define PC_REGISTER_TEST_INTERVAL       120  // * 500ms = 60 seconds
#define RAM_TEST_INTERVAL               2    // * 500ms = 1 seconds
#define FLASH_TEST_INTERVAL             1    // * 500ms = 0.5 seconds
#define EEPROM_TEST_INTERVAL            120  // * 500ms = 60 seconds
#define CLOCK_TEST_INTERVAL             7200 // * 500ms = 3600 seconds


#define RAM_RUN_TIME_TEST_SIZE          4    // No of bytes to be tested
#define CODE_RUN_TIME_TEST_SIZE         16   // No of bytes to be tested
#define CPU_CLOCK_FREQUENCY_IN_HZ       15000000
#define REF_CLK_FREQUENCY_IN_HZ         55   // Midpoint of 47 to 63 Hz
#define CLOCK_TEST_TOLERANCE            180  // 18 %

//  CLASS METHOD PROTOTYPES

void ClassB_POST(void);
bool ClassB_RunTimeTest(void);

// Extern variable
extern SelfTest_STYP selfTest;

#endif /* _SELFTEST_H */

