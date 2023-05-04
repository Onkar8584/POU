/*
================================================================================
File name:    IoTranslate.h
                    
Originator:   Poorana kumar G

Platform:     dsPIC33EV64GM006
Compiler:     XC16 V1.40
IDE     :     MPLAB X IDE V5.20

Description: 
  IoTranslate.c contains all the macro definitions for virtual hardware to
  actual hardware translation.

Class Methods:
 None

Resources:
 None

================================================================================
 History:	
-*-----*-----------*------------------------------------*-----------------------
       05-12-2020  Initial Write                        Poorana kumar G
--------------------------------------------------------------------------------
*/


#include "ADCRead.h"

/*
================================================================================
Method name:  readADCIOStatus
                    
Originator:   Poorana kumar G

Description: 
  To read the digital status of the analog input pin.

Resources:
  None

================================================================================
 History:	
-*-----*-----------*------------------------------------*-----------------------
       05-12-2020  Initial Write                        Poorana kumar G
--------------------------------------------------------------------------------
*/

bool readADCIOStatus(uint8_t adcNum)
{
  bool status = 0;
  uint16_t delayW = 0;

  // Configure the relevant analog input as GPIO with internal pull up
  switch(adcNum) {
    case INLET_TEMPERATURE:
      ANSELAbits.ANSA7 = 0;
      CNPUAbits.CNPUA7 = 1;
      break;

    case OUTLET_TEMPERATURE:
      ANSELAbits.ANSA10 = 0;
      CNPUAbits.CNPUA10 = 1;
      break;

    case CHAMBER_TEMPERATURE1:
      ANSELCbits.ANSC0 = 0;
      CNPUCbits.CNPUC0 = 1;
      break;

    case CHAMBER_TEMPERATURE2:
      ANSELCbits.ANSC1 = 0;
      CNPUCbits.CNPUC1 = 1;
      break;

    case CHAMBER_TEMPERATURE3:
      ANSELCbits.ANSC2 = 0;
      CNPUCbits.CNPUC2 = 1;
      break;

    case CHAMBER_TEMPERATURE4:
      ANSELCbits.ANSC11 = 0;
      CNPUCbits.CNPUC11 = 1;
      break;

    default:
      break;
  }

  // Wait to IO level is settling
  for (delayW = 0; delayW < 3000; delayW++);

  // Read IO digital status & configure it as analog input
  switch(adcNum) {
    case INLET_TEMPERATURE:
      status = PORTAbits.RA7;
      CNPUAbits.CNPUA7 = 0;
      ANSELAbits.ANSA7 = 1;
      break;

    case OUTLET_TEMPERATURE:
      status = PORTAbits.RA10;
      CNPUAbits.CNPUA10 = 0;
      ANSELAbits.ANSA10 = 1;
      break;

    case CHAMBER_TEMPERATURE1:
      status = PORTCbits.RC0;
      CNPUCbits.CNPUC0 = 0;
      ANSELCbits.ANSC0 = 1;
      break;

    case CHAMBER_TEMPERATURE2:
      status = PORTCbits.RC1;
      CNPUCbits.CNPUC1 = 0;
      ANSELCbits.ANSC1 = 1;
      break;

    case CHAMBER_TEMPERATURE3:
      status = PORTCbits.RC2;
      CNPUCbits.CNPUC2 = 0;
      ANSELCbits.ANSC2 = 1;
      break;

    case CHAMBER_TEMPERATURE4:
      status = PORTCbits.RC11;
      CNPUCbits.CNPUC11 = 0;
      ANSELCbits.ANSC11 = 1;
      break;

    default:
      break;
  }

  // Return the IO status read
  return status;
}



