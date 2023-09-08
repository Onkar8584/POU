/*************************************************************************
 *  © 2014 Microchip Technology Inc.                                       
 *  
 *  Project Name:    Class B Library
 *  Version:         2.4 
 *  FileName:        classb_crc.c
 *  Dependencies:    classb.h
 *  Processor:       PIC24, dsPIC
 *  Compiler:        XC16
 *  IDE:             MPLAB® X                        
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *  Description:     This file contains functions for the 16-bit CRC
 *                   calculation.
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


/****************************************************************************
  Polynomial used for the CRC calculation. For the standard 16-bit CRC
  algorithm it should be set to 0x8005 (x^16 + x^15 + x^2 + 1).  
  ***************************************************************************/
#define CLASSB_CRC_POLYNOMIAL           0x8005


typedef struct PROG_POINTER_TAG {
  union {
    __prog__ uint16_t* pointer;
    struct {
        uint16_t offset;
        uint8_t  tlbpag;
    };
  };
} PROG_POINTER;

uint16_t WordCRC(uint16_t word, uint16_t crcSeed) 
{
uint8_t   i;
uint8_t   crcNext;
uint16_t  crcResult;

    crcResult = crcSeed;

	for(i=0; i<16; i++)
	{
		crcNext = 0;

		if(crcResult&0x8000)
        {
            crcNext = 1;
        }

        if(word&0x8000)
        {
		    crcNext ^= 1;
        }

		word <<= 1;
		crcResult <<= 1;

 
		if(crcNext)
        {
            crcResult ^= CLASSB_CRC_POLYNOMIAL;
        }
	}

    return crcResult;
}

uint16_t ByteCRC(uint8_t byte, uint16_t crcSeed) 
{
uint8_t   i;
uint8_t   crcNext;
uint16_t  crcResult;

    crcResult = crcSeed;

	for(i=0; i<8; i++)
	{
		crcNext = 0;

		if(crcResult&0x8000)
        {
            crcNext = 1;
        }

        if(byte&0x80)
        {
		    crcNext ^= 1;
        }

		byte <<= 1;
		crcResult <<= 1;

 
		if(crcNext)
        {
            crcResult ^= CLASSB_CRC_POLYNOMIAL;
        }
	}

    return crcResult;
}

uint16_t CLASSB_CRCFlashTestF(__prog__ uint16_t* startAddress, uint32_t length, uint16_t crcSeed)
{
uint16_t  value;
uint16_t  crcResult;
PROG_POINTER address;

    length >>= 1;
    address.pointer = startAddress;
    crcResult = crcSeed;

    while(length)
    { 
        TBLPAG = address.tlbpag;
        value = __builtin_tblrdh(address.offset);
        crcResult = ByteCRC(value, crcResult);
        value = __builtin_tblrdl(address.offset);
        crcResult = WordCRC(value, crcResult);
        address.pointer++;
        length--;
    }

    return crcResult;
}

uint16_t CLASSB_CRCEEPROMTestF(__prog__ uint16_t* startAddress, uint32_t length, uint16_t crcSeed)
{
uint16_t  value;
uint16_t  crcResult;
PROG_POINTER address;

    length >>= 1;
    address.pointer = startAddress;
    crcResult = crcSeed;

    while(length)
    { 
        TBLPAG = address.tlbpag;
        value = __builtin_tblrdl(address.offset);
        crcResult = WordCRC(value, crcResult);
        address.pointer++;
        length--; 
    }

    return crcResult;
}

