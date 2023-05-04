/*************************************************************************
 *  © 2014 Microchip Technology Inc.                                       
 *  
 *  Project Name:    Class B Library
 *  Version:         2.4 
 *  FileName:        classb_registers_.s
 *  Dependencies:    None
 *  Processor:       PIC24, dsPIC
 *  Compiler:        XC16
 *  IDE:             MPLAB® X                        
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *  Description:     This file contains functions to check MCU registers.
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
 

.macro disable_interrupts save_reg
 mov  DISICNT, \save_reg
 disi #0x3fff
.endm

.macro enable_interrupts restore_reg
 mov \restore_reg, DISICNT
 mov  w0, WREG0
.endm

 .macro check_register check_reg, value
 mov \check_reg, w1 
 mov #\value, \check_reg
 mov #\value, w0
 cp \check_reg, w0
 mov w1, \check_reg
 bra nz, Error
 .endm

 .macro check_sfr sfr, value
 mov \sfr, w1 
 mov #\value, w0
 mov w0, \sfr
 nop
 cp \sfr
 mov w1, \sfr
 bra nz, Error
 .endm

 .macro check_sfr_bytewise sfr,value
 mov \sfr, w1 
 mov #\value,w0
 mov w0,\sfr
 nop
 cp.b \sfr
 mov w1, \sfr
 bra nz,Error
 .endm

.global _CLASSB_CPURegistersTest

.section .text

_CLASSB_CPURegistersTest:

;*********************************
; Test WREG0 && WREG1 registers
   mov #0xAAAA, w0
   mov #0xAAAA, w1
   cp  w0, w1
   bra nz, Error

   mov #0x5555, w0
   mov #0x5555, w1
   cp  w0, w1
   bra nz, Error

;*********************************
; Test WREG2 register
   check_register w2 0xAAAA
   check_register w2 0x5555
   
;*********************************
; Test WREG3 register
   check_register w3 0xAAAA
   check_register w3 0x5555

;*********************************
; Test WREG4 register
   check_register w4 0xAAAA
   check_register w4 0x5555

;*********************************
; Test WREG5 register
   check_register w5 0xAAAA
   check_register w5 0x5555

;*********************************
; Test WREG6 register
   check_register w6 0xAAAA
   check_register w6 0x5555

;*********************************
; Test WREG7 register
   check_register w7 0xAAAA
   check_register w7 0x5555

;*********************************
; Disable interrupts
   disable_interrupts w7

;*********************************
; Test SPLIM register
   mov w15,w2
   mov SPLIM,w3

   clr w15
   check_sfr SPLIM 0xAAAA
   check_sfr SPLIM 0x5554

;*********************************
; Test WREG15
   mov #0xFFFE, w0
   mov w0, SPLIM

   check_register w15 0xAAAA
   check_register w15 0x5554

   mov w2,w15
   mov w3,SPLIM

;*********************************
; Restore interrupts
   enable_interrupts w7

;*********************************
; Test WREG8 register
   check_register w8 0xAAAA
   check_register w8 0x5555

;*********************************
; Test WREG9 register 
   check_register w9 0xAAAA
   check_register w9 0x5555

;*********************************
; Test WREG10 register
   check_register w10 0xAAAA
   check_register w10 0x5555

;*********************************
; Test WREG11 register
   check_register w11 0xAAAA
   check_register w11 0x5555

;*********************************
; Test WREG12 register
   check_register w12 0xAAAA
   check_register w12 0x5555

;*********************************
; Test WREG13 register
   check_register w13 0xAAAA
   check_register w13 0x5555

;*********************************
; Test WREG14 register
   check_register w14 0xAAAA
   check_register w14 0x5555

;*********************************
; Test TBLPAG register
   check_sfr_bytewise TBLPAG 0xAA
   check_sfr_bytewise TBLPAG 0x55

;*********************************
; Test DSRPAG register
.ifdecl DSRPAG
   check_sfr_bytewise DSRPAG 0xAA
   check_sfr_bytewise DSRPAG 0x55
.endif

;*********************************
; Test DSWPAG register
.ifdecl DSWPAG 
   check_sfr_bytewise DSWPAG 0xAA
   check_sfr_bytewise DSWPAG 0x55
.endif

;*********************************
; Test PSVPAG register
.ifdecl PSVPAG
   check_sfr_bytewise PSVPAG 0xAA
   check_sfr_bytewise PSVPAG 0x55
.endif
   
;*********************************
; Test RCOUNT register
   check_sfr RCOUNT 0x2AAA
   check_sfr RCOUNT 0x1555

;*********************************
; Test ACCAL register
.ifdecl ACCAL
   check_sfr ACCAL 0xAAAA
   check_sfr ACCAL 0x5555
.endif

;*********************************
; Test ACCAH register
.ifdecl ACCAH
   check_sfr ACCAH 0xAAAA
   check_sfr ACCAH 0x5555
.endif

;*********************************
; Test ACCAU register
.ifdecl ACCAU 
   check_sfr_bytewise ACCAU 0xAA
   check_sfr_bytewise ACCAU 0x55
.endif

;*********************************
; Test ACCBL register
.ifdecl ACCBL 
   check_sfr ACCBL 0xAAAA
   check_sfr ACCBL 0x5555
.endif

;*********************************
; Test ACCBH register
.ifdecl ACCBH 
   check_sfr ACCBH 0xAAAA
   check_sfr ACCBH 0x5555
.endif

;*********************************
; Test ACCBU register
.ifdecl ACCBU 
   check_sfr_bytewise ACCBU 0xAA
   check_sfr_bytewise ACCBU 0x55
.endif

;*********************************
; Test DCOUNT register
.ifdecl DCOUNT 
   check_sfr DCOUNT 0x2AAA
   check_sfr DCOUNT 0x1555
.endif

;*********************************
; Test DOSTARTL register
.ifdecl DOSTARTL 
   check_sfr DOSTARTL 0xAAAA
   check_sfr DOSTARTL 0x5554
.endif

;*********************************
; Test DOENDL register
.ifdecl DOENDL 
   check_sfr DOENDL 0xAAAA
   check_sfr DOENDL 0x5554
.endif

;*********************************
; Test DOSTARTH register
.ifdecl DOSTARTH 
   check_sfr_bytewise DOSTARTH 0x2A
   check_sfr_bytewise DOSTARTH 0x15
.endif

;*********************************
; Test DOENDH register
.ifdecl DOENDH 
   check_sfr_bytewise DOENDH 0x2A
   check_sfr_bytewise DOENDH 0x15
.endif

;*********************************
; Test MODCON register
.ifdecl MODCON 
   check_sfr MODCON 0x8AAA
   check_sfr MODCON 0x4555
.endif

;*********************************
; Test XMODSRT register
.ifdecl XMODSRT 
   check_sfr XMODSRT 0xAAAA
   check_sfr XMODSRT 0x5554
.endif

;*********************************
; Test XMODEND register
.ifdecl XMODEND 
   check_sfr XMODEND 0xAAAB
   check_sfr XMODEND 0x5555
.endif

;*********************************
; Test YMODSRT register
.ifdecl YMODSRT 
   check_sfr YMODSRT 0xAAAA
   check_sfr YMODSRT 0x5554
.endif

;*********************************
; Test YMODEND register
.ifdecl YMODEND 
   check_sfr YMODEND 0xAAAB
   check_sfr YMODEND 0x5555
.endif

;*********************************
; Test XBREV register
.ifdecl XBREV 
   check_sfr XBREV 0xAAAA
   check_sfr XBREV 0x5555
.endif

   clr w0
   return

Error:
   mov #0xffff, w0

   return

.end
