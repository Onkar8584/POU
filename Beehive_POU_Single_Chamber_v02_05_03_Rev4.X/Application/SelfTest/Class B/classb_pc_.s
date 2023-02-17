/*************************************************************************
 *  © 2014 Microchip Technology Inc.                                       
 *  
 *  Project Name:    Class B Library
 *  Version:         2.4 
 *  FileName:        classb_pc_.s
 *  Dependencies:    classb_config.h
 *  Processor:       PIC24, dsPIC
 *  Compiler:        XC16
 *  IDE:             MPLAB® X                        
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *  Description:     This file contains functions to check program counter.
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

.include "classb_config.h"

.if   (CLASSB_DEVICE_FLASH_SIZE_KB == 4)

.equ PC_TEST_FUNCTION5555_ADDRESS, 0x00000554
.equ PC_TEST_FUNCTIONAAAA_ADDRESS, 0x000004aa

.elseif (CLASSB_DEVICE_FLASH_SIZE_KB == 6)

.equ PC_TEST_FUNCTION5555_ADDRESS, 0x00000554
.equ PC_TEST_FUNCTIONAAAA_ADDRESS, 0x00000aaa

.elseif (CLASSB_DEVICE_FLASH_SIZE_KB == 8)

.equ PC_TEST_FUNCTION5555_ADDRESS, 0x00000554
.equ PC_TEST_FUNCTIONAAAA_ADDRESS, 0x00000aaa

.elseif (CLASSB_DEVICE_FLASH_SIZE_KB == 12)

.equ PC_TEST_FUNCTION5555_ADDRESS, 0x00001554
.equ PC_TEST_FUNCTIONAAAA_ADDRESS, 0x00000aaa

.elseif (CLASSB_DEVICE_FLASH_SIZE_KB == 16)

.equ PC_TEST_FUNCTION5555_ADDRESS, 0x00001554
.equ PC_TEST_FUNCTIONAAAA_ADDRESS, 0x00000aaa

.elseif (CLASSB_DEVICE_FLASH_SIZE_KB == 24)

.equ PC_TEST_FUNCTION5555_ADDRESS, 0x00001554
.equ PC_TEST_FUNCTIONAAAA_ADDRESS, 0x00002aaa

.elseif (CLASSB_DEVICE_FLASH_SIZE_KB == 32)

.equ PC_TEST_FUNCTION5555_ADDRESS, 0x00001554
.equ PC_TEST_FUNCTIONAAAA_ADDRESS, 0x00002aaa

.elseif (CLASSB_DEVICE_FLASH_SIZE_KB == 48)

.equ PC_TEST_FUNCTION5555_ADDRESS, 0x00005554
.equ PC_TEST_FUNCTIONAAAA_ADDRESS, 0x00002aaa

.elseif (CLASSB_DEVICE_FLASH_SIZE_KB == 64)

.equ PC_TEST_FUNCTION5555_ADDRESS, 0x00005554
.equ PC_TEST_FUNCTIONAAAA_ADDRESS, 0x00002aaa

.elseif (CLASSB_DEVICE_FLASH_SIZE_KB == 96)

.equ PC_TEST_FUNCTION5555_ADDRESS, 0x00005554
.equ PC_TEST_FUNCTIONAAAA_ADDRESS, 0x0000aaaa

.elseif (CLASSB_DEVICE_FLASH_SIZE_KB == 128)

.equ PC_TEST_FUNCTION5555_ADDRESS, 0x00005554
.equ PC_TEST_FUNCTIONAAAA_ADDRESS, 0x0000aaaa

.elseif (CLASSB_DEVICE_FLASH_SIZE_KB == 256)

.equ PC_TEST_FUNCTION5555_ADDRESS, 0x00015554
.equ PC_TEST_FUNCTIONAAAA_ADDRESS, 0x0000aaaa

.elseif (CLASSB_DEVICE_FLASH_SIZE_KB == 512)

.equ PC_TEST_FUNCTION5555_ADDRESS, 0x00015554
.equ PC_TEST_FUNCTIONAAAA_ADDRESS, 0x0002aaaa

.else

.error "Valid CLASSB_DEVICE_FLASH_SIZE_KB must be defined in classb_config.h"

.endif


.global _pcErrorFlag
.global _CLASSB_PCTestFunction5555
.global _CLASSB_PCTestFunctionAAAA

.bss _pcErrorFlag, 2  ; program counter error flag

.section function5555, code, address(PC_TEST_FUNCTION5555_ADDRESS-2)

    return ; guard return instruction
_CLASSB_PCTestFunction5555:
    dec [w0], [w0]
    return

.section functionAAAA, code, address(PC_TEST_FUNCTIONAAAA_ADDRESS-2)

    return ; guard return instruction
_CLASSB_PCTestFunctionAAAA:
    dec [w0], [w0]
    return

.end
