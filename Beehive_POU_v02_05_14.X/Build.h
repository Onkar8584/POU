/*
================================================================================
File name:    Build.h
                    
Originator:   Poorana kumar G

Platform:     dsPIC33EV64GM006
Compiler:     XC16 V1.40
IDE     :     MPLAB X IDE V5.20

Description: 

Class Methods:
  This file contains special complier build options for the specific processor
  and project.

Resources:
 None
================================================================================
 History:	
-*-----*-----------*------------------------------------*-----------------------
       09-23-2019  Initial Write                            Poorana kumar G
2.2.0  07-16-2020  New macro is added to disable the inlet  Poorana kumar G
                   thermistor in build time.
--------------------------------------------------------------------------------
*/

#ifndef _BUILD_H
#define	_BUILD_H

// include processor files - each processor file is guarded.
#include <xc.h>
#include <stdint.h>

// Microchip code Generator files
#include "pin_manager.h"
#include "adc1.h"
#include "memory/flash.h"
#include "uart1.h"
#include "tmr1.h"
#include "tmr4.h"
#include "pwm.h"

// Uncomment this macro to enable the debug UART prints
#define DEBUG_MACRO

// Uncomment this macro to block the inlet thermistor related process
#define DISABLE_INLET_THERMISTOR

#define SCHEDULER_MAX_TASKS         10  // Maximum tasks can be scheduled.

#define TOTAL_ADC_CHANNELS          8       // Total analog inputs
#define ADC_REF_VOLTAGE             5000    // ADC ref voltage in miliVolts
#define ADC_FULL_COUNT              4096    // 12 bit ADC
#define TOTAL_THERMISTORS           6       // Total thermistors

#define FtoCconvert(F)              (((F - 32) * 5) / 9)
#define CtoFconvert(C)              (((C * 9) / 5) + 32)

#endif	/* _BUILD_H */

