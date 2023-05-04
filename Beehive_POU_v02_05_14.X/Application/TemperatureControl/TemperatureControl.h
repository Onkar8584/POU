/*
================================================================================
File name:    TemperatureControl.h
                    
Originator:   Poorana kumar G

Platform:     dsPIC33EV64GM006
Compiler:     XC16 V1.40
IDE     :     MPLAB X IDE V5.20

Description: 
  The purpose of this object to monitor the temperature of inlet & outlet by
  using the converted ADC value and control the relays used for heaters based
  on the water flow detector input as well as decides the power cycle to be
  provided to the opto couplers using the PID constants & algorithm.

Class Methods:
  void TemperatureControl(void);
    Call periodically from Scheduler (500 msec), to control the temperature of
    inlet and outlet temperature based on the thermistor analog data and flow
    detection using PID algorithm.

  void PIDCalculation(void);
    Called when the the relay control status is "RELAY_CONTROL_CONTROL" to
    calculate the power cycle to be applied to the opto coupler.

Method Calling Requirements:
  tempControl.Control() should be called once per 500 millisecond in
  scheduler.

Resources:
  2 GPIOs for 2 Relays

IoTranslate requirements:
  #define RelayControl1DigOut_ON()      // To turn ON the Relay1
  #define RelayControl1DigOut_OFF()     // To turn OFF the Relay1
  #define RelayControl2DigOut_ON()      // To turn ON the Relay2
  #define RelayControl2DigOut_OFF()     // To turn OFF the Relay2

================================================================================
 History:	
-*-----*-----------*------------------------------------*-----------------------
       10-03-2019  Initial Write                        Poorana kumar G
--------------------------------------------------------------------------------
*/

#ifndef _TEMPERATURECONTROL_H
#define _TEMPERATURECONTROL_H

#include "Build.h"
#include "IoTranslate.h"
#include "ADCRead.h"
#include "NonVol.h"
#include "FlowDetector.h"
#include "OptoCouplerControl.h"
#include "FaultIndication.h"

// Enums for Relay control status
typedef enum {
  RELAY_CONTROL_INITIAL = 0,
  RELAY_CONTROL_CONTROL,
  RELAY_CONTROL_SHUTDOWN,
  RELAY_CONTROL_STBYCOOL,
  RELAY_CONTROL_STBYHEAT,
  RELAY_CONTROL_LOWFLOW,
  RELAY_CONTROL_ERROR,
  RELAY_CONTROL_ERROR_WAIT,
  RELAY_CONTROL_DRY_FIRE_WAIT,
  RELAY_CONTROL_TEMP_WAIT,
  RELAY_CONTROL_ANTISCALE        
}RelayControlState_ETYP;

typedef struct {

//  Public Variables
  struct {
    // Temperature control Shut Down flag 0-ON 1-Shutdown        
    uint8_t shutDownFLG:1;
    // Temperature control disable flag 0-Enable 1-Disable control operation
    uint8_t disableControlFLG:1;
    // Reverse flow flag for confirm one more before declare error
    uint8_t reverseFlowFLG:1;
    // overheat flag status for thermistor 1
    uint8_t thermistor1OverHeatFLG:1;
    // overheat flag status for thermistor 2
    uint8_t thermistor2OverHeatFLG:1;
    // overheat flag status for thermistor 3
    uint8_t thermistor3OverHeatFLG:1;
    // overheat flag status for thermistor 4
    uint8_t thermistor4OverHeatFLG:1;
    // In Low flow any to turn any one relay this flag will be used
    uint8_t lowFlowRelayControlFLG:1;
    
  } flags;
  RelayControlState_ETYP relayStatus;
  RelayControlState_ETYP prevRelayStatus;
  int16_t targetADCHalfUnitsW;
  uint16_t overHeatADCHalfUnits;

// Public Methods
  bool (*Control)(void);

// Private Variables
  int16_t temperature2backARYW[TOTAL_THERMISTORS];
  int16_t temperaturePrevARYW[TOTAL_THERMISTORS];
  int16_t temperatureARYW[TOTAL_THERMISTORS];
  int16_t dtOfAverageARYW[TOTAL_THERMISTORS];
  uint16_t dryFireWaitTimerW;

  // For PID calculations
  int16_t outletTemperatureW;
  int16_t outletTemperaturePrevW;
  int16_t dtOutletTemperatureW;
  uint16_t shutDownCounterW;
  uint16_t errorWaitCounterW;
  int16_t dryFireThresholdW;
  float integralF;
  void (*PIDFunction)(void);
} TemperatureControl_STYP;


// DEFINE CLASS OBJECT DEFAULTS

#define TEMPERATURE_CONTROL_DEFAULTS {                              \
                                        {0,0,0,0,0,0,0,0},            \
                                        RELAY_CONTROL_INITIAL,      \
                                        RELAY_CONTROL_INITIAL,      \
                                        0,                          \
                                        0,                          \
                                        &TemperatureControl,        \
                                        {0,0,0,0,0,0},              \
                                        {0,0,0,0,0,0},              \
                                        {THERMISTOR_OPEN_ADC_COUNT, \
                                        THERMISTOR_OPEN_ADC_COUNT,  \
                                        THERMISTOR_OPEN_ADC_COUNT,  \
                                        THERMISTOR_OPEN_ADC_COUNT,  \
                                        THERMISTOR_OPEN_ADC_COUNT,  \
                                        THERMISTOR_OPEN_ADC_COUNT}, \
                                        {0,0,0,0,0,0},              \
                                        DRY_FIRE_WAIT_TIME,         \
                                        0,                          \
                                        0,                          \
                                        0,                          \
                                        0,                          \
                                        0,                          \
                                        DRY_FIRE_THRESHOLD_DEFAULT, \
                                        0.0,                        \
                                        &PIDCalculation,            \
                                     }

#define SHUT_DOWN_TIMEOUT           60                      // 5 Minutes // 30 seconds post v2.3.0 changes
#define ERROR_WAIT_TIME             60                      // 30 seconds
#define MAXPOWER_POWER_CYCLE        120                     // Mode 8
#define STANDBY_POWER_CYCLE         MAXPOWER_POWER_CYCLE    // Maximum
#define POWER_CYCLE_OFF             0                       // OFF
#define STANDBY_OFFSET              10                      // From set point

#define INTIAL_WAIT_TIME             60                      // 30 seconds

#define OVER_HEAT_TEMPERATURE       200                     // 200°F            // value changed as per Mike Jan Updates from 190F to 200F 

#define Tin                         tempControl.temperatureARYW[0]
#define Tout                        tempControl.temperatureARYW[1]
#define TinRise                     tempControl.dtOfAverageARYW[0]
#define ToutRise                    tempControl.dtOfAverageARYW[1]
#define Chamber1Rise                tempControl.dtOfAverageARYW[2]
#define Chamber2Rise                tempControl.dtOfAverageARYW[3]
#define Chamber3Rise                tempControl.dtOfAverageARYW[4]
#define Chamber4Rise                tempControl.dtOfAverageARYW[5]

#define ADC_COUNT_MIDPOINT          5362        // ADC Half units for mid point
#define TEMPERATURE_MIDPOINT        133         // in °F

#define DegAt0VoltbelowMid          8.8f        // In °F
#define DegPerADHalfUnitbelowMid    (0.023304f)

#define DegAt0VoltAbvoveMid         133.75f      // In °F
//#define DegPerADHalfUnitAbvoveMid   (0.040418f)
//#define DegAbvoveMidTempConst       83.01f
#define DegPerADHalfUnitAbvoveMid   (0.033225f)     // values changed because mike reported that display shows 190 F but actual was 180F
#define DegAbvoveMidTempConst       44.4f

//#define DegAt0Volt                  10.6f               // In °F
#define DegPerADHalfUnit            ((DegPerADHalfUnitbelowMid + DegPerADHalfUnitAbvoveMid)/2)
#define ADHalfUnitPerDeg            (1./DegPerADHalfUnit)

#define ErrorLimitForKDToKickIn     (6 * ADHalfUnitPerDeg/2)
#define eeIntegralLimit             106200.0f

#define TDiffForShutDown                    (0)
#define TinMinimumRiseLimitForSignificant   (64)
#define ToutMaximumRiseLimitForSignificant  (24)
#define TinMaximumRiseForNoEvent            (-32)
#define ToutMinimumRiseForNoEvent           (32)

// Macros for Dry fire detection
#define DRY_FIRE_WAIT_TIME                  (10 * 2)        // *500 millisec
#define DRY_FIRE_THRESHOLD                  3200            // in ADC Half units of temperature
#define DRY_FIRE_THRESHOLD_DEFAULT          3913            //  100 in degree Farenheit count converted to adc counts  
#define DRY_FIRE_THRESHOLD_UPPER_LIMIT      100             //  100 in degree Farenheit will be maximum limit
#define DRY_FIRE_THRESHOLD_LOWER_LIMIT      1               //  1 in degree Farenheit will be minimum limit  
#define DRY_FIRE_THRESHOLD_DEFAULT_LIMIT    100             //  100 in degree Farenheit will be default value

//  CLASS METHOD PROTOTYPES

bool TemperatureControl(void);
void PIDCalculation(void);
uint16_t adcCountToTemperature(uint16_t adcCount);
uint16_t temperatureToADCCount(uint16_t temperature);

// EXTERN VARIABLES
extern TemperatureControl_STYP tempControl;

#endif /* _TEMPERATURECONTROL_H */

