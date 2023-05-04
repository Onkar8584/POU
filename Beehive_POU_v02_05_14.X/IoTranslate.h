/*
================================================================================
File name:    IoTranslate.h
                    
Originator:   Poorana kumar G

Platform:     dsPIC33EV64GM006
Compiler:     XC16 V1.40
IDE     :     MPLAB X IDE V5.20

Description: 
  IoTranslate.h contains all the macro definitions for virtual hardware to
  actual hardware translation.

Class Methods:
 None

Resources:
 None

================================================================================
 History:	
-*-----*-----------*------------------------------------*-----------------------
       09-23-2019  Initial Write                        Poorana kumar G
--------------------------------------------------------------------------------
*/


#ifndef _IO_TRANSLATE_H
#define	_IO_TRANSLATE_H

#include "Build.h"
#include "mcc_generated_files/tmr2.h"
#include "mcc_generated_files/watchdog.h"

// MACROS USED FOR WATCHDOG
// To start the watchdog
#define START_WATCHDOG()                    WATCHDOG_TimerSoftwareEnable()


// To reset the watchdog
#define RESET_WATCHDOG()                    WATCHDOG_TimerClear()

// MACROS USED IN SCHEDULER
#define START_SCHEDULER_TIMER()             TMR1_Start()


// MACROS USED IN ADCREAD
// Selects ADC channel to do conversion
#define ADCREAD_CHANNEL_SELECT(ChannelIdx)  ADC1_ChannelSelectSet(ChannelIdx)

// Trigger the ADC sampling
#define ADCREAD_START_SAMPLING()            ADC1_SamplingStart()

// Stop the ADC sampling
#define ADCREAD_STOP_SAMPLING()             ADC1_SamplingStop()

// Check the conversion status
#define ADCREAD_CONVERSION_COMPLETE()       ADC1_IsConversionComplete()

// Read the converted value
#define ADCREAD_READ_REGISTER()             ADC1_Channel0ConversionResultGet()


// MACROS USED IN FAULTINDICATION
// To turn ON the Celsius LED
#define CelsiusLEDDigOut_ON()               C_LED_SetLow()

// To turn OFF the Celsius LED
#define CelsiusLEDDigOut_OFF()              C_LED_SetHigh()

// To turn ON the Fahrenheit LED
#define FahrenheitLEDDigOut_ON()            F_LED_SetLow()

// To turn OFF the Fahrenheit LED
#define FahrenheitLEDDigOut_OFF()           F_LED_SetHigh()

// To turn ON the Heart Beat LED
#define HeartBeatLEDDigOut_ON()             HEART_BEAT_LED_SetLow()

// To turn OFF the Heart Beat LED
#define HeartBeatLEDDigOut_OFF()            HEART_BEAT_LED_SetHigh()

// To toggle the Heart Beat LED
#define HeartBeatLEDDigOut_TOGGLE()         HEART_BEAT_LED_Toggle()


// MACROS USED IN FLOWDETECTOR
// To Read the Flow Detector Pulse input
#define FlowDetectorPulseDigIn_Read()       FLOW_DETECTOR_PULSE_GetValue()

// To Read the Flow Detector Connection status
#define FlowDetectorConnDigIn_Read()        FLOW_DETECTOR_CONNECTION_GetValue()


// MACROS USED IN MODECHECK
// To Read the Emergency Eye wash mode GPIO
#define EmergencyEyeWashModeDigIn_Read()    TEMPSEL_EMGY_GetValue()

// To Read the Lavatory mode GPIO
#define LavatoryModeDigIn_Read()            TEMPSEL_LAVY_GetValue()

// To Read the Sanitation mode GPIO
#define SanitationModeDigIn_Read()          TEMPSEL_SANI_GetValue()


// MACROS USED IN OPTOCOUPLERCONTROL
// To turn ON the Opto Coupler 1
#define OptoCoupler1ControlDigOut_ON()      RELAY1_OPTO_CONTROL_SetHigh()

// To turn OFF the Opto Coupler 1
#define OptoCoupler1ControlDigOut_OFF()     RELAY1_OPTO_CONTROL_SetLow()

// To turn ON the Opto Coupler 2
#define OptoCoupler2ControlDigOut_ON()      RELAY2_OPTO_CONTROL_SetHigh()

// To turn OFF the Opto Coupler 2
#define OptoCoupler2ControlDigOut_OFF()     RELAY2_OPTO_CONTROL_SetLow()


// MACROS USED IN SELFTEST
// Relay 1 - 5V feedback GPIO read
#define Relay1SupplyStatusDigIn_Read()      RELAY1_POSITIVE_STATUS_IN_GetValue()

// Relay 1 - Gnd feedback GPIO read
#define Relay1GroundStatusDigIn_Read()      RELAY1_NEGATIVE_STATUS_IN_GetValue()

// Relay 2 - 5V feedback GPIO read
#define Relay2SupplyStatusDigIn_Read()      RELAY2_POSITIVE_STATUS_IN_GetValue()

// Relay 2 - Gnd feedback GPIO read
#define Relay2GroundStatusDigIn_Read()      RELAY2_NEGATIVE_STATUS_IN_GetValue()

// Opto Coupler 1 Status input GPIO read
#define OptoCoupler1FBStatusDigIn_Read()    OPTO1_FB_IN_GetValue()

// Opto Coupler 2 Status input GPIO read
#define OptoCoupler2FBStatusDigIn_Read()    OPTO2_FB_IN_GetValue()


// MACROS USED IN TEMPERATURECONTROL
// To turn ON the Relay1
#define RelayControl1DigOut_ON()                \
        {                                       \
          RELAY1_POSITIVE_CONTROL_SetHigh();    \
          RELAY1_NEGATIVE_CONTROL_SetLow();     \
        }

// To turn OFF the Relay1
#define RelayControl1DigOut_OFF()               \
        {                                       \
          RELAY1_POSITIVE_CONTROL_SetLow();     \
          RELAY1_NEGATIVE_CONTROL_SetHigh();    \
        }

// To turn ON the Relay2
#define RelayControl2DigOut_ON()                \
        {                                       \
          RELAY2_POSITIVE_CONTROL_SetHigh();    \
          RELAY2_NEGATIVE_CONTROL_SetLow();     \
        }

// To turn OFF the Relay2
#define RelayControl2DigOut_OFF()               \
        {                                       \
          RELAY2_POSITIVE_CONTROL_SetLow();     \
          RELAY2_NEGATIVE_CONTROL_SetHigh();    \
        }


// MACROS USED IN USERINTERFACE
// The GPIO port display segments connected
#define DISPLAY_GPIO_PORT                   LATB

// To Read the Up button status
#define UpButtonDigIn_Read()                UP_BUTTON_IN_GetValue()

// To Read the Down button status
#define DownButtonDigIn_Read()              DOWN_BUTTON_IN_GetValue()

// To Read the enter button status
#define EnterButtonDigIn_Read()             ENTER_BUTTON_IN_GetValue()

// To turn ON the digit 1 of the 7 segment display
#define DisplayDigit1DigOut_ON()            DISPLAY_DIGIT1_CONTROL_SetLow()

// To turn OFF the digit 1 of the display
#define DisplayDigit1DigOut_OFF()           DISPLAY_DIGIT1_CONTROL_SetHigh()   

// To turn ON the digit 2 of the 7 segment display
#define DisplayDigit2DigOut_ON()            DISPLAY_DIGIT2_CONTROL_SetLow()

// To turn OFF the digit 2 of the display
#define DisplayDigit2DigOut_OFF()           DISPLAY_DIGIT2_CONTROL_SetHigh()

// To turn ON the digit 3 of the 7 segment display
#define DisplayDigit3DigOut_ON()            DISPLAY_DIGIT3_CONTROL_SetLow()

// To turn OFF the digit 3 of the display
#define DisplayDigit3DigOut_OFF()           DISPLAY_DIGIT3_CONTROL_SetHigh()

// To set the status of GPIO used to control segment A
#define DisplaySegADigOut_High()            SEG_A_SetHigh()

// To clear the status of GPIO used to control segment A
#define DisplaySegADigOut_Low()             SEG_A_SetLow()

// To set the status of GPIO used to control segment B
#define DisplaySegBDigOut_High()            SEG_B_SetHigh()

// To clear the status of GPIO used to control segment B
#define DisplaySegBDigOut_Low()             SEG_B_SetLow()

// To set the status of GPIO used to control segment C
#define DisplaySegCDigOut_High()            SEG_C_SetHigh()

// To clear the status of GPIO used to control segment C
#define DisplaySegCDigOut_Low()             SEG_C_SetLow()

// To set the status of GPIO used to control segment D
#define DisplaySegDDigOut_High()            SEG_D_SetHigh()

// To clear the status of GPIO used to control segment D
#define DisplaySegDDigOut_Low()             SEG_D_SetLow()

// To set the status of GPIO used to control segment E
#define DisplaySegEDigOut_High()            SEG_E_SetHigh()

// To clear the status of GPIO used to control segment E
#define DisplaySegEDigOut_Low()             SEG_E_SetLow()

// To set the status of GPIO used to control segment F
#define DisplaySegFDigOut_High()            SEG_F_SetHigh()

// To clear the status of GPIO used to control segment F
#define DisplaySegFDigOut_Low()             SEG_F_SetLow()

// To set the status of GPIO used to control segment G
#define DisplaySegGDigOut_High()            SEG_G_SetHigh()

// To clear the status of GPIO used to control segment G
#define DisplaySegGDigOut_Low()             SEG_G_SetLow()

// To set the status of GPIO used to control segment Dot
#define DisplaySegDotDigOut_High()          SEG_DOT_SetHigh()

// To clear the status of GPIO used to control segment Dot
#define DisplaySegDotDigOut_Low()           SEG_DOT_SetLow()


// To Read the status of AC Line cross GPIO
#define ACLineCrossDigIn_Read()             AC_LINE_CROSS_GetValue()

// To turn ON the Buzzer
#define BuzzerDigOut_ON()                   PWM_ModuleEnable()

// To turn OFF the Buzzer
#define BuzzerDigOut_OFF()                  PWM_ModuleDisable()

// Enable the clock input to timer and start it
#define ENABLE_TIMER_FOR_CLOCK_TEST()       TMR2_Start()

// Disable the clock input to timer and stop it
#define DISABLE_TIMER_AFTER_CLOCK_TEST()    TMR2_Stop()

// Stop the timer, reset it and then start
#define RESET_RESTART_INTR_MONITOR_TIMER()      \
        {                                       \
            TMR4_Stop();                        \
            TMR4_Counter16BitSet(0);            \
            TMR4_Start();                       \
        }

extern void TimerISRFunction(void);
extern void InterruptMonitorTimerISRFunction(void);

/*
================================================================================
Method name:  TimersStartup
                    
Originator:   Poorana kumar G

Description: 
  This function is used to initialize all the timers used in the system.

  This method should be called using TimersStartup().

Resources:
  None 

================================================================================
 History:	
-*-----*-----------*------------------------------------*-----------------------
       10-21-2019  Initial Write                        Poorana kumar G
--------------------------------------------------------------------------------
*/
inline static void TimersStartup(void)
{
  // Timer to schedule the tasks
  TMR1_SetInterruptHandler(&TimerISRFunction);
  TMR1_Start();

  // Timer to monitor the timer 1 interrupt
  TMR4_SetInterruptHandler(&InterruptMonitorTimerISRFunction);
  TMR4_Start();
}

/*
================================================================================
Method name:  ADCReadTriggerDischarge
                    
Originator:   Poorana kumar G

Description: 
  This function is used to discharge the sample & hold capacitor before ADC
  conversion triggers.

  This method should be called using ADCReadTriggerDischarge().

Resources:
  None 

================================================================================
 History:	
-*-----*-----------*------------------------------------*-----------------------
       05-12-2020  Initial Write                        Poorana kumar G
--------------------------------------------------------------------------------
*/

inline static void ADCReadTriggerDischarge(void)
{
  // Select CTMU Channel, start sampling and enable discharge
  ADCREAD_CHANNEL_SELECT(ADC1_CHANNEL_CTMU);
  ADCREAD_START_SAMPLING();
  CTMUCON1bits.IDISSEN = 1;
}


/*
================================================================================
Method name:  ADCReadStopDischarge
                    
Originator:   Poorana kumar G

Description: 
  This function is used to stop the discharge operation.

  This method should be called using ADCReadTriggerDischarge().

Resources:
  None 

================================================================================
 History:	
-*-----*-----------*------------------------------------*-----------------------
       05-12-2020  Initial Write                        Poorana kumar G
--------------------------------------------------------------------------------
*/

inline static void ADCReadStopDischarge(void)
{
  // Stop discharge & sampling operation
  CTMUCON1bits.IDISSEN = 0;
  ADCREAD_STOP_SAMPLING();
}


// FUNCTIONS
bool readADCIOStatus(uint8_t adcNum);

#endif	/* _IO_TRANSLATE_H */

