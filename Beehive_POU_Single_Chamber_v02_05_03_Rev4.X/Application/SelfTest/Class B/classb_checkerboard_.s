/*************************************************************************
 *  © 2014 Microchip Technology Inc.                                       
 *  
 *  Project Name:    Class B Library
 *  Version:         2.4 
 *  FileName:        classb_checkerboard_.s
 *  Dependencies:    None
 *  Processor:       PIC24, dsPIC
 *  Compiler:        XC16
 *  IDE:             MPLAB® X                        
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *  Description:     This file contains functions for the RAM checker board
 *                   test.
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

.global _CLASSB_RAMCheckerboardTestF

.section .text

_CLASSB_RAMCheckerboardTestF:
; w0 - start address
; w1 - length in bytes

; W2 - 0x5555
; W3 - 0xAAAA
; w4 - 1st memory value to restore
; W5 - 2nd memory value to restore
; W6 - interrupt status to restore
; W7 - temp

    lsr  w1, #2, w1
    cp0  w1 
    bra  nz, NonZeroLength
    clr  w0
    return

NonZeroLength:
    mov #0x5555, w2
    mov #0xAAAA, w3
    mov DISICNT, w6

Loop:
    disi #64         ; disable interrupts

    mov  [w0], w4    ; save 1st memory value
    mov  [w0+2], w5  ; save 2nd memory value

    mov  w2, [w0]    ; write 0x5555
    mov  w3, [w0+2]  ; write 0xAAAA

    mov  [w0], w7    ; read  0x5555
    cp   w2, w7      ; compare
    bra  nz, Error

    mov  [w0+2], w7  ; read  0xAAAA
    cp   w3, w7      ; compare
    bra  nz, Error

    mov  w3, [w0]    ; write 0xAAAA
    mov  w2, [w0+2]  ; write 0x5555

    mov  [w0], w7    ; read  0xAAAA
    cp   w3, w7      ; compare
    bra  nz, Error

    mov  [w0+2], w7  ; read  0x5555
    cp   w2, w7      ; compare
    bra  nz, Error

    mov  w4, [w0]    ; restore 1st memory value
    mov  w5, [w0+2]  ; restore 2nd memory value

    mov  w6, DISICNT ; restore interrupts
    mov  w0, WREG0   ; DISICNT BUG FIX

    add  w0, #4, w0  ; move to the next location
    dec  w1, w1   
    bra  nz, Loop

    clr w0
    return

Error:
    mov  w4, [w0]    ; restore 1st memory value
    mov  w5, [w0+2]  ; restore 2nd memory value

    mov #1, w0
    return  

.end
