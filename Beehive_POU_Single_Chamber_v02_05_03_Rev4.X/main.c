/**
  Generated main.c file from MPLAB Code Configurator

  @Company
    Microchip Technology Inc.

  @File Name
    main.c

  @Summary
    This is the generated main.c using PIC24 / dsPIC33 / PIC32MM MCUs.

  @Description
    This source file provides main entry point for system intialization and application code development.
    Generation Information :
        Product Revision  :  PIC24 / dsPIC33 / PIC32MM MCUs - 1.95-b-SNAPSHOT
        Device            :  dsPIC33EV64GM006
    The generated drivers are tested against the following:
        Compiler          :  XC16 v1.36
        MPLAB 	          :  MPLAB X v5.10
*/

/*
    (c) 2016 Microchip Technology Inc. and its subsidiaries. You may use this
    software and any derivatives exclusively with Microchip products.

    THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS". NO WARRANTIES, WHETHER
    EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE, INCLUDING ANY IMPLIED
    WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS FOR A
    PARTICULAR PURPOSE, OR ITS INTERACTION WITH MICROCHIP PRODUCTS, COMBINATION
    WITH ANY OTHER PRODUCTS, OR USE IN ANY APPLICATION.

    IN NO EVENT WILL MICROCHIP BE LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE,
    INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND
    WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP HAS
    BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE FORESEEABLE. TO THE
    FULLEST EXTENT ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL CLAIMS IN
    ANY WAY RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT OF FEES, IF ANY,
    THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS SOFTWARE.

    MICROCHIP PROVIDES THIS SOFTWARE CONDITIONALLY UPON YOUR ACCEPTANCE OF THESE
    TERMS.
*/

/**
  Section: Included Files
*/
#include "mcc_generated_files/system.h"

#include "IoTranslate.h"
#include "Scheduler.h"
#include "FlowDetector.h"
#include "NonVol.h"
#include "ModeCheck.h"
#include "ADCRead.h"
#include "UserInterface.h"
#include "OptoCouplerControl.h"
#include "FaultIndication.h"
#include "Events.h"
#include "SelfTest.h"
#include "TemperatureControl.h"
#include "Version.h"

#ifdef DEBUG_MACRO
#include "SerialDebug.h"
#endif

/*
                         Main application
*/

// Objects declaration and Initialization
Scheduler_STYP scheduler = SCHEDULER_DEFAULTS;
UserInterface_STYP uI = USER_INTERFACE_DEFAULTS;
OptoCouplerControl_STYP optoCouplerControl = OPTO_COUPLER_CONTROL_DEFAULTS;
FlowDetector_STYP flowDetector = FLOW_DETECTOR_DEFAULTS;
ModeCheck_STYP modeCheck = MODE_CHECK_DEFAULTS;
ADCRead_STYP adcRead = ADC_READ_DEFAULTS;
NonVol_STYP nonVol = NON_VOL_DEFAULTS;
FaultIndication_STYP faultIndication = FAULT_INDICATION_DEFAULTS;
SelfTest_STYP selfTest = SELF_TEST_DEFAULTS;
TemperatureControl_STYP tempControl = TEMPERATURE_CONTROL_DEFAULTS;

#ifdef DEBUG_MACRO
SerialDebug_STYP Serial = SERIAL_DEBUG_DEFAULTS;
#endif

// Software version
char SWVersion[] = VERSION_STRING;

int main(void)
{
  // initialize the device
  SYSTEM_Initialize();

  // Start watchdog timer
  START_WATCHDOG();

  // Initialize the timers
  TimersStartup();

  CTMUCON1bits.CTMUEN = 1;

  // Initialize the non volatile memory
  nonVol.init();

  // Execute the power ON self test
  selfTest.POST();

#ifdef DEBUG_MACRO
  // Print the software version
  Serial.PrintVersion();
#endif

  // Trigger the ADC conversion
//  adcRead.TriggerConversion();

  scheduler.AddTask(optoCouplerControl.Modulate,                            \
          OPTOCOUPLER_CONTROL_INITIAL_DELAY, OPTOCOUPLER_CONTROL_INTERVAL);
  scheduler.AddTask(uI.UIFunction, USER_INTERFACE_INITIAL_DELAY,            \
          USER_INTERFACE_INTERVAL);
  scheduler.AddTask(flowDetector.Detect, FLOW_DETECTOR_INITIAL_DELAY,       \
          FLOW_DETECTOR_INTERVAL);
  scheduler.AddTask(adcRead.ReadFunction, ADC_READ_INITIAL_DELAY,           \
          ADC_READ_INTERVAL);
  scheduler.AddTask(faultIndication.Control, FAULT_INDICATION_INITIAL_DELAY,\
          FAULT_INDICATION_INTERVAL);
  scheduler.AddTask(modeCheck.Check, MODE_CHECK_INITIAL_DELAY,              \
          MODE_CHECK_INTERVAL);
  scheduler.AddTask(selfTest.RunTimeTest, SELFTEST_INITIAL_DELAY,           \
          SELFTEST_INTERVAL);
  scheduler.AddTask(tempControl.Control, TEMPERATURE_CONTROL_INITIAL_DELAY, \
          TEMPERATURE_CONTROL_INTERVAL);

#ifdef DEBUG_MACRO
  scheduler.AddTask(Serial.DebugFunction, SERIAL_DEBUG_INITIAL_DELAY,       \
          SERIAL_DEBUG_INTERVAL);
#endif

  while (1)
  {
    // Add your application code
    if ( scheduler.flags.interruptFLG) {
      scheduler.flags.interruptFLG = 0;
      scheduler.run();

      // Reset watchdog timer whenever tasks get executed
      RESET_WATCHDOG();
    }
  }
  return 1; 
}
/**
 End of File
*/

