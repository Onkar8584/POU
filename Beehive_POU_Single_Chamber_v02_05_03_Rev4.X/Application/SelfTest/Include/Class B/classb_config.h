/*************************************************************************
 *  © 2013 Microchip Technology Inc.                                       
 *  
 *  Project Name:    Class B Library
 *  FileName:        classb_config.h
 *  Dependencies:    None
 *  Processor:       PIC24, dsPIC
 *  Compiler:        XC16
 *  IDE:             MPLAB® X                        
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *  Description:     Configuration file for Class B library.
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

#ifndef __C30__

/****************************************************************************
  Startup march C test enable flag. If this option is not zero, entire RAM
  will be tested using the march C test before main() function entry.
  ***************************************************************************/
.equ    CLASSB_MARCH_C_STARTUP,         0

/****************************************************************************
  Startup march C minus test enable flag. If this option is not zero, entire RAM
  will be tested using the march C minus test before main() function entry.
  ***************************************************************************/
.equ    CLASSB_MARCH_C_MINUS_STARTUP,   1

/****************************************************************************
  Startup march B test enable flag. If this option is not zero, entire RAM
  will be tested using the march B test before main() function entry.
  ***************************************************************************/
.equ    CLASSB_MARCH_B_STARTUP,         1

/****************************************************************************
  Device flash memory size in KBytes. This option is used by the program
  counter test.
  ***************************************************************************/
.equ    CLASSB_DEVICE_FLASH_SIZE_KB,    64

#else

/****************************************************************************
  Clock test time in mS. Can be about 20 mS if clock is between 1-32MHz and
  the reference clock is 50Hz-33kHz.
  ***************************************************************************/
#define CLASSB_CLOCK_TEST_TIME_MS       20

/****************************************************************************
  Clock test hardware timer SFR address. The timer is used to count
  the reference clock pulses.
  ***************************************************************************/
#define CLASSB_CLOCK_TEST_TIMER_ADDRESS (&TMR2)


#endif
