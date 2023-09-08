/*************************************************************************
 *  © 2014 Microchip Technology Inc.                                       
 *  
 *  Project Name:    Class B Library
 *  Version:         2.4 
 *  FileName:        classb.h
 *  Dependencies:    xc.h, stdint.h
 *  Processor:       PIC24, dsPIC
 *  Compiler:        XC16
 *  IDE:             MPLAB® X                        
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *  Description:     This file contains declarations for the class B functions.
 *
 *************************************************************************/
/**************************************************************************
 * MICROCHIP SOFTWARE NOTICE AND DISCLAIMER: You may use this software, and 
 * any derivatives created by any person or entity by or on your behalf, 
 * exclusively with Microchip's products in accordance with applicable
 * software license terms and conditions, a copy of which is provided for
 * your referencein accompanying documentation. Microchip and its licensors 
 * retain all ownership and intellectual property rights in the 
 * accompanying software and in all derivatives hereto. 
 * 
 * This software and any accompanying information is for suggestion only. 
 * It does not modify Microchip's standard warranty for its products. You 
 * agree that you are solely responsible for testing the software and 
 * determining its suitability. Microchip has no obligation to modify, 
 * test, certify, or support the software. 
 * 
 * THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS". NO WARRANTIES, WHETHER 
 * EXPRESS, IMPLIED OR STATUTORY, INCLUDING, BUT NOT LIMITED TO, IMPLIED 
 * WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS FOR A 
 * PARTICULAR PURPOSE APPLY TO THIS SOFTWARE, ITS INTERACTION WITH 
 * MICROCHIP'S PRODUCTS, COMBINATION WITH ANY OTHER PRODUCTS, OR USE IN ANY 
 * APPLICATION. 
 * 
 * IN NO EVENT, WILL MICROCHIP BE LIABLE, WHETHER IN CONTRACT, WARRANTY, 
 * TORT (INCLUDING NEGLIGENCE OR BREACH OF STATUTORY DUTY), STRICT 
 * LIABILITY, INDEMNITY, CONTRIBUTION, OR OTHERWISE, FOR ANY INDIRECT, 
 * SPECIAL, PUNITIVE, EXEMPLARY, INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, 
 * FOR COST OR EXPENSE OF ANY KIND WHATSOEVER RELATED TO THE SOFTWARE, 
 * HOWSOEVER CAUSED, EVEN IF MICROCHIP HAS BEEN ADVISED OF THE POSSIBILITY 
 * OR THE DAMAGES ARE FORESEEABLE. TO THE FULLEST EXTENT ALLOWABLE BY LAW, 
 * MICROCHIP'S TOTAL LIABILITY ON ALL CLAIMS IN ANY WAY RELATED TO THIS 
 * SOFTWARE WILL NOT EXCEED THE AMOUNT OF FEES, IF ANY, THAT YOU HAVE PAID 
 * DIRECTLY TO MICROCHIP FOR THIS SOFTWARE. 
 * 
 * MICROCHIP PROVIDES THIS SOFTWARE CONDITIONALLY UPON YOUR ACCEPTANCE OF 
 * THESE TERMS. 
 *************************************************************************/

#ifndef _CLASSB_H_
#define _CLASSB_H_

#include <xc.h>
#include <stdint.h>
#include <stdbool.h>

/****************************************************************************
  Enumeration:
    CLASSBRESULT

  Description:
    This enumeration is used by the class B test functions to return the results:
 
    CLASSB_TEST_PASS    - the test is finished successfully,
    CLASSB_TEST_FAIL    - the test is failed,
    CLASSB_TEST_TIMEOUT - the test is failed because a timeout was detected.

  ***************************************************************************/
typedef enum _CLASSBRESULT_
{
  CLASSB_TEST_PASS = 0,
  CLASSB_TEST_FAIL,
  CLASSB_TEST_TIMEOUT
} CLASSBRESULT; 

/****************************************************************************
  Macro:
    CLASSBRESULT CLASSB_RAMMarchCTest(startAddress, length, bufferAddress, minus)

  Summary:
    Tests RAM using march C or march C minus algorithms.

  Description:
    Tests RAM using march C or march C minus algorithms.
 
  Precondition:
    None.

  Parameters:
    startAddress  - the first address of the tested memory,
    length        - the byte length of the tested memory,
    bufferAddress - the first address of the buffer to store the memory
                    content, if it is NULL then the test will clear the 
                    tested memory,
    minus         - if the parameter is TRUE the "minus" algorithm is used.

  Returns:
    Returns zero if successful. Non zero means - failed.

  Remarks:
    Interrupts are disabled during this test.
    The address and length must be even (aligned by 2 bytes).
  ***************************************************************************/
CLASSBRESULT CLASSB_RAMMarchCTestF(uint16_t* startAddress, uint16_t length, uint16_t* bufferAddress, uint16_t minus);
#define CLASSB_RAMMarchCTest(startAddress, length, bufferAddress, minus)  CLASSB_RAMMarchCTestF((uint16_t*)(startAddress), (uint16_t)(length), (uint16_t*)(bufferAddress), (uint16_t)minus)

/****************************************************************************
  Macro:
    CLASSBRESULT CLASSB_RAMMarchBTest(startAddress, length, bufferAddress)

  Summary:
    Tests RAM using march B algorithm.

  Description:
    Tests RAM using march B algorithm.
 
  Precondition:
    None.

  Parameters:
    startAddress  - the first address of the tested memory,
    length        - the byte length of the tested memory,
    bufferAddress - the first address of the buffer to store the memory
                    content, if it is NULL then the test will clear the 
                    tested memory.

  Returns:
    Returns zero if successful. Non zero means - failed.

  Remarks:
    Interrupts are disabled during this test.
    The address and length must be even (aligned by 2 bytes).
  ***************************************************************************/
CLASSBRESULT CLASSB_RAMMarchBTestF(uint16_t* startAddress, uint16_t length, uint16_t* bufferAddress, uint16_t mustbezero);
#define CLASSB_RAMMarchBTest(startAddress, length, bufferAddress)  CLASSB_RAMMarchBTestF((uint16_t*)(startAddress), (uint16_t)(length), (uint16_t*)(bufferAddress), (uint16_t)0)


/****************************************************************************
  Macro:
    CLASSBRESULT CLASSB_StartupMarchTestGetResult()

  Summary:
    Returns the result of the startup march tests.

  Description:
    Returns the result of the startup march tests. These tests are run
    before main() function entry if  CLASSB_MARCH_C_STARTUP,
    CLASSB_MARCH_C_MINUS_STARTUP or/and CLASSB_MARCH_B_STARTUP flags are 
    not zero in classb_config.h file.
 
  Precondition:
    CLASSB_MARCH_C_STARTUP, CLASSB_MARCH_C_MINUS_STARTUP or/and
    CLASSB_MARCH_B_STARTUP flags must be set in classb_config.h file.

  Parameters:
    None.

  Returns:
    Returns zero if successful. Non zero means - failed.

  Remarks:
    Entire memory march tests can take a while. Watchdog timer should provide 
    enough time to complete the tests. It is not possible to debug the startup
    march test code portion because these tests erase the debugger data in the memory.
  ***************************************************************************/
extern uint16_t startupMarchTestErrorFlag;
#define CLASSB_StartupMarchTestGetResult() (startupMarchTestErrorFlag != 0)


/****************************************************************************
  Macro:
    CLASSBRESULT CLASSB_RAMCheckerboardTest(startAddress, length)

  Summary:
    Tests RAM region using checker board pattern.

  Description:
    Tests RAM region using checker board pattern.
 
  Precondition:
    None.

  Parameters:
    startAddress - the first address of the tested memory,
    length       - the byte length of the tested memory.

  Returns:
    Returns zero if successful. Non zero means - failed.

  Remarks:
    The address must be even (aligned by 2 bytes).
    The length must be aligned by 4 bytes.
  ***************************************************************************/
CLASSBRESULT CLASSB_RAMCheckerboardTestF(uint16_t* startAddress, uint16_t length);
#define CLASSB_RAMCheckerboardTest(startAddress, length) CLASSB_RAMCheckerboardTestF((uint16_t*)(startAddress), (uint16_t)(length))

/****************************************************************************
  Function:
    CLASSBRESULT CLASSB_CPURegistersTest()

  Summary:
    Tests CPU registers.

  Description:
    Tests CPU registers.
 
  Precondition:
    None.

  Parameters:
    None.

  Returns:
    Returns zero if successful. Non zero means - failed.

  Remarks:
    None.
  ***************************************************************************/
CLASSBRESULT CLASSB_CPURegistersTest();

/****************************************************************************
  Macro:
    CLASSB_CPUPCTest()

  Summary:
    Tests CPU program counter.

  Description:
    Tests CPU program counter. CLASSB_CPUPCTestGetResult() should be used to get
    the result of the test.    
 
  Precondition:
    None.

  Parameters:
    None.

  Returns:
    None.

  Remarks:
    The device flash memory size must be set using parameter
    CLASSB_DEVICE_FLASH_SIZE in classb_conig.h.
  ***************************************************************************/
// PC counter test error flag.
extern uint16_t pcErrorFlag;
// Pointer to test function with 5555 pattern decrementing the error flag.
extern void CLASSB_PCTestFunction5555(uint16_t* pcErrorFlag);
// Pointer to test function with AAAA pattern decrementing the error flag.
extern void CLASSB_PCTestFunctionAAAA(uint16_t* pcErrorFlag);

#define CLASSB_CPUPCTest()\
{\
volatile uint16_t disicnt;\
disicnt = DISICNT;\
asm volatile ("disi #0x3fff");\
pcErrorFlag++;\
CLASSB_PCTestFunction5555(&pcErrorFlag);\
pcErrorFlag++;\
CLASSB_PCTestFunctionAAAA(&pcErrorFlag);\
DISICNT = disicnt;\
asm volatile ("mov  w0, WREG0");\
}


/****************************************************************************
  Macro:
    CLASSBRESULT CLASSB_CPUPCTestGetResult()

  Summary:
    Returns the result of the CPU program counter test.

  Description:
    Returns the result of the CPU program counter test done with CLASSB_CPUPCTest()    
 
  Precondition:
    None.

  Parameters:
    None.

  Returns:
    Returns zero if successful. Non zero means - failed.

  Remarks:
    None.
  ***************************************************************************/
#define CLASSB_CPUPCTestGetResult() (pcErrorFlag != 0)

/****************************************************************************
  Macro:
    uint16_t CLASSB_CRCFlashTest(startAddress, length, crcSeed)

  Summary:
    The function calculates the CRC check sum for the flash memory region.

  Description:
    The function calculates the CRC check sum for the flash memory region.
 
  Precondition:
    None.

  Parameters:
    startAddress - the first address of the tested flash memory in bytes,
    length       - the length address of the tested flash memory in PC units,
    crcSeed      - initial value of the CRC check sum.

  Returns:
    The function returns the standard 16-bit CRC.

  Remarks:
    The start address and length must be even (aligned by 2).
    "length" must be specified in PC units (PC units are used in map files).
  ***************************************************************************/
uint16_t  CLASSB_CRCFlashTestF(__prog__ uint16_t* startAddress, uint32_t length, uint16_t crcSeed);
#define   CLASSB_CRCFlashTest(startAddress, length, crcSeed) CLASSB_CRCFlashTestF((__prog__ uint16_t*)(startAddress), (uint32_t)(length), (uint16_t)(crcSeed))

/****************************************************************************
  Macro:
    uint16_t  CLASSB_CRCEEPROMTest(startAddress, length, crcSeed)

  Summary:
    The function calculates the CRC check sum for the EEPROM memory region.

  Description:
    The function calculates the CRC check sum for the EEPROM memory region.
 
  Precondition:
    None.

  Parameters:
    startAddress - the first address of the tested EEPROM memory in bytes,
    length       - the length address of the tested EEPROM memory,
    crcSeed      - initial value of the CRC check sum.

  Returns:
    The function returns the standard 16-bit CRC.

  Remarks:
    The start address and length must be even (aligned by 2).
  ***************************************************************************/
uint16_t  CLASSB_CRCEEPROMTestF(__prog__ uint16_t* startAddress, uint32_t length, uint16_t crcSeed);
#define   CLASSB_CRCEEPROMTest(startAddress, length, crcSeed) CLASSB_CRCEEPROMTestF((__prog__ uint16_t*)(startAddress), (uint32_t)(length), (uint16_t)(crcSeed))

/****************************************************************************
  Function:
    CLASSBRESULT CLASSB_ClockTest(uint32_t clockFrequency,
                                  uint32_t referenceFrequency,
                                  uint16_t tolerance)
  Summary:
    Tests the CPU clock source.

  Description:
    Tests the CPU clock source.The reference
    clock should be connected to the timer.The timer must be
    initialized by the application code to count the reference
    clock pulses. The period register must be set to the maximum
    value. The address of the timer counter register must
    be specicied in classb_config.h using the compile time
    option CLASSB_CLOCK_TEST_TIMER_ADDRESS. There's a second
    compile option  CLASSB_CLOCK_TEST_TIME_MS in the 
    classb_config.h. It defines the test time and can be about
    20 mS if clock is between 1-32MHz and the reference clock
    is 50Hz-33kHz. 
 
  Precondition:
    None.

  Parameters:
    clockFrequency     - frequency of the clock source,
    referenceFrequency - frequency of the reference clock
                         (such as power line or secondary oscillator),
    tolerance          - maximum valid frequency tolerance, can be from
                         1(0.1%) to 100(10%).

  Returns:
    Returns zero if successful. Non zero means - failed. See CLASSBRESULT
    enumeration for details.

  Remarks:
    Interrupts are disabled during this test.
  ***************************************************************************/
CLASSBRESULT CLASSB_ClockTest(uint32_t clockFrequency, uint32_t referenceFrequency, uint16_t tolerance);

#endif
