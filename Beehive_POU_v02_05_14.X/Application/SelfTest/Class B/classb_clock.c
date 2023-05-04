/*************************************************************************
 *  © 2014 Microchip Technology Inc.                                       
 *  
 *  Project Name:    Class B Library
 *  Version:         2.4 
 *  FileName:        classb_clock.c
 *  Dependencies:    classb.h
 *  Processor:       PIC24, dsPIC
 *  Compiler:        XC16
 *  IDE:             MPLAB® X                        
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *  Description:     This file contains functions to check the MCU clock 
 *                   source.
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

#include "classb.h"

#include "classb_config.h"

extern uint16_t ClockTest(uint16_t timerCount, volatile unsigned int* timerAddress);

CLASSBRESULT CLASSB_ClockTest(uint32_t clockFrequency, uint32_t referenceFrequency, uint16_t tolerance)
{
volatile uint32_t expectedCount;
volatile uint32_t toleranceCount;
uint16_t timerCount;
uint32_t count;

    timerCount = ((uint32_t)CLASSB_CLOCK_TEST_TIME_MS*referenceFrequency)/1000;
    if(timerCount == 0)
    {
        timerCount = 1;
    }

    expectedCount  = (uint32_t)(((uint64_t)clockFrequency*timerCount)/(10*referenceFrequency)); // one count = 10 cycles
    toleranceCount = (expectedCount*tolerance)/1000;

    // measure number of instruction cycles during the test time
    // one count = 10 cycles
    count = (uint32_t)ClockTest(timerCount, CLASSB_CLOCK_TEST_TIMER_ADDRESS);

    if(count == 0)
    {
        return CLASSB_TEST_TIMEOUT;
    }

    if(count < (expectedCount - toleranceCount))
    {
        return CLASSB_TEST_FAIL;
    }

    if(count > (expectedCount + toleranceCount))
    {
        return CLASSB_TEST_FAIL;
    }

    return CLASSB_TEST_PASS;
}
