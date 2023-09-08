/*
================================================================================
File name:    SelfTest.c
                    
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
  And for IO test it access 6 feedback GPIOs

  // Enable the clock input to timer and start it
  #define ENABLE_TIMER_FOR_CLOCK_TEST()
  // Disable the clock input to timer and stop it
  #define DISABLE_TIMER_AFTER_CLOCK_TEST()

================================================================================
 History:	
-*-----*-----------*------------------------------------*-----------------------
       10-01-2019  Initial Write                            Poorana kumar G
       10-24-2019  crcCalculated is reset to CEC_SEED       Poorana kumar G
                   after it compares final CRC in
                   "CodeFlashTest" function.
       11-04-2019  PC-Lint warnings are cleared.            Poorana kumar G
2.3.0  09-14-2020  Code flash CRC check is changed to check Poorana kumar G
                   the CRC stored in NVM against calculated
                   one.
--------------------------------------------------------------------------------
*/

#include "SelfTest.h"


/*
================================================================================
Method name:  RAMTest
                    
Originator:   Poorana kumar G

Description: 
  Call this function after the timer for RAM test timer expires in run time.

  This method should be called using CodeFlashTest().

Resources:
  Class B safety library provided by Microchip
================================================================================
 History:	
-*-----*-----------*------------------------------------*-----------------------
       10-01-2019  Initial Write                        Poorana kumar G
       11-04-2019  PC-Lint warning is cleared by        Poorana kumar G
                   type casting as "uint16_t"
--------------------------------------------------------------------------------
*/

static void RAMTest(void)
{
  static uint16_t ramIndexW = __DATA_BASE;

  if ( CLASSB_RAMCheckerboardTest(__DATA_BASE, RAM_RUN_TIME_TEST_SIZE) !=   \
          CLASSB_TEST_PASS) {
    faultIndication.Error(RAM_TEST_ERROR);
  }

  ramIndexW += RAM_RUN_TIME_TEST_SIZE;
  
  if ( ramIndexW >= (uint16_t) (__DATA_BASE + __DATA_LENGTH)) {
    ramIndexW = __DATA_BASE;
  }
}


/*
================================================================================
Method name:  CodeFlashTest
                    
Originator:   Poorana kumar G

Description: 
  Call this function after the timer for Code flash test timer expires in run
  time.

  This method should be called using CodeFlashTest().

Resources:
  Class B safety library provided by Microchip
================================================================================
 History:	
-*-----*-----------*------------------------------------*-----------------------
       10-01-2019  Initial Write                            Poorana kumar G
       10-24-2019  crcCalculated is reset to CEC_SEED       Poorana kumar G
                   after it compares final CRC
2.3.0  09-15-2020  Calculated CRC will be compared with     Poorana kumar G
                   the stored one in NVM
--------------------------------------------------------------------------------
*/

static void CodeFlashTest(void)
{
  static uint32_t flashIndex = __RESET_BASE;
  static uint16_t crcCalculated = CEC_SEED;
  uint32_t lastLoc = ((uint32_t)((__prog__ uint32_t*)&_PROGRAM_END)) & 0xFFFFFF;
  uint32_t byteCount = 0;

  // Find the bytes count
  byteCount = lastLoc - flashIndex;
  if ( byteCount > CODE_RUN_TIME_TEST_SIZE) {
    byteCount = CODE_RUN_TIME_TEST_SIZE;
  }

  // Calculate CRC
  crcCalculated = CLASSB_CRCFlashTest(flashIndex, byteCount, crcCalculated);

  // Go to next index
  flashIndex += byteCount;

  if ( flashIndex >= lastLoc) {
    flashIndex = __RESET_BASE;

    if ( nonVol.settings.codeFlashCRCW != crcCalculated) {
      faultIndication.Error(FLASH_CRC_TEST_ERROR);
    }

    // Reset the CRC to its seed
    crcCalculated = CEC_SEED;
  }
}


/*
================================================================================
Method name:  ClassB_POST
                    
Originator:   Poorana kumar G

Description: 
  Call this function after the power up initialization done for the MCU.
  This has to be executed to ensure the safety tests are pass before enter into
  application.

  This method should be called using selfTest.POST().

Resources:
  Class B safety library provided by Microchip
================================================================================
 History:	
-*-----*-----------*------------------------------------*-----------------------
       10-01-2019  Initial Write                            Poorana kumar G
       11-04-2019  PC-Lint warning is cleared by updating   Poorana kumar G
                   the Class B PC test check condition.
2.3.0  09-14-2020  Code flash CRC check is changed to check Poorana kumar G
                   the CRC stored in NVM against calculated
                   one.
--------------------------------------------------------------------------------
*/

void ClassB_POST(void)
{
  // Find the last address of the program
  uint32_t codeEnd = ((uint32_t)((__prog__ uint32_t*)&_PROGRAM_END)) & 0xFFFFFF;
  // Variable to hold the clock test result
  CLASSBRESULT clockTestResult = CLASSB_TEST_PASS;

  // Execute CPU Register test
  if ( CLASSB_CPURegistersTest() != CLASSB_TEST_PASS) {
    faultIndication.Error(CPU_REGISTER_TEST_ERROR);
  }

  // Execute CPU Program counter test
  CLASSB_CPUPCTest();
  if ( CLASSB_CPUPCTestGetResult()) {
    faultIndication.Error(PROGRAM_COUNTER_TEST_ERROR);
  }

  // Execute RAM Checker Board for entire RAM region
  if ( CLASSB_RAMCheckerboardTest(__DATA_BASE, __DATA_LENGTH) != CLASSB_TEST_PASS) {
    faultIndication.Error(RAM_TEST_ERROR);
  }

  // Calculate the crc16 for code flash memory and check it against CRC in NVM
  if ( CLASSB_CRCFlashTest(__RESET_BASE,(codeEnd - __RESET_BASE), CEC_SEED) != \
          nonVol.settings.codeFlashCRCW) {
    faultIndication.Error(FLASH_CRC_TEST_ERROR);
  }

  // Execute the clock test
  ENABLE_TIMER_FOR_CLOCK_TEST();

  selfTest.flags.ignoreINTRTestFLG = 1;

  clockTestResult = CLASSB_ClockTest(CPU_CLOCK_FREQUENCY_IN_HZ,             \
          REF_CLK_FREQUENCY_IN_HZ, CLOCK_TEST_TOLERANCE);

  if ( clockTestResult != CLASSB_TEST_PASS) {
//    faultIndication.Error(CLOCK_TEST_ERROR);
  }

  DISABLE_TIMER_AFTER_CLOCK_TEST();
}


/*
================================================================================
Method name:  ClassB_RunTimeTest
                    
Originator:   Poorana kumar G

Description: 
  Call periodically from Scheduler (500 msec), to execute the class B safety
  test periodically to ensure the peripherals are working fine.

  This method should be called using selfTest.RunTimeTest().

Resources:
  Class B safety library provided by Microchip
================================================================================
 History:	
-*-----*-----------*------------------------------------*-----------------------
       10-01-2019  Initial Write                        Poorana kumar G
       11-04-2019  PC-Lint warning is cleared by        Poorana kumar G
                   updating the Class B PC test check
                   condition.
--------------------------------------------------------------------------------
*/

bool ClassB_RunTimeTest(void)
{
  // Variable to hold the clock test result
  CLASSBRESULT clockTestResult = CLASSB_TEST_PASS;

  // Execute CPU Register test
  if ( --selfTest.cpuRegisterTestTimerW == 0) {
    selfTest.cpuRegisterTestTimerW = CPU_REGISTER_TEST_INTERVAL;

    if ( CLASSB_CPURegistersTest() != CLASSB_TEST_PASS) {
      faultIndication.Error(CPU_REGISTER_TEST_ERROR);
    }
  }

  // Execute CPU Program counter test
  if ( --selfTest.pcTestTimerW == 0) {
    selfTest.pcTestTimerW = PC_REGISTER_TEST_INTERVAL;

    CLASSB_CPUPCTest();
    if ( CLASSB_CPUPCTestGetResult()) {
      faultIndication.Error(PROGRAM_COUNTER_TEST_ERROR);
    }
  }

  // Execute RAM Checker Board for entire RAM region
  if ( --selfTest.ramTestTimerW == 0) {
    selfTest.ramTestTimerW = RAM_TEST_INTERVAL;

    RAMTest();
  }

  // Execute CRC test for code flash region
  if ( --selfTest.flashCRCTestTimerW == 0) {
    selfTest.flashCRCTestTimerW = FLASH_TEST_INTERVAL;

    CodeFlashTest();
  }

  // Execute EEPROM CRC test
  if ( --selfTest.eepromCRCTestTimerW == 0) {
    selfTest.eepromCRCTestTimerW = EEPROM_TEST_INTERVAL;

    if(nonVol.ValidateCRC() != true) {
      faultIndication.Error(EEPROM_CRC_TEST_ERROR);
    }
  }

  // Execute the clock test
  if ( --selfTest.clcokTestTimerW == 0) {
    selfTest.clcokTestTimerW = CLOCK_TEST_INTERVAL;

    // Execute the test only when heater is in OFF state
    if ( (tempControl.relayStatus != RELAY_CONTROL_CONTROL)             \
            && (tempControl.relayStatus != RELAY_CONTROL_SHUTDOWN)      \
            && (tempControl.relayStatus != RELAY_CONTROL_STBYHEAT)      \
            && (tempControl.relayStatus != RELAY_CONTROL_ERROR)) {
      // This routine will take around 20 msec to complete the test
      // Stop the timer, reset it and then start
      RESET_RESTART_INTR_MONITOR_TIMER();
      selfTest.flags.ignoreINTRTestFLG = 1;

      ENABLE_TIMER_FOR_CLOCK_TEST();

      clockTestResult = CLASSB_ClockTest(CPU_CLOCK_FREQUENCY_IN_HZ,       \
              REF_CLK_FREQUENCY_IN_HZ, CLOCK_TEST_TOLERANCE);

      if ( clockTestResult != CLASSB_TEST_PASS) {
//        faultIndication.Error(CLOCK_TEST_ERROR);
      }

      DISABLE_TIMER_AFTER_CLOCK_TEST();
    }
  }
  return TASK_COMPLETED;
}

