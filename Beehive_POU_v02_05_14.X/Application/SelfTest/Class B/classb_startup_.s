/*************************************************************************
 *  © 2014 Microchip Technology Inc.                                       
 *  
 *  Project Name:    Class B Library
 *  Version:         2.4 
 *  FileName:        classb_startup_.s
 *  Dependencies:    classb_config.h
 *  Processor:       PIC24, dsPIC
 *  Compiler:        XC16
 *  IDE:             MPLAB® X                        
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *  Description:     This file performs march tests of entire RAM before
 *                   main() function entry.   
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

.global __user_init
.global __has_user_init
.equ    __has_user_init, 1

.global StartupMarchCReturn
.global StartupMarchBReturn

.global _startupMarchTestErrorFlag

.section _startupErrorFlagSection, bss, noload, align(2)
_startupMarchTestErrorFlag:
.space  2

.section .text
__user_init:

    ; initialize error flag
    clr w0
    mov #_startupMarchTestErrorFlag, w1
    mov w0, [w1]

; MARCH C
.if (CLASSB_MARCH_C_STARTUP || CLASSB_MARCH_C_MINUS_STARTUP)

    mov  #__DATA_BASE+#0x0050, w0   ; start address
    mov  #__DATA_LENGTH-#0x0050, w1 ; length
    clr  w2                 ; no buffer

    ; bit 15 indicates startup test
    ; bit 0 indicates minus test 
.if  CLASSB_MARCH_C_MINUS_STARTUP
    mov  #0x8001, w3
.else
    mov  #0x8000, w3
.endif
    ; run march C test
    goto _CLASSB_RAMMarchCTestF
StartupMarchCReturn:
    cp0  w0
    bra  nz, Done
.else
StartupMarchCReturn:
.endif

; MARCH B
.if CLASSB_MARCH_B_STARTUP
    mov  #__DATA_BASE+#0x0050, w0   ; start address
    mov  #__DATA_LENGTH-#0x0050, w1 ; length
    clr  w2                 ; no buffer
    ; bit 15 indicates startup test
    mov  #0x8000, w3
    ; run march B test
    goto _CLASSB_RAMMarchBTestF
StartupMarchBReturn:
.else
StartupMarchBReturn:
    clr  w0
.endif

Done:
    ; save result
    mov #_startupMarchTestErrorFlag, w1
    mov w0, [w1]

.if (CLASSB_MARCH_C_STARTUP || CLASSB_MARCH_C_MINUS_STARTUP || CLASSB_MARCH_B_STARTUP)

    ; restore initialized data
    mov   #__dinit_tbloffset,w0 ; w0,w1 = template
    mov   #__dinit_tblpage,w1   ;
    
.ifdef __dsPIC33E
    call  __data_init_extended       ; initialize data
.else
.ifdef __PIC24E
    call  __data_init_extended       ; initialize data
.else
    call  __data_init       ; initialize data
.endif
.endif    

.endif

    call  _main                 ; call user's main()
    .pword 0xDA4000             ; halt the simulator
    reset                       ; reset the processor

.end
