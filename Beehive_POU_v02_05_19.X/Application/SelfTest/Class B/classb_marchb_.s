/*************************************************************************
 *  © 2014 Microchip Technology Inc.                                       
 *  
 *  Project Name:    Class B Library
 *  Version:         2.4 
 *  FileName:        classb_marchb_.s
 *  Dependencies:    None
 *  Processor:       PIC24, dsPIC
 *  Compiler:        XC16
 *  IDE:             MPLAB® X                        
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *  Description:     This file contains functions for march B test.
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
.global Rd0Wr1Rd1Wr0Rd0Wr1Up
.global Rd1Wr0Wr1Up
.global Rd1Wr0Wr1Wr0Down
.global Rd0Wr1Wr0Down
.global _CLASSB_RAMMarchBTestF

.section .text

;------------------------------------------------------------------------
_CLASSB_RAMMarchBTestF:

   lsr  w1, #1, w1
   cp0  w1 
   bra  nz, NonZeroLength
   clr  w0
   return

NonZeroLength:

   mov  w0, w4   ; start address
   mov  w1, w5   ; length in 16-bit words
   mov  w2, w6   ; buffer address
   mov  w3, w7   ; startup flag

   ; save interrupts status
   mov  DISICNT, w3

   ; save data in buffer
   mov  w4, w0 ; address
   mov  w5, w1 ; length 
   mov  w6, w2 ; buffer address
   cp0  w2
   bra  z, SkipSave
SaveLoop:
   disi #0x3fff
   mov [w0++], [w2++]
   dec  w1, w1  
   bra  nz, SaveLoop
SkipSave:

   ; fill memory with zeros
   mov  w4, w0 ; address
   mov  w5, w1 ; length 
FillZeroLoop:
   disi #0x3fff
   clr [w0]
   inc2 w0, w0
   dec  w1, w1  
   bra  nz, FillZeroLoop

   ; Rd0Wr1Rd1Wr0Rd0Wr1Up Test
   mov  w4, w0 ; address
   mov  w5, w1 ; length   
   goto Rd0Wr1Rd1Wr0Rd0Wr1Up
Rd0Wr1Rd1Wr0Rd0Wr1UpReturn:

   ; Rd1Wr0Wr1Up Test
   mov  w4, w0 ; address
   mov  w5, w1 ; length   
   goto Rd1Wr0Wr1Up
Rd1Wr0Wr1UpReturn:

   ; Rd1Wr0Wr1Wr0Down Test
   mov  w4, w0 ; address
   mov  w5, w1 ; length   
   goto Rd1Wr0Wr1Wr0Down
Rd1Wr0Wr1Wr0DownReturn:

   ; Rd0Wr1Wr0Down Test
   mov  w4, w0 ; address
   mov  w5, w1 ; length   
   goto Rd0Wr1Wr0Down
Rd0Wr1Wr0DownReturn:

;---------------------------
   ; restore data from buffer
   mov  w4, w0 ; address

   clr  w4     ; SUCCESS
   bra  Restore

Error:
   ; restore data from buffer
   mov  w4, w0 ; address

   mov  #1, w4 ; FAILED

Restore:
;---------------------------

   mov  w5, w1 ; length 
   mov  w6, w2 ; buffer address
   cp0  w2
   bra  z, SkipRestore
RestoreLoop:
   disi #0x3fff
   mov [w2++], [w0++]
   dec  w1, w1  
   bra  nz, RestoreLoop
SkipRestore:

   mov  w4, w0

   ; restore interrupts
   mov  w3, DISICNT
   mov  w0, WREG0   ; DISICNT BUG FIX

   btsc w7, #15
   goto StartupMarchBReturn

   return


;------------------------------------------------------------------------
Rd0Wr1Rd1Wr0Rd0Wr1Up:

Loop0:
   disi #0x3fff

   btsc [w0], #0
   bra  Error     
   bset [w0], #0
   btss [w0], #0
   bra  Error     
   bclr [w0], #0
   btsc [w0], #0
   bra  Error     
   bset [w0], #0

   btsc [w0], #1
   bra  Error     
   bset [w0], #1
   btss [w0], #1
   bra  Error     
   bclr [w0], #1
   btsc [w0], #1
   bra  Error     
   bset [w0], #1

   btsc [w0], #2
   bra  Error     
   bset [w0], #2
   btss [w0], #2
   bra  Error     
   bclr [w0], #2
   btsc [w0], #2
   bra  Error     
   bset [w0], #2

   btsc [w0], #3
   bra  Error     
   bset [w0], #3
   btss [w0], #3
   bra  Error     
   bclr [w0], #3
   btsc [w0], #3
   bra  Error     
   bset [w0], #3

   btsc [w0], #4
   bra  Error     
   bset [w0], #4
   btss [w0], #4
   bra  Error     
   bclr [w0], #4
   btsc [w0], #4
   bra  Error     
   bset [w0], #4

   btsc [w0], #5
   bra  Error    
   bset [w0], #5
   btss [w0], #5
   bra  Error     
   bclr [w0], #5
   btsc [w0], #5
   bra  Error     
   bset [w0], #5

   btsc [w0], #6
   bra  Error     
   bset [w0], #6
   btss [w0], #6
   bra  Error     
   bclr [w0], #6
   btsc [w0], #6
   bra  Error     
   bset [w0], #6

   btsc [w0], #7
   bra  Error     
   bset [w0], #7
   btss [w0], #7
   bra  Error     
   bclr [w0], #7
   btsc [w0], #7
   bra  Error     
   bset [w0], #7

   btsc [w0], #8
   bra  Error     
   bset [w0], #8
   btss [w0], #8
   bra  Error     
   bclr [w0], #8
   btsc [w0], #8
   bra  Error     
   bset [w0], #8

   btsc [w0], #9
   bra  Error     
   bset [w0], #9
   btss [w0], #9
   bra  Error     
   bclr [w0], #9
   btsc [w0], #9
   bra  Error     
   bset [w0], #9

   btsc [w0], #10
   bra  Error     
   bset [w0], #10
   btss [w0], #10
   bra  Error     
   bclr [w0], #10
   btsc [w0], #10
   bra  Error     
   bset [w0], #10

   btsc [w0], #11
   bra  Error     
   bset [w0], #11
   btss [w0], #11
   bra  Error     
   bclr [w0], #11
   btsc [w0], #11
   bra  Error     
   bset [w0], #11

   btsc [w0], #12
   bra  Error     
   bset [w0], #12
   btss [w0], #12
   bra  Error     
   bclr [w0], #12
   btsc [w0], #12
   bra  Error     
   bset [w0], #12

   btsc [w0], #13
   bra  Error     
   bset [w0], #13
   btss [w0], #13
   bra  Error     
   bclr [w0], #13
   btsc [w0], #13
   bra  Error     
   bset [w0], #13

   btsc [w0], #14
   bra  Error     
   bset [w0], #14
   btss [w0], #14
   bra  Error     
   bclr [w0], #14
   btsc [w0], #14
   bra  Error     
   bset [w0], #14

   btsc [w0], #15
   bra  Error     
   bset [w0], #15
   btss [w0], #15
   bra  Error     
   bclr [w0], #15
   btsc [w0], #15
   bra  Error     
   bset [w0], #15

   inc2 w0, w0
   dec  w1, w1  
   bra  nz, Loop0

   goto Rd0Wr1Rd1Wr0Rd0Wr1UpReturn

;------------------------------------------------------------------------
Rd1Wr0Wr1Up:

Loop1:
   disi #0x3fff

   btss [w0], #0
   bra  Error     
   bclr [w0], #0
   bset [w0], #0

   btss [w0], #1
   bra  Error     
   bclr [w0], #1
   bset [w0], #1

   btss [w0], #2
   bra  Error     
   bclr [w0], #2
   bset [w0], #2

   btss [w0], #3
   bra  Error     
   bclr [w0], #3
   bset [w0], #3

   btss [w0], #4
   bra  Error     
   bclr [w0], #4
   bset [w0], #4

   btss [w0], #5
   bra  Error     
   bclr [w0], #5
   bset [w0], #5

   btss [w0], #6
   bra  Error     
   bclr [w0], #6
   bset [w0], #6

   btss [w0], #7
   bra  Error     
   bclr [w0], #7
   bset [w0], #7

   btss [w0], #8
   bra  Error     
   bclr [w0], #8
   bset [w0], #8

   btss [w0], #9
   bra  Error     
   bclr [w0], #9
   bset [w0], #9

   btss [w0], #10
   bra  Error     
   bclr [w0], #10
   bset [w0], #10

   btss [w0], #11
   bra  Error     
   bclr [w0], #11
   bset [w0], #11

   btss [w0], #12
   bra  Error     
   bclr [w0], #12
   bset [w0], #12

   btss [w0], #13
   bra  Error     
   bclr [w0], #13
   bset [w0], #13

   btss [w0], #14
   bra  Error     
   bclr [w0], #14
   bset [w0], #14

   btss [w0], #15
   bra  Error     
   bclr [w0], #15
   bset [w0], #15

   inc2 w0, w0
   dec  w1, w1  
   bra  nz, Loop1

   goto Rd1Wr0Wr1UpReturn

;------------------------------------------------------------------------
Rd1Wr0Wr1Wr0Down:

   add  w1, w0, w0
   add  w1, w0, w0

Loop2:
   disi #0x3fff

   dec2 w0, w0

   btss [w0], #15
   bra  Error     
   bclr [w0], #15
   bset [w0], #15
   bclr [w0], #15

   btss [w0], #14
   bra  Error     
   bclr [w0], #14
   bset [w0], #14
   bclr [w0], #14

   btss [w0], #13
   bra  Error     
   bclr [w0], #13
   bset [w0], #13
   bclr [w0], #13

   btss [w0], #12
   bra  Error     
   bclr [w0], #12
   bset [w0], #12
   bclr [w0], #12

   btss [w0], #11
   bra  Error     
   bclr [w0], #11
   bset [w0], #11
   bclr [w0], #11

   btss [w0], #10
   bra  Error     
   bclr [w0], #10
   bset [w0], #10
   bclr [w0], #10

   btss [w0], #9
   bra  Error     
   bclr [w0], #9
   bset [w0], #9
   bclr [w0], #9

   btss [w0], #8
   bra  Error     
   bclr [w0], #8
   bset [w0], #8
   bclr [w0], #8

   btss [w0], #7
   bra  Error     
   bclr [w0], #7
   bset [w0], #7
   bclr [w0], #7

   btss [w0], #6
   bra  Error     
   bclr [w0], #6
   bset [w0], #6
   bclr [w0], #6

   btss [w0], #5
   bra  Error     
   bclr [w0], #5
   bset [w0], #5
   bclr [w0], #5

   btss [w0], #4
   bra  Error     
   bclr [w0], #4
   bset [w0], #4
   bclr [w0], #4

   btss [w0], #3
   bra  Error     
   bclr [w0], #3
   bset [w0], #3
   bclr [w0], #3

   btss [w0], #2
   bra  Error     
   bclr [w0], #2
   bset [w0], #2
   bclr [w0], #2

   btss [w0], #1
   bra  Error     
   bclr [w0], #1
   bset [w0], #1
   bclr [w0], #1

   btss [w0], #0
   bra  Error     
   bclr [w0], #0
   bset [w0], #0
   bclr [w0], #0

   dec  w1, w1
   bra  nz, Loop2

   goto Rd1Wr0Wr1Wr0DownReturn

;------------------------------------------------------------------------
Rd0Wr1Wr0Down:

   add  w1, w0, w0
   add  w1, w0, w0

Loop3:
   disi #0x3fff

   dec2 w0, w0

   btsc [w0], #15
   bra  Error     
   bset [w0], #15
   bclr [w0], #15

   btsc [w0], #14
   bra  Error     
   bset [w0], #14
   bclr [w0], #14

   btsc [w0], #13
   bra  Error     
   bset [w0], #13
   bclr [w0], #13

   btsc [w0], #12
   bra  Error     
   bset [w0], #12
   bclr [w0], #12

   btsc [w0], #11
   bra  Error     
   bset [w0], #11
   bclr [w0], #11

   btsc [w0], #10
   bra  Error     
   bset [w0], #10
   bclr [w0], #10

   btsc [w0], #9
   bra  Error     
   bset [w0], #9
   bclr [w0], #9

   btsc [w0], #8
   bra  Error     
   bset [w0], #8
   bclr [w0], #8

   btsc [w0], #7
   bra  Error     
   bset [w0], #7
   bclr [w0], #7

   btsc [w0], #6
   bra  Error     
   bset [w0], #6
   bclr [w0], #6

   btsc [w0], #5
   bra  Error     
   bset [w0], #5
   bclr [w0], #5

   btsc [w0], #4
   bra  Error     
   bset [w0], #4
   bclr [w0], #4

   btsc [w0], #3
   bra  Error     
   bset [w0], #3
   bclr [w0], #3

   btsc [w0], #2
   bra  Error     
   bset [w0], #2
   bclr [w0], #2

   btsc [w0], #1
   bra  Error     
   bset [w0], #1
   bclr [w0], #1

   btsc [w0], #0
   bra  Error     
   bset [w0], #0
   bclr [w0], #0

   dec  w1, w1
   bra  nz, Loop3

   goto Rd0Wr1Wr0DownReturn

.end
